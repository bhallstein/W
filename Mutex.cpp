#include "Mutex.h"
#include "types.h"
#include "Log.h"

W::Mutex::Mutex() : objs(NULL)
{
	createMutex();
}
W::Mutex::~Mutex()
{
	destroyMutex();
}

/******************************************/
/*** Mutex: Mac-specific implementation ***/
/******************************************/

#ifdef WTARGET_MAC

#include <pthread.h>

struct W::Mutex::Objs {
	pthread_mutex_t mutex;
};
void W::Mutex::createMutex() {
	if (objs) {
		W::log << "Mutex: createMutex() called, but 'objs' already exists\n";
		return;
	}
	objs = new Objs();
	if (!!pthread_mutex_init(&objs->mutex, NULL))
		throw Exception("Couldn't initialize mutex");
}
void W::Mutex::destroyMutex() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
}
void W::Mutex::lock() {
	if (int err = pthread_mutex_lock(&objs->mutex))
		throw W::Exception("Error locking mutex", err);
}
void W::Mutex::unlock() {
	if (int err = pthread_mutex_unlock(&objs->mutex))
		throw W::Exception("Error unlocking mutex", err);
}

/******************************************/
/*** Mutex: Win-specific implementation ***/
/******************************************/

#elif defined WTARGET_WIN

#include "Windows.h"

struct W::Objs {
	CRITICAL_SECTION mutex;
};
void createMutex() {
	if (objs) {
		W::log << "Mutex: createMutex() called, but 'objs' already exists\n";
		return;
	}
	objs = new Objs();
	InitializeCriticalSection(&objs->mutex);
}
void destroyMutex() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
}
void lock() {
	EnterCriticalSection(m);
}
void unlock() {
	LeaveCriticalSection(m);
}

#endif
