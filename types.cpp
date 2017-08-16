/*
 * W - a tiny 2D game develpoment library
 *
 * =============
 *  types.cpp
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "types.h"
#include <sstream>

#ifdef WTARGET_MAC
	#include <Cocoa/Cocoa.h>
#elif defined WTARGET_IOS
	#include <Foundation/Foundation.h>
#elif defined WTARGET_WIN
	#include "Windows.h"
#endif

/********************/
/*** W::Exception ***/
/********************/

W::Exception::Exception(const std::string &_msg, int _err)
{
	std::stringstream ss;
	ss << _msg << " (error: " << _err << ")";
	msg = ss.str();
}


/*************************/
/*** Stringy functions ***/
/*************************/

bool W::isNum(const char c) {
	return (c >= '0' && c <= '9');
}

void W::upCase(std::string &s) {
	for (std::string::iterator it = s.begin(); it < s.end(); it++) {
		char &c = *it;
		if (c >= 'a' && c <= 'z')
			c += 'A' - 'a';
	}
}
void W::downCase(std::string &s) {
	for (std::string::iterator it = s.begin(); it < s.end(); it++) {
		char &c = *it;
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
	}
}

void W::strSplit(const std::string &s, std::vector<std::string> &v, char delim) {
	v.clear();
	std::stringstream ss(s);
	std::string chunk;
	while(std::getline(ss, chunk, delim))
		v.push_back(chunk);
}

void W::implode(const std::vector<std::string> &v, std::string &s, const char *glue) {
	int i=0;
	std::stringstream ss;
	for (std::vector<std::string>::const_iterator it = v.begin(); it < v.end(); it++, i++) {
		if (i > 0) ss << glue;
		ss << *it;
	}
	s = ss.str();
}


/********************************/
/*** File/Directory functions ***/
/********************************/

bool W::isValidDir(const std::string &path) {
	return isValidDir(path.c_str());
}
bool W::isValidDir(const char *path) {
	#ifdef __APPLE__
		BOOL isdir;
		[[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithCString:path encoding:NSUTF8StringEncoding]
											 isDirectory:&isdir];
		return isdir;
	#elif defined _WIN32 || _WIN64
		DWORD dw = GetFileAttributes(path);
		return (dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_DIRECTORY));
	#endif
}
bool W::createDir(const std::string &path) {
	return createDir(path.c_str());
}
bool W::createDir(const char *dir) {
	#ifdef __APPLE__
		return [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithCString:dir encoding:NSUTF8StringEncoding]
										 withIntermediateDirectories:YES
														  attributes:nil
															   error:nil];
	#elif defined _WIN32 || _WIN64
		return CreateDirectory(dir, NULL);
	#endif
}
