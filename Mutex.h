/*
 * Mutex.h
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
