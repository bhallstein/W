/*
 * W - a simple, cross-platform 2D game develpoment library
 *
 * =============
 *  Mutex.cpp
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

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

#if defined WTARGET_MAC || defined WTARGET_IOS

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

struct W::Mutex::Objs {
	CRITICAL_SECTION mutex;
};
void W::Mutex::createMutex() {
	if (objs) {
		W::log << "Mutex: createMutex() called, but 'objs' already exists\n";
		return;
	}
	objs = new Objs();
	InitializeCriticalSection(&objs->mutex);
}
void W::Mutex::destroyMutex() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
}
void W::Mutex::lock() {
	EnterCriticalSection(&objs->mutex);
}
void W::Mutex::unlock() {
	LeaveCriticalSection(&objs->mutex);
}

#endif
