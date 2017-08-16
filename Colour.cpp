/*
 * W - a simple, cross-platform 2D game develpoment library
 *
 * ==============
 *  Colour.cpp
 * ==============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Colour.h"

W::Colour::Colour(const W::Colour &col) : r(col.r), g(col.g), b(col.b), a(col.a)
{
	
}
W::Colour::Colour(float _r, float _g, float _b, float _a) :
	r(_r), g(_g), b(_b), a(_a)
{
	
}

bool W::Colour::operator==(const Colour &c) {
	return r == c.r && g == c.g && b == c.b && a == c.a;
}


namespace W {
	Colour Colour::Black(0, 0, 0);
	Colour Colour::TransparentBlack(0, 0, 0, 0.5);
	Colour Colour::White(1, 1, 1);
	Colour Colour::Red(1, 0, 0);
	Colour Colour::Green(0, 1, 0);
	Colour Colour::Blue(0, 0, 1);
	Colour Colour::Yellow(1, 1, 0);
	Colour Colour::Purple(0.47, 0, 0.78);
	Colour Colour::Orange(1, 0.6, 0.09);
	Colour Colour::SkyBlue(0.58, 0.90, 1);
	Colour Colour::Brown(0.62, 0.42, 0.25);
}
