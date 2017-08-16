/*
 * W - a tiny 2D game develpoment library
 *
 * ============
 *  Colour.h
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__Colour
#define __W__Colour

#include <iostream>

namespace W {
	
	class Colour {
	public:
		Colour(const W::Colour &);
		Colour(float _r = 0, float _g = 0, float _b = 0, float _a = 1);
		bool operator==(const Colour &);
		float r;
		float g;
		float b;
		float a;
		
		static Colour Black;
		static Colour TransparentBlack;
		static Colour White;
		static Colour Red;
		static Colour Green;
		static Colour Blue;
		static Colour Yellow;
		static Colour Purple;
		static Colour Orange;
		static Colour SkyBlue;
		static Colour Brown;
	};
	
}

#endif
