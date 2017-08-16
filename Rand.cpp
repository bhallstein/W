#include "Rand.h"
#include <ctime>

#define A_BILLION 1000000000

struct W::Rand::Init {
	Init() {
		time_t thetime;
		time(&thetime);
		twister.seed(thetime);
	}
};

W::Rand::Init *W::Rand::init = new W::Rand::Init();
MTRand_int32 W::Rand::twister;

int W::Rand::intUpTo(int x) {
	if (x <= 0) return 0;
	return twister()%x;
}
float W::Rand::floatUpTo(float x) {
	float y = twister()%A_BILLION;
	return y/A_BILLION * x;
}
