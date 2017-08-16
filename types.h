#ifndef W_types_h
#define W_types_h

#include <string>

namespace W {

	/* Coordinates & such */
	struct position {
		int x, y;
		float a, b;
		position() : x(0), y(0), a(0), b(0) { }
		position(int _x, int _y) : x(_x), y(_y), a(0), b(0) { }
		position(float _a, float _b) : x(0), y(0), a(_a), b(_b) { }
		position(int _x, int _y, float _a, float _b) : x(_x), y(_y), a(_a), b(_b) { }
	};
	struct size {
		int width, height;
		size() : width(0), height(0) { }
		size(int _width, int _height) : width(_width), height(_height) { }
	};
	struct rect {
		struct position pos;
		struct size sz;
	};

	/* Exceptions */
	class Exception : public std::exception {
	public:
		Exception(const char *s) : msg(s) { }
		~Exception() throw() { }
		virtual const char* what() {
			return msg.c_str();
		}
	private:
		std::string msg;
	};
	
}

#endif
