/*
 * W - a tiny 2D game development library
 *
 * ===========
 *  types.h
 * ===========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef W_types_h
#define W_types_h

#include <string>
#include <sstream>
#include <vector>


/* Target identification */

#ifdef __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE
		#define WTARGET_IOS
	#elif TARGET_OS_MAC
		#define WTARGET_MAC
	#endif

#elif defined _WIN32 || _WIN64
	#define WTARGET_WIN

#endif

#include "Colour.h"

namespace W {

	/* Coordinates & such */
	struct size {
		int width, height;
		size() : width(0), height(0) { }
		size(int _s) : width(_s), height(_s) { }
		size(int _w, int _h) : width(_w), height(_h) { }
		bool operator== (const size &sz) const { return (width == sz.width && height == sz.height); }
		bool operator!= (const size &sz) const { return (width != sz.width || height != sz.height); }
		size operator+  (const size &sz) const { return size(width+sz.width,height+sz.height); }
		size operator-  (const size &sz) const { return size(width-sz.width,height-sz.height); }
		void operator+= (const size &sz) { width += sz.width; height += sz.height; }
		void operator-= (const size &sz) { width -= sz.width; height -= sz.height; }
		size operator*  (float x) const { return size(width*x,height*x); }
		size operator*  (int x)   const { return size(width*x,height*x); }
		size operator/  (float x) const { return size(width/x,height/x); }
		size operator/  (int x)   const { return size(width/x,height/x); }
		void operator*= (float x) { width *= x; height *= x; }
		void operator*= (int x)   { width *= x; height *= x; }
		void operator/= (float x) { width /= x; height /= x; }
		void operator/= (int x)   { width /= x; height /= x; }
		std::string str() const {
			std::stringstream ss;
			ss << width << "x" << height;
			return ss.str();
		}
	};
	struct position {
		int x, y;
		float a, b;
		position() : x(0), y(0), a(0), b(0) { }
		position(int _x, int _y) : x(_x), y(_y), a(0), b(0) { }
		position(float _a, float _b) : x(0), y(0), a(_a), b(_b) { }
		position(int _x, int _y, float _a, float _b) : x(_x), y(_y), a(_a), b(_b) { }
		position operator+ (const position &pos2) const {
			return position(
							x + pos2.x, y + pos2.y,
							a + pos2.a, b + pos2.b
							);
		}
		void operator+= (const position &pos2) {
			x += pos2.x, y += pos2.y;
			a += pos2.a, b += pos2.b;
		}
		position operator- (const position &pos2) const {
			return position(
							x - pos2.x, y - pos2.y,
							a - pos2.a, b - pos2.b
							);
		}
		void operator-= (const position &pos2) {
			x -= pos2.x, y -= pos2.y;
			a -= pos2.a, b -= pos2.b;
		}
		bool operator== (const position &pos2) const {
			return
			x == pos2.x &&
			y == pos2.y &&
			a == pos2.a &&
			b == pos2.b;
		}
		bool operator!= (const position &pos2) const {
			return
			x != pos2.x ||
			y != pos2.y ||
			a != pos2.a ||
			b != pos2.b;
		}
		position operator+ (const size &sz) const {
			return position(x + sz.width, y + sz.height);
		}
		void operator+= (const size &sz) {
			x += sz.width, y += sz.height;
		}
		position operator- (const size &sz) const {
			return position(x - sz.width, y - sz.height);
		}
		void operator-= ( const size &sz) {
			x -= sz.width, y -= sz.height;
		}
		
		std::string xyStr() const {
			std::stringstream ss;
			ss << x << "," << y;
			return ss.str();
		}
		std::string abStr() const {
			std::stringstream ss;
			ss << a << "," << b;
			return ss.str();
		}
		std::string str() const {
			return xyStr() + "[" + abStr() + "]";
		}
	};
	struct rect {
		W::position pos;
		W::size sz;
		rect() { }
		rect(position _pos, size _sz) : pos(_pos), sz(_sz) { }
		void setPos(const W::position &_pos) { pos = _pos; }
		void setSz(const W::size &_sz) { sz = _sz; }
		bool overlapsWith(const W::position &p) const {
			float x = pos.x + pos.a, y = pos.y + pos.b;
			return (p.x >= x && p.x < x + sz.width && p.y >= y && p.y < y + sz.height);
		}
		std::string str() const {
			return pos.str() + std::string("/") + sz.str();
		}
	};

	/* Exceptions */
	class Exception : public std::exception {
	public:
		Exception(const std::string &_msg) : msg(_msg) { }
		Exception(const std::string &_msg, int _err);
		~Exception() throw() { }
		const char* what() {
			return msg.c_str();
		}
	private:
		std::string msg;
	};
	
	/* String-related helper functions */
	bool isNum(const char c);
	std::string upCase(const std::string &);
	std::string downCase(const std::string &);
	void strSplit(const std::string &s, std::vector<std::string> &v, char delim);
	void implode(const std::vector<std::string> &v, std::string &s, const char *glue = "");
	
	template <typename T>
	bool strToT(T &t, const std::string &s) {	// Convert (first part of) string to numeric type
		std::istringstream ss(s);
		return !(ss >> t).fail();
	}

	/* File/Directory helper functions */
	bool isValidDir(const std::string &path);
	bool isValidDir(const char *path);
	bool createDir(const std::string &path);
	bool createDir(const char *path);
	
	// Vertexy stuff
	struct v3f {
		float x, y, z;
		std::string str() {
			std::stringstream ss;
			ss << '{' << x << ',' << y << ',' << z << '}';
			return ss.str();
		}
	};
	struct c4f {
		float r, g, b, a;
		std::string str() {
			std::stringstream ss;
			ss << "{r:" << r << ",b:" << b << ",g:" << g << ",a:" << a << "}";
			return ss.str();
		}
		void operator= (const W::Colour &c) {
			r = c.r, g = c.g, b = c.b, a = c.a;
		}
	};
	struct t2f {
		float x, y;
		std::string str() {
			std::stringstream ss;
			ss << '{' << x << ',' << y << '}';
			return ss.str();
		}
	};
	
}

#endif
