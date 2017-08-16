/*
 * W - a tiny 2D game develpoment library
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
	
	class MTNode;
	
	class Texture {
	public:
		Texture(const std::string &_filename);
		~Texture();
			// Must be called from the thread on which the OpenGL context is current
		
		void incrementUsageCount() { ++usageCount; }
		void decrementUsageCount() { --usageCount; }
		
		MTNode *mtNode;
		int mtA, mtB;	// Pixel coordinates of the Texture within the MegaTexture
		size sz;		// Set by MT in addTex()
		
		float floatCoordA(int texA);
		float floatCoordB(int texB);
		float floatCoordC(int texC);
		float floatCoordD(int texD);
			// Take a pixel-coord relative to the tex, return a %-coord
			// relative to the MT.
		
		// PLACEHOLDER REQD STILL?
		// If the texture is uploaded into a "fresh" area of the MT,
		// will be transp. & don't need a placeholder transp. tex
		// But if uploaded over something that was deleted before, might
		// briefly see previous MT contents?
		
		static void _createWhiteTexture();
		static Texture *_whiteTexture;
			// Used for drawing coloured rects
		
	private:
		Texture() : mtNode(NULL), usageCount(0) { }
		std::string filename;
		int usageCount;
			// Number of DObjs referencing the texture
	};
	
}

#endif
