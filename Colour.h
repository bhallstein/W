/*
 * Colour.hpp - a colour
 *
 */

#ifndef __W__Colour
#define __W__Colour

#include <iostream>

namespace W {
	
	class Colour {
	public:
		Colour();
		Colour(const W::Colour &);
		Colour(float _r, float _g, float _b, float _a = 1);
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
	};
	
}

#endif
