#include "W.h"
#include <iostream>
#include <ctime>
#include <Cocoa/Cocoa.h>
#include "ObjC-Classes.h"

#define MAX_PATH 250

std::string   W::logFilePath, W::logFileName;
std::ofstream W::log;
MTRand_int32  W::twister;
std::vector<W::GameState*> W::_gs;
std::vector<W::Event> W::_evqueue;
bool  W::_gsShouldPop;
W::Returny W::_returny(W::ReturnyType::EMPTY_RETURNY);
bool  W::_quit = false;
void* W::_demon;
std::vector<W::Window*> W::_windows;
int W::INFINITATION = 99999999;

struct W::_init {
	_init() {
		// Set default log path
		char path[MAX_PATH] = "";
		[NSHomeDirectory() getCString:path maxLength:MAX_PATH encoding:NSUTF8StringEncoding];
		std::string p = path;
		p += "/Desktop/W_app_log.txt";
		setLogPath(p.c_str());
		
		// Init MTRand
		time_t timey;
		time(&timey);
		twister.seed(timey);
		
		// Init the Demon (runs the gameloop timer)
		_demon = (void*) [[W_Demon alloc] init];
	}
};
struct W::_init *W::_initializer = new W::_init();


void W::setLogPath(const char *p) {
	logFilePath = p;
	log.open((logFilePath + logFileName).c_str());
}

unsigned int W::randUpTo(int x) {
	if (x == 0) return 0;
	return twister()%x;
}

void W::pushState(GameState *g) {
	for (std::vector<GameState*>::iterator it = _gs.begin(); it < _gs.end(); it++)
		if (*it == g) return;
//	if (!states.empty()) states.back()->pause();
	_gs.push_back(g);
}
void W::popState(W::Returny &r) {
	_gsShouldPop = true;
	_returny = r;
}
void W::_performPop() {
	do {
		_gsShouldPop = false;
		delete _gs.back();
		_gs.pop_back();
		if (_gs.size()) _gs.back()->resume(&_returny);
	} while (_gsShouldPop && _gs.size());
}

void W::_addEvent(const Event &ev) {
	_evqueue.push_back(ev);
}

void W::startGameLoop() {
	[( W_Demon*)_demon gametimerStart];
}
void W::_runGameLoop() {
	_windows[0]->_generateMouseMoveEvent();	// TODO: only generate if within window bounds (plus some border)
	GameState *g;
	_gsShouldPop = false;
	
	// Send events
	if (_gs.size()) {
		g = _gs.back();
		for (std::vector<Event>::iterator it = _evqueue.begin(); it < _evqueue.end(); it++) {
			g->handleEvent(&*it);
			if (_gsShouldPop) break;
		}
		_evqueue.clear();
	}
	
	// Check for poppage due to event input
	if (_gsShouldPop) {
		_performPop();
		_gsShouldPop = false;
	}
	
	// Update if no poppage
	else if (_gs.size()) {
		g = _gs.back();
		g->update();
	}
	
	// Check for poppage due to update
	if (_gsShouldPop) {
		_performPop();
		_gsShouldPop = false;
	}
	
	// Draw
	if (_gs.size()) {
		g = _gs.back();
		std::map<Window*, std::list<View*> > *_wv = g->_getViews();
		for (std::map<Window*, std::list<View*> >::iterator it = _wv->begin(); it != _wv->end(); it++) {
			it->first->_startDrawing();
			std::list<View*> *l = &it->second;
			for (std::list<View*>::iterator itl = l->begin(); itl != l->end(); itl++)
				(*itl)->_draw();
			it->first->_finishDrawing();
		}
	}
	else {
		_quit = true;
		[( W_Demon*)_demon gametimerStop];
		[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0];
	}
}

void W::_updateAllViewPositions() {
	for (std::vector<GameState*>::iterator itgs = _gs.begin(); itgs < _gs.end(); itgs++) {
		std::map<Window*, std::list<View*> > *wv = (*itgs)->_getViews();
		for (std::map<Window*, std::list<View*> >::iterator itwv = wv->begin(); itwv != wv->end(); itwv++) {
			size s = itwv->first->_getDimensions();
			std::list<View*> *l = &itwv->second;
			for (std::list<View*>::iterator itl = l->begin(); itl != l->end(); itl++)
				(*itl)->_updatePosition(&s);
		}
	}
}
