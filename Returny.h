/*
 * W - a tiny 2D game development library
 *
 * =============
 *  Returny.h
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__Returny__
#define __W__Returny__

#include <string>

namespace W {

	namespace ReturnyType {
		enum T {
			Empty,
			Killer,
			Payload		// has a string payload
		};
	}

	class Returny {
	public:
		Returny(ReturnyType::T _type) : type(_type) { }
		bool operator==(Returny &r) {
			return type == r.type;
		}
		ReturnyType::T type;
		std::string payload;
	};

	extern Returny KillerReturny;
	extern Returny EmptyReturny;
	
}

#endif
