/*
 * W - a tiny 2D game development library
 *
 * ==========
 *  Rand.h
 * ==========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__Rand
#define __W__Rand

class MTRand_int32;

namespace W {
	
	class Rand {
	public:
		static int intUpTo(int x);				// int in the interval [0,x-1]
		static float floatUpTo(float x = 1.0);	// float in the interval [0..x]
		
	private:
		Rand() { }
		
		struct Init;
		static Init *init;
	};
}

#endif
