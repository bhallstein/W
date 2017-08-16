#ifndef W_types_h
#define W_types_h

#include <string>
#include <sstream>
#include <vector>

namespace W {

	/* Coordinates & such */
	struct position {
		int x, y;
		float a, b;
		position() : x(0), y(0), a(0), b(0) { }
		position(int _x, int _y) : x(_x), y(_y), a(0), b(0) { }
		position(float _a, float _b) : x(0), y(0), a(_a), b(_b) { }
		position(int _x, int _y, float _a, float _b) : x(_x), y(_y), a(_a), b(_b) { }
		position operator+ (const position &pos2) {
			return position(
				x + pos2.x, y + pos2.y,
				a + pos2.a, b + pos2.b
			);
		}
		void operator+= (const position &pos2) {
			x += pos2.x, y += pos2.y;
			a += pos2.a, b += pos2.b;
		}
		position operator- (const position &pos2) {
			return position(
				x - pos2.x, y - pos2.y,
				a - pos2.a, b - pos2.b
			);
		}
		void operator-= (const position &pos2) {
			x -= pos2.x, y -= pos2.y;
			a -= pos2.a, b -= pos2.b;
		}
		bool operator== (const position &pos2) {
			return
				x == pos2.x &&
				y == pos2.y &&
				a == pos2.a &&
				b == pos2.b;
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
	struct size {
		int width, height;
		size() : width(0), height(0) { }
		size(int _width, int _height) : width(_width), height(_height) { }
		bool operator== (const size &s2) {
			return width == s2.width && height == s2.height;
		}
		std::string str() const {
			std::stringstream ss;
			ss << width << "x" << height;
			return ss.str();
		}
	};
	struct rect {
		struct position pos;
		struct size sz;
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
		Exception(const char *_msg) : msg(_msg) { }
		Exception(const std::string &_msg) : msg(_msg) { }
		Exception(const char *_msg, int _err);
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
	void upCase(std::string &);
	void downCase(std::string &);
	void strSplit(const std::string &s, std::vector<std::string> &v, char delim);
	void implode(const std::vector<std::string> &v, std::string &s, const char *glue = "");
	
	template <typename T>
	bool strToT(T &t, const std::string &s) {	// Convert (first part of) string to numeric type
		std::istringstream ss(s);
		return !(ss >> t).fail();
	}
	
}

#endif
