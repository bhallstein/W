/*
 * W - a tiny 2D game develpoment library
 *
 * ==================
 *  GenericRetro.h
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W_GenericRetro
#define __W_GenericRetro

#include <map>
#include "types.h"

namespace W {
	
	extern std::map<char,std::vector<rect>> GenericRetro;
	void initGenericRetro();
	
	struct _grInit;
	extern _grInit *_grInitializer;

}

#endif
