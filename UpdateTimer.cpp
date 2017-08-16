#include "UpdateTimer.h"
#include "types.h"
#include "W.h"

/******************************************/
/*** UpdateTimer: common implementation ***/
/******************************************/

W::UpdateTimer::UpdateTimer(Callback *_c) : objs(NULL), running(false), c(_c)
{
	createTimer();
}
W::UpdateTimer::~UpdateTimer()
{
	destroyTimer();
	delete c;
}


/************************************************/
/*** UpdateTimer: Mac-specific implementation ***/
/************************************************/

#ifdef WTARGET_MAC

#include "MacOSXClasses.h"

struct W::UpdateTimer::Objs {
	W_UpdateTimer *timer;
};

void W::UpdateTimer::createTimer() {
	if (objs) {
		W::log << "UpdateTimer: createTimer() called but 'objs' already exists";
		return;
	}
	objs = new Objs();
	objs->timer = [[W_UpdateTimer alloc] initWithCallback:c];
}
void W::UpdateTimer::destroyTimer() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
}
void W::UpdateTimer::start() { [objs->timer start]; }
void W::UpdateTimer::stop() { [objs->timer stop]; }


/************************************************/
/*** UpdateTimer: iOS-specific implementation ***/
/************************************************/

#elif defined WTARGET_IOS

#include "iOSClasses.h"

struct W::UpdateTimer::Objs {
	// ...
};

// ...


/************************************************/
/*** UpdateTimer: Win-specific implementation ***/
/************************************************/

#elif defined WTARGET_WIN

struct W::UpdateTimer::Objs {
	UINT timer;
};

W::Callback *_W_running_callback = NULL;
void CALLBACK _mmtCallback(UINT timerid, UINT uMsg, DWORD userdata, DWORD d1, DWORD d2) {
	if (_W_running_callback)
		_W_running_callback->call();
}
struct W::UpdateTimer::Init {
	Init() {
		MMRESULT mmr = timeBeginPeriod(2);
		if (mmr != TIMERR_NOERROR)
			throw Exception("UpdateTimer: Error setting timer resolution");
	}
};
W::UpdateTimer::Init *W::UpdateTimer::init = new W::UpdateTimer::Init();

void W::UpdateTimer::createTimer() {
	if (objs) {
		W::log << "UpdateTimer: createTimer() called but 'objs' already exists";
		return;
	}
	objs = new Objs();
	_W_running_callback = c;
}
void W::UpdateTimer::destroyTimer() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
}
void W::UpdateTimer::start() {
	// Create mmt
	MMRESULT mmr = timeSetEvent(20, 40, &_mmtCallback, NULL, TIME_PERIODIC);
	if (mmr == NULL)
		throw Exception("UpdateTimer: Error starting multimedia timer");
		objs->timer = mmr;
}
void W::UpdateTimer::stop() {
	timeKillEvent(objs->timer);
}

#endif
