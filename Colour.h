/*
 * W - a tiny 2D game development library
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
#include <sstream>

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
		
		static const Colour Black;
		static const Colour TransparentBlack;
		static const Colour White;
		static const Colour Red;
		static const Colour Green;
		static const Colour Blue;
		static const Colour Yellow;
		static const Colour Purple;
		static const Colour Pink;
		static const Colour Orange;
		static const Colour SkyBlue;
		static const Colour Brown;
		
		std::string str() {
			std::stringstream ss;
			ss << "r" << r << ",g" << g << ",b" << b << ",a" << a;
			return ss.str();
		}
	};
	
}

#endif
