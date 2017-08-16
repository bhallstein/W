/*
 * W - a tiny 2D game development library
 *
 * =============
 *  Texture.h
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__Texture
#define __W__Texture

#include <string>

namespace W {
	
	class TANode;
	class TextureAtlas;
	
	class Texture {
		friend class TextureAtlas;
	public:
		Texture(const std::string &_filename, TextureAtlas *_atlas = NULL);
		~Texture();
			// Must be called from the thread on which the OpenGL context is current
		
		void incrementUsageCount() { ++usageCount; }
		void decrementUsageCount() { --usageCount; }
		
		float floatCoordA(/*int texA*/);
		float floatCoordB(/*int texB*/);
		float floatCoordC(/*int texC*/);
		float floatCoordD(/*int texD*/);
			// Take a pixel-coord relative to the tex, return a %-coord
			// relative to the MT.
		
		const size& getSize() { return sz; }
		
		// PLACEHOLDER REQD STILL?
		// If the texture is uploaded into a "fresh" area of the MT,
		// will be transp. & don't need a placeholder transp. tex
		// But if uploaded over something that was deleted before, might
		// briefly see previous MT contents?
		
		TextureAtlas *atlas;
		
	private:
		Texture() : taNode(NULL), usageCount(0) { }
		std::string filename;
		int usageCount;
			// Number of DObjs referencing the texture
		
		TANode *taNode;
		int atlasA, atlasB;	// Pixel coordinates of the Texture within the Atlas
		size sz;
			// All set by TextureAtlas in addTex()
	};
	
}

#endif
