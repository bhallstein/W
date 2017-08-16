/*
 * W - a tiny 2D game development library
 *
 * ====================
 *  TextureAtlas.cpp
 * ====================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "TextureAtlas.h"
#include "Log.h"
#include "Drawable.h"
#include "W_internal.h"

#include "oglInclude.h"
#include "SOIL.h"

#define TA_INITIAL_POWER 8	// i.e. 2^8 = 256
#define TA_MAX_POWER 10		// i.e. 1024
#define TANODE_FULL_SIZE -1
	// used when the node stretches to the far side of the megatexture
	// (reqd since the TA may vary in size)

#define TA_DEBUG

#pragma mark TANode

/**************/
/*** TANode ***/
/**************/

namespace W {

	class TANode {
	public:
		TANode() : c1(NULL), c2(NULL), tex(NULL) { }
		~TANode() {
			if (c1) delete c1;
			if (c2) delete c2;
		}
		TANode* insert(const W::size &sz, int atlasCurSideLength);
			// Attempt to insert image of size r here or among descendants
		
		W::rect rct;
		TANode *c1, *c2;
		
		W::Texture *tex;
		
	private:
		inline bool isLeaf() { return !isBranch(); }
		inline bool isBranch() { return (c1 && c2); }
		inline bool isEmpty() { return (isLeaf() && !tex); }
		
		// A node is a leaf (`area` type) if it has no children.
		// Otherwise is a branch (`split` type).
		// An `area` node is empty if its `tex` property is NULL.
	};
	
}


#pragma mark - TextureAtlas

/***********************************/
/*** TextureAtlas implementation ***/
/***********************************/

std::set<W::TextureAtlas*> W::TextureAtlas::modifiedAtlases;
W::TextureAtlas *W::TextureAtlas::_defaultTexAtlas = new W::TextureAtlas();

W::TextureAtlas::TextureAtlas() :
	data(NULL),
	curTexPower(0),
	glTexId(0)
{
	allocate(TA_INITIAL_POWER);
	
	// Create initial node
	topNode = new TANode;
	topNode->rct.sz = size(TANODE_FULL_SIZE);
}


bool W::TextureAtlas::addTex(const std::string &filename, Texture *_tex) {
	size imageSz;
	int imChannels;
	unsigned char *imagedata = SOIL_load_image(
		filename.c_str(),
		&imageSz.width, &imageSz.height, &imChannels,
		SOIL_LOAD_RGBA	// Resulting array should thusly have 4 channels
	);
	if (!imagedata) {
		log << "Could not load texture '" << filename << "': " << "file may not exist, or is invalid";
		return false;
	}
	bool success = addTex(imagedata, imageSz, _tex);
	free(imagedata);
	
	#ifdef TA_DEBUG
		debug();
	#endif
	
	return success;
}
bool W::TextureAtlas::addTex(unsigned char *imagedata, const size &imageSz, W::Texture *_tex) {
	// Find node to copy image into
	TANode *n = NULL;
	while(curTexPower <= TA_MAX_POWER && !n) {
		n = topNode->insert(imageSz, 1 << curTexPower);
		if (!n) {
			if (curTexPower == TA_MAX_POWER) {
				log << "Could not load texture: MegaTexture at maximum capacity\n";
				return false;
			}
			allocate(curTexPower+1);
		}
	}
	
	// Copy image into megatexture at given node
	int curAtlasSideLength = 1 << curTexPower;
	const int &w1 = imageSz.width, &h1 = imageSz.height;
	int &w2 = curAtlasSideLength;
	
	for (int j=0; j < h1; ++j)
		for (int i=0; i < w1*N_CHANNELS; ++i) {
			int x = j*w1*N_CHANNELS + i;
			int y = (j+n->rct.pos.y)*w2*N_CHANNELS + i + n->rct.pos.x*N_CHANNELS;
			data[y] = imagedata[x];
		}
	
	// Set node & texture properties
	n->tex = _tex;
	_tex->taNode = n;
	_tex->atlasA = n->rct.pos.x;
	_tex->atlasB = n->rct.pos.y;
	_tex->sz = imageSz;
		
	setModified();
	return true;
}
void W::TextureAtlas::removeTex(TANode *_n) {
	// No need either to reupload the TA or alter its data in any way --
	// can simply re-mark that part of the atlas as available.
	_n->tex = NULL;
	// Check if size should dynamically stretch to edge of Megatex
	if (_n->rct.pos.x + _n->rct.sz.width == width())  _n->rct.sz.width = TANODE_FULL_SIZE;
	if (_n->rct.pos.y + _n->rct.sz.height == width()) _n->rct.sz.height = TANODE_FULL_SIZE;
}

void W::TextureAtlas::allocate(int newTexPower) {
	if (newTexPower < curTexPower) throw Exception("New MegaTexture size is smaller than previous size");
	
	int prevTexSize = 1 << curTexPower;
	int newTexSize = 1 << newTexPower;
	
	unsigned char *new_data = (unsigned char *) calloc(newTexSize*newTexSize*N_CHANNELS, sizeof(unsigned char));
	
	if (data) {
		for (int j=0; j < prevTexSize; ++j)
			for (int i=0; i < prevTexSize*N_CHANNELS; ++i)
				new_data[j*newTexSize*N_CHANNELS + i] = data[j*prevTexSize*N_CHANNELS + i];
		free(data);
	}
	data = new_data;
	curTexPower = newTexPower;
	
	updateTexcoords();
	
	setModified();
}
void W::TextureAtlas::updateTexcoords() {
	for (std::set<DTexturedShape*>::iterator it = drawables.begin(); it != drawables.end(); ++it)
		(*it)->recopyT();
}
void W::TextureAtlas::upload() {
	if (!data) return;
	glDeleteTextures(1, &glTexId);
	glTexId = SOIL_create_OGL_texture(
		data,
		1<<curTexPower, 1<<curTexPower, N_CHANNELS,
		SOIL_CREATE_NEW_ID,
		0
	);
	if (!glTexId) throw W::Exception("Error: could not upload TextureAtlas to the GPU");
	
	// Set params
	oglState.bindAtlas(this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
int debugCount = 0;
void W::TextureAtlas::debug(std::string f) {
	if (f == "") {
		std::stringstream ss;
		ss << "/Users/bh/Desktop/mtdebug-" << this << "_" << ++debugCount << ".bmp";
		f = ss.str();
	}
	SOIL_save_image(
		f.c_str(),
		SOIL_SAVE_TYPE_BMP,
		width(), width(), N_CHANNELS,
		data
	);
}


#pragma mark - TANode::insert()

/*****************************/
/*** TANode implementation ***/
/*****************************/

W::TANode* W::TANode::insert(const W::size &imageSz, int atlasCurSideLength) {
	if (isBranch()) {
		// If this is a `split` node, try to insert in descendants
		TANode *retnode = c1->insert(imageSz, atlasCurSideLength);
		if (!retnode) retnode = c2->insert(imageSz, atlasCurSideLength);
		return retnode;
	}
	// This is an `area` node: try to insert, or create split if larger than image
	if (tex) return NULL;
	size nodeSize(
		rct.sz.width == TANODE_FULL_SIZE ? atlasCurSideLength - rct.pos.x : rct.sz.width,
		rct.sz.height == TANODE_FULL_SIZE ? atlasCurSideLength - rct.pos.y : rct.sz.height
	);
	if (imageSz.width > nodeSize.width || imageSz.height > nodeSize.height) return NULL;
	if (imageSz == nodeSize) return this;
	else {
		c1 = new TANode, c2 = new TANode;
		bool split_horizontally = (nodeSize.width - imageSz.width >= nodeSize.height - imageSz.height);
		if (split_horizontally) {
			c1->rct = W::rect(
				rct.pos,
				W::size(imageSz.width, rct.sz.height)
			);
			c2->rct = rect(
				position(rct.pos.x + imageSz.width, rct.pos.y),
				size(rct.sz.width == TANODE_FULL_SIZE ? TANODE_FULL_SIZE : nodeSize.width - imageSz.width, rct.sz.height)
			);
		}
		else {
			c1->rct = W::rect(
				rct.pos,
				W::size(rct.sz.width, imageSz.height)
			);
			c2->rct = W::rect(
				position(rct.pos.x, rct.pos.y + imageSz.height),
				size(rct.sz.width, rct.sz.height == TANODE_FULL_SIZE ? TANODE_FULL_SIZE : nodeSize.height - imageSz.height)
			);
		}
		return c1->insert(imageSz, atlasCurSideLength);
	}
}
