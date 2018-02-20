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

namespace W {

	extern int DEFAULT_LAYER;
	
	namespace BlendMode {
		enum T { Normal, Additive };
	}
	
	
	/* Vector types */
	
	struct v2i {
		int a, b;
		
		v2i();
		v2i(int x);
		v2i(int a, int b);
		
		bool operator== (const v2i &v) const;
		bool operator!= (const v2i &v) const;
		v2i operator+ (const v2i &v) const;
		v2i operator- (const v2i &v) const;
		void operator+= (const v2i &v);
		void operator-= (const v2i &v);
		v2i operator*  (int x) const;
		v2i operator/  (int x) const;
		void operator*= (int x);
		void operator/= (int x);
		
		v2i& operator= (const v2i &);
		
		std::string str() const;
	};
	
	struct v2f {
		float a, b;
		
		v2f();
		v2f(float x);
		v2f(float a, float b);
		v2f(const v2i &);
		
		bool operator== (const v2f &v) const;
		bool operator!= (const v2f &v) const;
		v2f operator+ (const v2f &v) const;
		v2f operator- (const v2f &v) const;
		void operator+= (const v2f &v);
		void operator-= (const v2f &v);
		v2f operator*  (float x) const;
		v2f operator/  (float x) const;
		void operator*= (float x);
		void operator/= (float x);
		
		v2f& operator= (const v2f &);
		
		float dot(const v2f &v2) {
			return a*v2.a + b*v2.b;
		}
		float mod();
		v2f normalise();
		
		std::string str() const;
	};
	
	struct v3f {
		float a, b, c;
		v3f();
		v3f(float x);
		v3f(float a, float b, float c);
		
		bool operator== (const v3f &v) const;
		bool operator!= (const v3f &v) const;
		v3f operator+ (const v3f &v);
		v3f operator- (const v3f &v);
		void operator+= (const v3f &v);
		void operator-= (const v3f &v);
		v3f operator*  (float x) const;
		v3f operator/  (float x) const;
		void operator*= (float x);
		void operator/= (float x);
		
		v3f& operator= (const v3f &);
		v3f& operator= (const v2f &);
		
		std::string str() const;
	};
	
	
	class Colour;
	
	struct v4f {
		float a, b, c, d;
		v4f();
		v4f(float x);
		v4f(float a, float b, float c, float d);
		
		bool operator== (const v4f &v) const;
		bool operator!= (const v4f &v) const;
		v4f operator+ (const v4f &v) const;
		v4f operator- (const v4f &v) const;
		void operator+= (const v4f &v);
		void operator-= (const v4f &v);
		v4f operator*  (float x) const;
		v4f operator/  (float x) const;
		void operator*= (float x);
		void operator/= (float x);
		
		v4f& operator= (const v4f &);
		v4f& operator= (const Colour);
		
		std::string str() const;
	};
	
	
	struct iRect {
		v2i position;
		v2i size;
		
		iRect();
		iRect(v2i _p, v2i _s);
		
		bool overlapsWith(v2f) const;
		std::string str() const;
	};
	struct fRect {
		v2f position;
		v2f size;

		fRect();
		fRect(v2f _p, v2f _s);
		fRect(const iRect &);

		bool overlapsWith(v2f coord) const;
		std::string str() const;
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
	
	bool isNum(char c);
	std::string upCase(std::string);
	std::string downCase(std::string);
	void strSplit(std::string s, std::vector<std::string> &v, char delim);
	void implode(std::vector<std::string> v, std::string &s, const char *glue = "");
	
	template <typename T>
	bool strToT(T &t, std::string s) {	// Convert (first part of) string to numeric type
		std::istringstream ss(s);
		return !(ss >> t).fail();
	}

  namespace TextAlign {
    enum T { Left, Right, Centre };
  }
}

#endif
