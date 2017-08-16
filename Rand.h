/*
 * Rand.h - fast, decent random numbers via mtrand
 *
 */

#ifndef __W__Rand
#define __W__Rand

#include "mtrand.h"

namespace W {
	
	class Rand {
	public:
		static int intUpTo(int x);				// int in the interval [0,x-1]
		static float floatUpTo(float x = 1.0);	// float in the interval [0..x]
		
	private:
		Rand() { }
		static MTRand_int32 twister;
		
		struct Init;
		static Init *init;
	};
}

#endif
