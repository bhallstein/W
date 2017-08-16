/*
 * W - a tiny 2D game development library
 *
 * ===============
 *  Texture.cpp
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "types.h"
#include "W.h"
#include "TextureAtlas.h"

W::Texture::Texture(const std::string &_filename, TextureAtlas *_atlas) :
	filename(_filename),
	atlas(_atlas ? _atlas : TextureAtlas::_defaultTexAtlas),
	taNode(NULL),
	usageCount(0)
{
	if (!atlas->addTex(filename, this))
		throw Exception("Error creating texture, see log for details");
}

W::Texture::~Texture()
{
	if (usageCount > 0) {
		std::stringstream ss;
		ss << "Error deallocating texture '" << filename << "': " << usageCount << " DObjs are still using it.";
		throw Exception(ss.str());
	}
	if (taNode) atlas->removeTex(taNode);
}

float W::Texture::floatCoordA(/*int texA*/) { return float(atlasA + /*texA +*/ 0.5) / atlas->width(); }
float W::Texture::floatCoordB(/*int texB*/) { return float(atlasB + /*texB +*/ 0.5) / atlas->width(); }
float W::Texture::floatCoordC(/*int texC*/) { return float(atlasA + sz.a - /*texC -*/ 0.5) / atlas->width(); }
float W::Texture::floatCoordD(/*int texD*/) { return float(atlasB + sz.b - /*texD -*/ 0.5) / atlas->width(); }
	// Note: texels are sampled in the middle. We add/subtract half a pixel to prevent unwanted edge blending.
	// See also http://www.mindcontrol.org/~hplus/graphics/opengl-pixel-perfect.html
