/*
 * W - a tiny 2D game development library
 *
 * ============
 *  Rand.cpp
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Rand.h"
#include <ctime>
#include <iostream>

#define A_BILLION 1000000000

struct W::Rand::Init {
	Init() {
		time_t thetime;
		time(&thetime);
		twister.seed(thetime);
	}
};

MTRand_int32 W::Rand::twister;
W::Rand::Init *W::Rand::init = new W::Rand::Init();

int W::Rand::intUpTo(int x) {
	if (x <= 0) return 0;
	return twister()%x;
}
float W::Rand::floatUpTo(float x) {
	float y = twister()%A_BILLION;
	return y/A_BILLION * x;
}
