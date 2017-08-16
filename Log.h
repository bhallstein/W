/*
 * W - a tiny 2D game develpoment library
 *
 * =========
 *  Log.h
 * =========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__Log
#define __W__Log

#include <fstream>

namespace W {
	
	extern std::ofstream log;
	void setLogPath(const std::string &path);
	
	struct _WLogInit;
	extern _WLogInit *_logInit;
	
}

#endif
