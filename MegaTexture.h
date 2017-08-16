/*
 * W - a tiny 2D game development library
 *
 * =================
 *  MegaTexture.h
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__MegaTexture
#define __W__MegaTexture

#include "W.h"

#define N_CHANNELS 4		// RGBA :)

namespace  W {

	class MTNode;

	class MegaTexture {
	public:
		static bool addTex(const std::string &filename, W::Texture *);
			// Adds the texture and calls setModified(), returning false if impossible,
		static bool addTex(unsigned char *, const size &, W::Texture *);
		static void removeTex(MTNode *);
			// Sets the given area node as empty
		
		static void upload();
			// Reupload the MegaTexture to gfx memory (& clear was_modified).
		
		static void setModified() { was_modified = true; }
		static bool modified() { return was_modified; }
			// The user may make multiple changes to the MT per cycle; we
			// don't want to upload it more than once per cycle, though.
		
		static unsigned int getGLTexId() { return glTexId; }
		
		inline static unsigned int width() { return 1 << curTexPower; }
		
		static void debug(const std::string &saveto);
		
	private:
		
		static MTNode *topNode;
		
		static unsigned char *megatexture;	// The texture itself, resident in main memory
		static unsigned int glTexId;		// The gl name of the uploaded texture
		static unsigned int curTexPower;	// i.e. the power of 2, eg '8' => 256 x 256
		static void createMegaTex(int newTexPower);
			// Allocates the memory repr. of the megatexture, copying
			// any previous contents into the texture.
		
		// TODO: when MT size changes, existing textures’ coords will need updating.
		
		static bool was_modified;
		
		struct initializer;
		static initializer *init;
	};

	class MTNode {
	public:
		MTNode() : c1(NULL), c2(NULL), tex(NULL) { }
		~MTNode() {
			if (c1) delete c1;
			if (c2) delete c2;
		}
		MTNode* insert(const W::size &sz);
			// Attempt to insert image of size r here or among descendants
		
		W::rect rct;
		MTNode *c1, *c2;
		
		W::Texture *tex;
		
	private:
		inline bool isLeaf() { return !isBranch(); }
		inline bool isBranch() { return (c1 && c2); }
		inline bool isEmpty() { return (isLeaf() && !tex); }
		
		// A node is a leaf (`area` type) if it has no children.
		// Otherwise is a  branch (`split` type).
		// An `area` is empty if its `tex` property is NULL.
	};

}

// Further info on the method used to pack the MT:
// http://www.blackpawn.com/texts/lightmaps/default.html

#endif
