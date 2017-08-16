#include "W.h"
#include <iostream>
#include <ctime>

#ifdef __APPLE__
	#include <Cocoa/Cocoa.h>
	#include "ObjC-Classes.h"
	#include <OpenGL/gl.h>
#elif defined _WIN32 || _WIN64
	/*#include "shlobj.h"*/
	#include <gl\gl.h>
	#include <gl\glu.h>
#endif

#include "Messenger.h"

namespace W {
	std::string   logFilePath, logFileName;
	std::ofstream log;
	
	MTRand_int32 twister;
	
	std::vector<GameState*> _gs;
	bool _gsShouldPop;
	Returny _returny(ReturnyType::EMPTY_RETURNY);
	bool _locked_gfx_mutex_for_gs_popping = false;
	
	std::vector<Event> _events;
	
	std::vector<Texture*> _textures_to_upload;
	std::vector<Texture*> _textures_to_unload;
	
	Window *_window = NULL;

	#ifdef __APPLE__
		pthread_t _drawingThread;
	#elif defined _WIN32 || _WIN64
		DWORD _drawingThreadId;
		HANDLE _drawingThreadHandle;
	#endif
	
	#ifdef __APPLE__
		void * _updateTimer;
	#elif defined _WIN32 || _WIN64
		UINT _updateTimer;
	#endif
	
	bool _quit = false;
	
	#ifdef __APPLE__
		pthread_mutex_t
	#elif defined _WIN32 || _WIN64
		CRITICAL_SECTION
	#endif
		event_mutex,
		graphics_mutex,
		texture_mutex;
	
	int W_MAXPATH = 450;
}


/* Initialisation */

struct W::_init {
	_init() {
		// Default log: /dev/null
		std::string p;
		#ifdef __APPLE__
			p = "/dev/null";
		#elif defined _WIN32 || _WIN64
			p = "\\Device\\Null";
		#endif
		setLogFile(p.c_str());
		
		// Init MTRand
		time_t thetime;
		time(&thetime);
		twister.seed(thetime);
		
		// Init timers
		#ifdef __APPLE__
			_updateTimer = [[UpdateTimer alloc] init];
		#elif defined _WIN32 || _WIN64
			// Set timer resolution
			MMRESULT mmr = timeBeginPeriod(2);
			if (mmr != TIMERR_NOERROR)
				throw Exception("Error setting windows timer resolution");
		#endif
		
		// Init mutexes
		#ifdef __APPLE__
			if (!!pthread_mutex_init(&event_mutex, NULL))
				throw Exception("Couldn't initialize event vector mutex");
			if (!!pthread_mutex_init(&graphics_mutex, NULL))
				throw Exception("Couldn't initialize graphics mutex");
			if (!!pthread_mutex_init(&texture_mutex, NULL))
				throw Exception("Couldn't initialize texture mutex");
		#elif defined _WIN32 | _WIN64
			InitializeCriticalSection(&event_mutex);
			InitializeCriticalSection(&graphics_mutex);
			InitializeCriticalSection(&texture_mutex);
		#endif
		
		W::log << "W app inited" << std::endl;
	}
};
struct W::_init *W::_initializer = new W::_init();


/* Logging */

void W::setLogFile(const char *path) {
	if (log.is_open()) log.close();
	log.open(path);
}
void W::setLogFile(const std::string &path) {
	if (log.is_open()) log.close();
	log.open(path.c_str());
}


/* MTRand */

unsigned int W::randUpTo(int x) {
	if (x == 0) return 0;
	return twister()%x;
}


/* GameState pushing/popping */

void W::pushState(GameState *g) {
	_lock_mutex(&graphics_mutex);
	for (std::vector<GameState*>::iterator it = _gs.begin(); it < _gs.end(); it++)
		if (*it == g) return;
//	if (!states.empty()) states.back()->pause();
	_gs.push_back(g);
	Messenger::_setActiveGamestate(g);
	_unlock_mutex(&graphics_mutex);
}
void W::popState(W::Returny &r) {
	_gsShouldPop = true;
	_returny = r;
}
void W::_performPop() {
	_locked_gfx_mutex_for_gs_popping = true;
	_lock_mutex(&graphics_mutex);
	do {
		_gsShouldPop = false;
		GameState *last_gs = _gs.back();
		delete _gs.back();
		Messenger::_gamestateDestroyed(last_gs);
		_gs.pop_back();
		if (_gs.size()) {
			GameState *new_gs = _gs.back();
			Messenger::_setActiveGamestate(new_gs);
			new_gs->resume(&_returny);
		}
	} while (_gsShouldPop && _gs.size());
	_unlock_mutex(&graphics_mutex);
	_locked_gfx_mutex_for_gs_popping = false;
}


/* Events */

void W::_addEvent(const Event &ev) {
	_lock_mutex(&event_mutex);
	_events.push_back(ev);
	_unlock_mutex(&event_mutex);
}


/* W::start() */

void W::start() {
	if (!_window)
		throw Exception("Error: W::startGameLoop called, but no window has been created");
	
	// Create drawing thread
	#ifdef __APPLE__
		int err;
		pthread_attr_t attr;
	
		err = pthread_attr_init(&attr);
		if (err) throw Exception("Error: couldn’t initialize drawing thread attributes", err);
		
		err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		if (err) throw Exception("Error: couldn’t set drawing thread to detached", err);
		
		err = pthread_create(&_drawingThread, &attr, drawingThreadFn, NULL);
		if (err) throw Exception("Error: couldn’t detach drawing thread", err);
		
		err = pthread_attr_destroy(&attr);
		if (err) log << "Error: couldn’t destroy drawing thread attributes obj" << " (error: " << err << ")" << std::endl;
	#elif defined _WIN32 || _WIN64
		_drawingThreadHandle = CreateThread(
			0,					// ptr to SECURITY_ATTRIBUTES struct (0 for def.)
			0,					// stack size (0 for def.)
			&drawingThreadFn,	// ptr to fn to be executed
			NULL,				// ptr to variable to be passed
			0,					// flags
			&_drawingThreadId	// ptr to dword to store thread id in
		);
		if (!_drawingThreadHandle)
			throw Exception("Error: couldn’t detach drawing thread");
	#endif
	
	// Begin updating
	_updateTimerStart();
	
	#if defined _WIN32 || _WIN64
		// Win: enter message pump loop
		MSG msg;
		while (!W::_quit) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT)
					W::_quit = true;
				else {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		quit();
	#endif
}
void W::_updateTimerStart() {
	#ifdef __APPLE__
		[(UpdateTimer*)_updateTimer start];
	#elif defined _WIN32 || _WIN64
		// Create mmt
		MMRESULT mmr = timeSetEvent(20, 40, &_mmtCallback, NULL, TIME_PERIODIC);
		if (mmr == NULL)
			throw Exception("Error starting multimedia timer");
		_updateTimer = mmr;
	#endif
}
void W::_updateTimerStop() {
	#ifdef __APPLE__
		[(UpdateTimer*)_updateTimer stop];
	#elif defined _WIN32 || _WIN64
		timeKillEvent(_updateTimer);
	#endif
}
#if defined _WIN32 || _WIN64
void CALLBACK W::_mmtCallback(UINT timerid, UINT uMsg, DWORD userdata, DWORD d1, DWORD d2) {
	_update();
}
#endif


/* The update cycle */

void W::_update() {
	if (!_gs.size())
		return;
	
	// Copy events to temporary vector
	_window->generateMouseMoveEvent();	// TODO: only generate if within window bounds (plus some border)
	_lock_mutex(&event_mutex);
	std::vector<Event> evs = _events;
	_events.clear();
	_unlock_mutex(&event_mutex);
	
	GameState *g;
	_gsShouldPop = false;
	
	// Send events
	g = _gs.back();
	for (std::vector<Event>::iterator it = evs.begin(); it < evs.end(); it++) {
		Event &ev = *it;
		if (ev.type == EventType::CLOSED)
			g->handleCloseEvent();
		else if (ev.type == EventType::LEFTMOUSEDOWN || ev.type == EventType::LEFTMOUSEUP
				 || ev.type == EventType::RIGHTMOUSEDOWN || ev.type == EventType::RIGHTMOUSEUP
				 || ev.type == EventType::MOUSEMOVE)
			Messenger::dispatchPositionalEvent(&ev);
		else
			Messenger::dispatchEvent(&ev);
		if (_gsShouldPop) break;
	}
	evs.clear();
	
	// Check for poppage due to event input
	if (_gsShouldPop) {
		_performPop();
		_gsShouldPop = false;
	}
	
	// Update if no poppage
	else if (_gs.size()) {
		g = _gs.back();
		g->update();
		
		// Check for poppage due to update
		if (_gsShouldPop) {
			_performPop();
			_gsShouldPop = false;
		}
	}
	
	// Call updateDOs on all views, to apply D.O. changes made during update
	if (_gs.size()) {
		_lock_mutex(&graphics_mutex);
		GameState::Viewlist &views = _gs.back()->_vlist;
		for (GameState::Viewlist::iterator it = views.begin(); it != views.end(); it++)
			(*it)->_updateDOs();
		_unlock_mutex(&graphics_mutex);
	}
	
	else {
		#ifdef __APPLE__
			quit();
		#elif defined _WIN32 || _WIN64
			_quit = true;
		#endif
	}
}


/* Quitting */

void W::quit() {
	_updateTimerStop();

	#ifdef __APPLE__
		// Called by NSTimer on same thread
		_quit = true;
		void *_drawThrRetVal;	// Wait for drawing thread to detect _quit and return
		if (int err = pthread_join(_drawingThread, &_drawThrRetVal))
			throw Exception("Error joining drawing thread", err);
	#elif defined _WIN32 || _WIN64
		// Called after message pump loop detects _quit and breaks
		DWORD exit_code;
		bool drawThreadStillRunning = true;	// Wait for drawing thread
		while (drawThreadStillRunning) {
			bool success = GetExitCodeThread(_drawingThreadHandle, &exit_code);
			if (!success) throw Exception("Error getting drawing thread exit status");
			drawThreadStillRunning = (exit_code == STILL_ACTIVE);
			Sleep(5);
		}
		CloseHandle(_drawingThreadHandle);
		timeEndPeriod(2);
	#endif
	
	delete _window;
	if (log.is_open()) log.close();
	
	#ifdef __APPLE__
		[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0];
	#endif
}


/* The drawing thread function */

#ifdef __APPLE__
void* W::drawingThreadFn(void *)
#elif defined _WIN32 || _WIN64
DWORD WINAPI W::drawingThreadFn(LPVOID lpParam)
#endif
{
	_window->setOpenGLThreadAffinity();
	
//	timeval t1, t2;
//	int nFramesToMeasure = 60;
//	std::vector<float> lastNFrameDurations;
//	lastNFrameDurations.resize(nFramesToMeasure);
//	int f = 0;
	
	_setUpViewport();
	
	// Draw loop
	while (!_quit) {
//		gettimeofday(&t1, NULL);
		// Initial setup
		size window_size = _window->getDimensions();
		int &winW = window_size.width, &winH= window_size.height;
		
		glScissor(0, 0, winW, winH);
		
		glClearColor(0.525, 0.187, 0.886, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Draw
		_lock_mutex(&graphics_mutex);
		if (int n = (int) _gs.size()) {
			int first_to_draw = n - 1;
			for (int i = n-1; i >= 0; i--)
                if (_gs[i]->isTranslucent())
					first_to_draw = (i ? i-1 : 0);
			
			// Draw all GameStates back to the last that was translucent - 1
			for (int i = first_to_draw; i < n; i++) {
				// Call _draw on all views
				GameState::Viewlist &views = _gs[i]->_vlist;
				for (GameState::Viewlist::iterator it = views.begin(); it != views.end(); it++)
					(*it)->_draw(window_size);
			}
		}
		_unlock_mutex(&graphics_mutex);
		
		_window->swapBuffers();
		
		// Texture uploading/unloading
		if (_textures_to_upload.size()) {
			_lock_mutex(&texture_mutex);
			for (std::vector<Texture*>::iterator it = _textures_to_upload.begin(); it < _textures_to_upload.end(); it++)
				(*it)->upload();
			_textures_to_upload.clear();
			_unlock_mutex(&texture_mutex);
		}
		if (_textures_to_unload.size()) {
			_lock_mutex(&texture_mutex);
			for (std::vector<Texture*>::iterator it = _textures_to_unload.begin(); it < _textures_to_unload.end(); it++)
				delete *it;
			_textures_to_unload.clear();
			_unlock_mutex(&texture_mutex);
		}
//		gettimeofday(&t2, NULL);
//		lastNFrameDurations[f] =
//			t2.tv_sec - t1.tv_sec
//			+ t2.tv_usec/1000000.0 - t1.tv_usec/1000000.0;
//		if (++f >= nFramesToMeasure) {
//			float totalTime = 0;
//			for (std::vector<float>::iterator it = lastNFrameDurations.begin(); it < lastNFrameDurations.end(); it++)
//				totalTime += *it;
//			std::cout
//				<< "average framerate for last " << nFramesToMeasure << " frames: "
//				<< nFramesToMeasure/totalTime
//				<< std::endl;
//			f = 0;
//		}
	}
	
	_window->clearOpenGLThreadAffinity();
	
	return 0;
}
void W::_setUpViewport() {
	size winSz = _window->getDimensions();
	
	glViewport(0, 0, winSz.width, winSz.height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, winSz.width, winSz.height, 0, -1, 1);
	
	glMatrixMode(GL_MODELVIEW);
}


/* Window */

void W::createWindow(const size &_size, const char *_title) {
	_window = new Window(_size, _title);
}
void W::_updateAllViewPositions() {
	const size &s = _window->getDimensions();
	for (std::vector<GameState*>::iterator itgs = _gs.begin(); itgs < _gs.end(); itgs++) {
		GameState::Viewlist &vlist = (*itgs)->_vlist;
		for (GameState::Viewlist::iterator itv = vlist.begin(); itv != vlist.end(); itv++)
			(*itv)->_updatePosition(s);
	}
}


/* Bollocks */

bool W::isValidDir(const std::string &path) {
	return isValidDir(path.c_str());
}
bool W::isValidDir(const char *path) {
	#ifdef __APPLE__
		BOOL isdir;
		[[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithCString:path encoding:NSUTF8StringEncoding]
											 isDirectory:&isdir];
		return isdir;
	#elif defined _WIN32 || _WIN64
		DWORD dw = GetFileAttributes(path);
		return (dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_DIRECTORY));
	#endif
}
bool W::createDir(const std::string &path) {
	return createDir(path.c_str());
}
bool W::createDir(const char *dir) {
	#ifdef __APPLE__
		return [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithCString:dir encoding:NSUTF8StringEncoding]
										 withIntermediateDirectories:YES
														  attributes:nil
															   error:nil];
	#elif defined _WIN32 || _WIN64
		return CreateDirectory(dir, NULL);
	#endif
}
