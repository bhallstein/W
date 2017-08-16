/*
 * W - a tiny 2D game development library
 *
 * ==================
 *  Controller.cpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Controller.h"
#include "Window.h"
#include "UpdateTimer.h"
#include "GameState.h"
#include "View.h"
#include "MegaTexture.h"
#include "Timer.h"

#include "oglInclude.h"

#ifdef WTARGET_MAC
	#import <Cocoa/Cocoa.h>
#elif defined WTARGET_WIN
	bool _quit = false;
#endif

/* Callback fn to wrap Controller::update() */

W::Controller *_mrController = NULL;
void _updateCallback() {
	if (_mrController) _mrController->update();
}


/***********************/
/*** Controller impl ***/
/***********************/

W::Controller::Controller() :
	window(NULL),
	updateTimer(new UpdateTimer(_updateCallback)),
	timer(new Timer),
	firstUpdate(true)
{
	_mrController = this;
}
W::Controller::~Controller()
{
	updateTimer->stop();
	delete updateTimer;
	_mrController = NULL;
	delete timer;
	if (window) delete window;
}

void W::Controller::createWindow(const W::size &sz, const std::string &title) {
	if (window) {
		log << "Warning: createWindow() called, but window already exists\n";
		return;
	}
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
	
	if (window->winSizeHasChanged) {
		window->setUpViewport();
		updateAllViewPositions();
		window->winSizeHasChanged = false;
	}
	
	if (MegaTexture::was_reallocated) {
		GameState::_updateAllDObjTexcoords();
		MegaTexture::was_reallocated = false;
	}
	
	if (!GameState::_gsStack.size()) {
		quit();
		return;
	}
	
	updateMicroseconds = (int) timer->getMicroseconds();
	timer->reset();
	
	
	/* 1. Event sending */
	
	#ifndef WTARGET_IOS
		window->generateMouseMoveEvent();
	#endif
	
	#ifdef WTARGET_WIN
		Event::_mutex.lock();
	#endif
	
	GameState *g = GameState::_gsStack.back();
	
	for (std::vector<Event*>::iterator it = Event::_events.begin(); it < Event::_events.end(); ++it) {
		Event *ev = *it;
		if (ev->type == EventType::Closed)
			g->handleCloseEvent();
		else {
			// Correct any touch events that are misplaced due to calibration error
			const W::size &wSz = window->getSize();
			if (ev->pos.x < 0) ev->pos.x = 0;
			else if (ev->pos.x >= wSz.width) ev->pos.x = wSz.width - 1;
			if (ev->pos.y < 0) ev->pos.y = 0;
			else if (ev->pos.y >= wSz.height) ev->pos.y = wSz.height - 1;
			Messenger::dispatchEvent(ev);
		}
		delete *it;
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
	
	
	/* 3. Texture uploading */

	if (MegaTexture::modified()) MegaTexture::upload();
	
	
	/* 4. Drawing */
	
	const size &window_size = window->getSize();
	window->beginDrawing(window_size);
	
	if (int n = (int)GameState::_gsStack.size()) {
		// Draw all GameStates back to the last that was translucent - 1
		int first_to_draw = n - 1;
		for (int i = n-1; i >= 0; --i)
			if (GameState::_gsStack[i]->isTranslucent())
				first_to_draw = (i ? i-1 : 0);
		for (int i = first_to_draw; i < n; ++i) {
			// Call _draw on all views
			GameState::Viewlist &views = GameState::_gsStack[i]->_vlist;
			for (GameState::Viewlist::iterator it = views.begin(); it != views.end(); ++it) {
				(*it)->_draw(window_size);
			}
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

#ifdef WTARGET_IOS
void W::createWindow() {
	_controller.createWindow(size(), "");
}
#else
void W::createWindow(const size &sz, const std::string &title) {
	_controller.createWindow(sz, title);
}
#endif
void W::start() {
	_controller.start();
}

int W::updateMicroseconds;
