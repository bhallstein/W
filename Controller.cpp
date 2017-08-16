#include "Controller.h"
#include "Window.h"
#include "UpdateTimer.h"
#include "GameState.h"
#include "View.h"
#include "MegaTexture.h"

#if defined WTARGET_MAC || WTARGET_IOS
	#include <Cocoa/Cocoa.h>
	#include <OpenGL/gl.h>
#elif defined WTARGET_WIN
	#include <gl\gl.h>
	#include <gl\glu.h>
	bool _quit = false;
#endif

W::Controller::Controller() :
	window(NULL),
	updateTimer(new UpdateTimer(new Callback(&Controller::update, this))),
	firstUpdate(true)
{
	// hello controller
}
W::Controller::~Controller()
{
	updateTimer->stop();
	delete updateTimer;
	if (window) delete window;
}

void W::Controller::createWindow(const W::size &sz, const std::string &title) {
	if (window) return;
	window = new Window(sz, title);
}

void W::Controller::start() {
	if (!window)
		throw Exception("start() called, but no window has been created");
	if (!GameState::_gsStack.size())
		throw Exception("start() called, but no GameState has been pushed");
	
	updateTimer->start();
	
	// Win: enter message pump
	#if defined WTARGET_WIN
		MSG msg;
		while (!_quit) {
			if (GetMessage(&msg, NULL, 0, 0)) {
				if (msg.message == WM_QUIT) {
					//W::_quit = true;
				}
				else {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		window->setOpenGLThreadAffinity();
	#endif
}
void W::Controller::quit() {
	updateTimer->stop();
	
	#ifdef WTARGET_MAC
		[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0];
	#elif defined WTARGET_WIN
		window->clearOpenGLThreadAffinity();
		_quit = true;
	#endif
}

void W::Controller::update() {
	if (firstUpdate) {
		window->setOpenGLThreadAffinity();
		window->setUpViewport();
		firstUpdate = false;
	}
	
	if (!GameState::_gsStack.size()) {
		quit();
		return;
	}
	
	/* 1. Event sending */
	
	window->generateMouseMoveEvent();
	
	#ifdef WTARGET_WIN
		Event::_mutex.lock();
	#endif
	
	GameState *g = GameState::_gsStack.back();
	
	for (std::vector<Event*>::iterator it = Event::_events.begin(); it < Event::_events.end(); ++it) {
		Event &ev = **it;
		if (ev.type == EventType::CLOSED)
			g->handleCloseEvent();
		else if (ev.type == EventType::LEFTMOUSEDOWN || ev.type == EventType::LEFTMOUSEUP
				 || ev.type == EventType::RIGHTMOUSEDOWN || ev.type == EventType::RIGHTMOUSEUP
				 || ev.type == EventType::MOUSEMOVE)
			Messenger::dispatchPositionalEvent(&ev);
		else
			Messenger::dispatchEvent(&ev);
		delete *it;
//		if (GameState::_pop) break;
	}
	Event::_events.clear();

	#ifdef WTARGET_WIN
		Event::_mutex.unlock();
	#endif
	
	// Check for poppage due to event input
	if (GameState::_pop) {
		GameState::_popState();
		GameState::_pop = false;
	}
	
	
	/* 2. Update (if no poppage) */
	
	// Update if no poppage
	else if (GameState::_gsStack.size()) {
		g = GameState::_gsStack.back();
		g->update();
		
		// Check for poppage due to update
		if (GameState::_pop) {
			GameState::_popState();
			GameState::_pop = false;
		}
	}
	
	
	/* 3. Apply changes to drawn objects */

	if (GameState::_gsStack.size()) {
		GameState::Viewlist &views = GameState::_gsStack.back()->_vlist;
		for (GameState::Viewlist::iterator it = views.begin(); it != views.end(); ++it)
			(*it)->_updateDObjs();
	}
	
	
	/* 4. Texture uploading */

	if (MegaTexture::modified()) MegaTexture::upload();
	
	
	/* 5. Drawing */

	size window_size = window->getSize();
	int &winW = window_size.width, &winH= window_size.height;
	
	glScissor(0, 0, winW, winH);
	glClearColor(0.525, 0.187, 0.886, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, MegaTexture::getGLTexId());
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_MAG_FILTER, GL_LINEAR);
	
	if (int n = (int)GameState::_gsStack.size()) {
		// Draw all GameStates back to the last that was translucent - 1
		int first_to_draw = n - 1;
		for (int i = n-1; i >= 0; --i)
			if (GameState::_gsStack[i]->isTranslucent())
				first_to_draw = (i ? i-1 : 0);
		for (int i = first_to_draw; i < n; ++i) {
			// Call _draw on all views
			GameState::Viewlist &views = GameState::_gsStack[i]->_vlist;
			for (GameState::Viewlist::iterator it = views.begin(); it != views.end(); ++it)
				(*it)->_draw(window_size);
		}
	}
	window->flushBuffer();
}

void W::Controller::updateAllViewPositions() {
	const size &s = window->getSize();
	for (std::vector<GameState*>::iterator itgs = GameState::_gsStack.begin(); itgs < GameState::_gsStack.end(); ++itgs) {
		GameState::Viewlist &vlist = (*itgs)->_vlist;
		for (GameState::Viewlist::iterator itv = vlist.begin(); itv != vlist.end(); itv++)
			(*itv)->_updatePosition(s);
	}
}


/*************************************************/
/*** Main W-running interface global functions ***/
/*************************************************/

W::Controller W::_controller;

void W::createWindow(const size &sz, const std::string &title) {
	_controller.createWindow(sz, title);
}
void W::start() {
	_controller.start();
}
