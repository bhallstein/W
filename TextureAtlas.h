/*
 * W - a tiny 2D game development library
 *
 * ==================
 *  TextureAtlas.h
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__MegaTexture
#define __W__MegaTexture

#include "W.h"
#include <set>

#define N_CHANNELS 4		// RGBA :)

namespace  W {

	class TANode;
	class DTexturedShape;
	
	class TextureAtlas {
	public:
		TextureAtlas();
		bool addTex(const std::string &filename, W::Texture *);
			// Adds the texture and calls setModified(), returning false if impossible
		bool addTex(unsigned char *, v2i, W::Texture *);
		void removeTex(TANode *);
			// Sets the given area node as empty
		
		void upload();
			// Reupload the MegaTexture to gfx memory (& clear was_modified).
		
		void setModified() { modifiedAtlases.insert(this); }
		static std::set<TextureAtlas*> modifiedAtlases;
			// We don't want to reupload the atlas more than once per cycle, but the user
			// may upload N textures during that cycle.

		void addDrawable(DTexturedShape *d) { drawables.insert(d); }
		void remDrawable(DTexturedShape *d) { drawables.erase(d); }
		void updateTexcoords();
		std::set<DTexturedShape*> drawables;
			// When the atlas is resized, drawables using it must update their texture coords.
			// Therefore, we need to keep a list of referencing drawables to update.
		
		unsigned int getGLTexId() { return glTexId; }
		
		inline unsigned int width() { return 1 << curTexPower; }
		
		void debug(std::string saveto = "");
		
		static TextureAtlas* _defaultTexAtlas;
		
	private:
		TANode *topNode;
		
		unsigned char *data;		// The texture itself, resident in main memory
		unsigned int glTexId;		// The gl name of the uploaded texture
		unsigned int curTexPower;	// i.e. the power of 2, eg '8' => 256 x 256
		void allocate(int newTexPower);
			// Allocates the memory repr. of the megatexture, copying
			// any previous contents into the texture, and sets
			// was_reallocated to true
		
	};

}

// Further info on the method used to pack the MT:
// http://www.blackpawn.com/texts/lightmaps/default.html

#endif
