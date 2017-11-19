/*
 * W - a tiny 2D game development library
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

#include "Colour.h"

namespace W {
	int DEFAULT_LAYER = 1;
}


/**********************/
/* v2i implementation */
/**********************/

#pragma mark - v2i

W::v2i::v2i() : a(0), b(0) { }
W::v2i::v2i(int x) : a(x), b(x) { }
W::v2i::v2i(int _a, int _b) : a(_a), b(_b) { }

bool W::v2i::operator== (const v2i &v) const { return a == v.a && b == v.b; }
bool W::v2i::operator!= (const v2i &v) const { return a != v.a || b != v.b; }
W::v2i W::v2i::operator+ (const v2i &v) const { return v2i(a + v.a, b + v.b); }
W::v2i W::v2i::operator- (const v2i &v) const { return v2i(a - v.a, b - v.b); }
void W::v2i::operator+= (const v2i &v) { a += v.a; b += v.b; }
void W::v2i::operator-= (const v2i &v) { a -= v.a; b -= v.b; }
W::v2i W::v2i::operator* (int x) const { return v2i(a*x, b*x); }
W::v2i W::v2i::operator/ (int x) const { return v2i(a/x, b/x); }
void W::v2i::operator*= (int x) { a *= x; b *= x; }
void W::v2i::operator/= (int x) { a /= x; b /= x; }

W::v2i& W::v2i::operator= (const v2i &v) {
	a = v.a, b = v.b;
	return *this;
}

std::string W::v2i::str() const {
	std::stringstream ss;
	ss << a << "," << b;
	return ss.str();
}


/**********************/
/* v2f implementation */
/**********************/

#pragma mark - v2f

W::v2f::v2f() : a(0), b(0) { }
W::v2f::v2f(float x) : a(x), b(x) { }
W::v2f::v2f(float _a, float _b) : a(_a), b(_b) { }
W::v2f::v2f(const v2i &v) : a(v.a), b(v.b) { }

bool W::v2f::operator== (const v2f &v) const { return a == v.a && b == v.b; }
bool W::v2f::operator!= (const v2f &v) const { return a != v.a || b != v.b; }
W::v2f W::v2f::operator+ (const v2f &v) const { return v2f(a + v.a, b + v.b); }
W::v2f W::v2f::operator- (const v2f &v) const { return v2f(a - v.a, b - v.b); }
void W::v2f::operator+= (const v2f &v) { a += v.a; b += v.b; }
void W::v2f::operator-= (const v2f &v) { a -= v.a; b -= v.b; }
W::v2f W::v2f::operator* (float x) const { return v2f(a*x, b*x); }
W::v2f W::v2f::operator/ (float x) const { return v2f(a/x, b/x); }
void W::v2f::operator*= (float x) { a *= x; b *= x; }
void W::v2f::operator/= (float x) { a /= x; b /= x; }

W::v2f& W::v2f::operator= (const v2f &v) {
	a = v.a, b = v.b;
	return *this;
}

std::string W::v2f::str() const {
	std::stringstream ss;
	ss << a << "," << b;
	return ss.str();
}


/**********************/
/* v3f implementation */
/**********************/

#pragma mark - v3f

W::v3f::v3f() : a(0), b(0), c(0) { }
W::v3f::v3f(float x) : a(x), b(x), c(x) { }
W::v3f::v3f(float _a, float _b, float _c) : a(_a), b(_b), c(_c) { }

bool W::v3f::operator== (const v3f &v) const { return a == v.a && b == v.b && c == v.c; }
bool W::v3f::operator!= (const v3f &v) const { return a != v.a || b != v.b || c != v.c; }
W::v3f W::v3f::operator+ (const v3f &v) { return v3f(a + v.a, b + v.b, c + v.c); }
W::v3f W::v3f::operator- (const v3f &v) { return v3f(a - v.a, b - v.b, c - v.c); }
void W::v3f::operator+= (const v3f &v) { a += v.a; b += v.b; c += v.c; }
void W::v3f::operator-= (const v3f &v) { a -= v.a; b -= v.b; c -= v.c; }
W::v3f W::v3f::operator* (float x) const { return v3f(a*x, b*x, c*x); }
W::v3f W::v3f::operator/ (float x) const { return v3f(a/x, b/x, c/x); }
void W::v3f::operator*= (float x) { a *= x; b *= x; c *= x; }
void W::v3f::operator/= (float x) { a /= x; b /= x; c /= x; }

W::v3f& W::v3f::operator= (const v3f &v) {
	a = v.a, b = v.b, c = v.c;
	return *this;
}
W::v3f& W::v3f::operator= (const v2f &v) {
	a = v.a, b = v.b, c = 0;
	return *this;
}

std::string W::v3f::str() const {
	std::stringstream ss;
	ss << a << "," << b << "," << c;
	return ss.str();
}

/**********************/
/* v4f implementation */
/**********************/

#pragma mark - v4f

W::v4f::v4f() : a(0), b(0), c(0), d(0) { }
W::v4f::v4f(float x) : a(x), b(x), c(x), d(x) { }
W::v4f::v4f(float _a, float _b, float _c, float _d) : a(_a), b(_b), c(_c), d(_d) { }

bool W::v4f::operator== (const v4f &v) const { return a == v.a && b == v.b && c == v.c && d == v.d; }
bool W::v4f::operator!= (const v4f &v) const { return a != v.a || b != v.b || c != v.c || d != v.d; }
W::v4f W::v4f::operator+ (const v4f &v) const { return v4f(a + v.a, b + v.b, c + v.c, d + v.d); }
W::v4f W::v4f::operator- (const v4f &v) const { return v4f(a - v.a, b - v.b, c - v.c, d - v.d); }
void W::v4f::operator+= (const v4f &v) { a += v.a; b += v.b; c += v.c; d += v.d; }
void W::v4f::operator-= (const v4f &v) { a -= v.a; b -= v.b; c -= v.c; d -= v.d; }
W::v4f W::v4f::operator* (float x) const { return v4f(a*x, b*x, c*x, d*x); }
W::v4f W::v4f::operator/ (float x) const { return v4f(a/x, b/x, c/x, d/x); }
void W::v4f::operator*= (float x) { a *= x; b *= x; c *= x; d *= x; }
void W::v4f::operator/= (float x) { a /= x; b /= x; c /= x; d /= x; }

W::v4f& W::v4f::operator= (const v4f &v) {
	a = v.a, b = v.b, c = v.c, d = v.d;
	return *this;
}
W::v4f& W::v4f::operator= (Colour col) {
	a = col.r;
	b = col.g;
	c = col.b;
	d = col.a;
	return *this;
}

std::string W::v4f::str() const {
	std::stringstream ss;
	ss << a << "," << b << "," << c << "," << d;
	return ss.str();
}


/***********************/
/* rect implementation */
/***********************/

#pragma mark - rect

W::iRect::iRect() { }
W::iRect::iRect(v2i _p, v2i _s) : position(_p), size(_s) { }

bool W::iRect::overlapsWith(const v2i &p) const {
	return
		p.a >= position.a && p.a < position.a + size.a &&
		p.b >= position.b && p.b < position.b + size.b;
}
std::string W::iRect::str() const {
	return position.str() + std::string("/") + size.str();
}


W::fRect::fRect() { }
W::fRect::fRect(v2f _p, v2f _s) : position(_p), size(_s) { }
W::fRect::fRect(const iRect &r) : position(r.position), size(r.size) { }

bool W::fRect::overlapsWith(const v2f &p) const {
	return
		p.a >= position.a && p.a < position.a + size.a &&
		p.b >= position.b && p.b < position.b + size.b;
}
std::string W::fRect::str() const {
	return position.str() + std::string("/") + size.str();
}


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

std::string W::upCase(const std::string &s) {
	std::stringstream ss;
	for (std::string::const_iterator it = s.begin(); it < s.end(); it++) {
		char c = *it;
		if (c >= 'a' && c <= 'z')
			c += 'A' - 'a';
		ss << c;
	}
	return ss.str();
}
std::string W::downCase(const std::string &s) {
	std::stringstream ss;
	for (std::string::const_iterator it = s.begin(); it < s.end(); it++) {
		char c = *it;
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		ss << c;
	}
	return ss.str();
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
