/*
 * W - a simple, cross-platform 2D game develpoment library
 *
 * ===========
 *  Mutex.h
 * ===========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__Mutex
#define __W__Mutex

namespace  W {

	class Mutex {
	public:
		Mutex();
		~Mutex();
		void lock();
		void unlock();
	private:
		void createMutex();
		void destroyMutex();
		
		struct Objs;
		Objs *objs;
	};
	
}

#endif
