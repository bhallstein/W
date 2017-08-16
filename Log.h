/*
 * Log.h
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
