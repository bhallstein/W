#include "MegaTexture.h"
#include "Log.h"

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#elif defined _WIN32 || _WIN64
	#include <gl\gl.h>
	#include <gl\glu.h>
#endif

#include "SOIL.h"

#define MT_INITIAL_POWER 8	// i.e. 2^8 = 256
#define MT_MAX_POWER 10		// i.e. 1024
#define MTNODE_FULL_SIZE -1
	// used when the node stretches to the far side of the megatexture
	// (reqd since the MT may vary in size)

//#define MT_DEBUG

/*************************************/
/*** MegaTexture static properties ***/
/*************************************/

W::MTNode *W::MegaTexture::topNode;
bool W::MegaTexture::was_modified = false;
unsigned char *W::MegaTexture::megatexture = NULL;
unsigned int W::MegaTexture::curTexPower = 0;
unsigned int W::MegaTexture::glTexId = 0;
	// N.B. zero is not a valid opengl texture id and is ignored by,
	// for example, glDeleteTextures().


/**********************************/
/*** MegaTexture initialization ***/
/**********************************/

struct W::MegaTexture::initializer {
	initializer() {
		// Allocate main memory megatexture repr
		createMegaTex(MT_INITIAL_POWER);
		
		// Create initial node
		topNode = new MTNode;
		topNode->rct.sz = W::size(MTNODE_FULL_SIZE);
		
		// Create white texture for coloured rects
		Texture::_createWhiteTexture();
	}
};
W::MegaTexture::initializer *W::MegaTexture::init = new MegaTexture::initializer();


/**********************************/
/*** MegaTexture implementation ***/
/**********************************/

bool W::MegaTexture::addTex(const std::string &filename, W::Texture *_tex) {
	using std::string;
	W::size imageSz;
	int imChannels;
	unsigned char *imagedata = SOIL_load_image(
		filename.c_str(),
		&imageSz.width, &imageSz.height, &imChannels,
		SOIL_LOAD_RGBA	// Resulting array should thusly have 4 channels
	);
	if (!imagedata) {
		W::log << "Could not load texture '" << filename << "': " << "file may not exist, or is invalid";
		return false;
	}
	bool success = addTex(imagedata, imageSz, _tex);
	free(imagedata);
	
	#ifdef MT_DEBUG
		debug("/Users/bh/Desktop/mtdebug.bmp");
	#endif
	
	if (success) setModified();
	return success;
}
bool W::MegaTexture::addTex(unsigned char *imagedata, const size &imageSz, W::Texture *_tex) {
	// Find node to copy image into
	MTNode *n = NULL;
	while(curTexPower <= MT_MAX_POWER && !n) {
		n = topNode->insert(imageSz);
		if (!n) {
			if (curTexPower == MT_MAX_POWER) {
				W::log << "Could not load texture: MegaTexture at maximum capacity\n";
				return false;
			}
			createMegaTex(curTexPower+1);
		}
	}
	
	// Copy image into megatexture at given node
	int curMTSideLength = 1 << curTexPower;
	const int &w1 = imageSz.width, &h1 = imageSz.height;
	int &w2 = curMTSideLength;
	
	for (int j=0; j < h1; ++j)
		for (int i=0; i < w1*N_CHANNELS; ++i) {
			int x = j*w1*N_CHANNELS + i;
			int y = (j+n->rct.pos.y)*w2*N_CHANNELS + i + n->rct.pos.x*N_CHANNELS;
			megatexture[y] = imagedata[x];
		}
	
	// Set node & texture properties
	n->tex = _tex;
	_tex->mtNode = n;
	_tex->mtA = n->rct.pos.x;
	_tex->mtB = n->rct.pos.y;
	_tex->sz = imageSz;
	
	setModified();
	return true;
}
void W::MegaTexture::removeTex(MTNode *_n) {
	// No need either to reupload the mt or alter its data in any way --
	// can simply re-mark that part of the megatexture as available.
	_n->tex = NULL;
	// Check if size should dynamically stretch to edge of Megatex
	if (_n->rct.pos.x + _n->rct.sz.width == width())  _n->rct.sz.width = MTNODE_FULL_SIZE;
	if (_n->rct.pos.y + _n->rct.sz.height == width()) _n->rct.sz.height = MTNODE_FULL_SIZE;
}

void W::MegaTexture::createMegaTex(int newTexPower) {
	if (newTexPower < curTexPower) throw Exception("New MegaTexture size is smaller than previous size");
	
	int prevTexSize = 1 << curTexPower;
	int newTexSize = 1 << newTexPower;
	
	unsigned char *new_megatexture = (unsigned char *) calloc(newTexSize*newTexSize*N_CHANNELS, sizeof(unsigned char));
	
	if (megatexture != NULL) {
		for (int j=0; j < prevTexSize; ++j)
			for (int i=0; i < prevTexSize*N_CHANNELS; ++i)
				new_megatexture[j*newTexSize*N_CHANNELS + i] = megatexture[j*prevTexSize*N_CHANNELS + i];
		free(megatexture);
	}
	megatexture = new_megatexture;
	curTexPower = newTexPower;
	
	setModified();
}
void W::MegaTexture::upload() {
	if (megatexture == NULL) return;
	glDeleteTextures(1, &glTexId);
	glTexId = SOIL_create_OGL_texture(
		megatexture,
		1<<curTexPower, 1<<curTexPower, N_CHANNELS,
		SOIL_CREATE_NEW_ID,
		0
	);
	if (!glTexId) throw W::Exception("Error: could not upload the MegaTexture to the GPU");
	was_modified = false;
}
void W::MegaTexture::debug(const std::string &saveto) {
	SOIL_save_image(
		saveto.c_str(),
		SOIL_SAVE_TYPE_BMP,
		width(), width(), N_CHANNELS,
		megatexture
	);
}


/*****************************/
/*** MTNode implementation ***/
/*****************************/

W::MTNode* W::MTNode::insert(const W::size &imageSz) {
	if (isBranch()) {
		// If this is a `split` node, try to insert in descendants
		MTNode *retnode = c1->insert(imageSz);
		if (!retnode) retnode = c2->insert(imageSz);
		return retnode;
	}
	// This is an `area` node: try to insert, or create split if larger than image
	if (tex) return NULL;
	W::size nodeSize(
		rct.sz.width == MTNODE_FULL_SIZE ? MegaTexture::width() - rct.pos.x : rct.sz.width,
		rct.sz.height == MTNODE_FULL_SIZE ? MegaTexture::width() - rct.pos.y : rct.sz.height
	);
	if (imageSz.width > nodeSize.width || imageSz.height > nodeSize.height) return NULL;
	if (imageSz == nodeSize) return this;
	else {
		c1 = new MTNode, c2 = new MTNode;
		bool split_horizontally = (nodeSize.width - imageSz.width >= nodeSize.height - imageSz.height);
		if (split_horizontally) {
			c1->rct = W::rect(
				rct.pos,
				W::size(imageSz.width, rct.sz.height)
			);
			c2->rct = W::rect(
				W::position(rct.pos.x + imageSz.width, rct.pos.y),
				W::size(rct.sz.width == MTNODE_FULL_SIZE ? MTNODE_FULL_SIZE : nodeSize.width - imageSz.width, rct.sz.height)
			);
		}
		else {
			c1->rct = W::rect(
				rct.pos,
				W::size(rct.sz.width, imageSz.height)
			);
			c2->rct = W::rect(
				W::position(rct.pos.x, rct.pos.y + imageSz.height),
				W::size(rct.sz.width, rct.sz.height == MTNODE_FULL_SIZE ? MTNODE_FULL_SIZE : nodeSize.height - imageSz.height)
			);
		}
		return c1->insert(imageSz);
	}
}
