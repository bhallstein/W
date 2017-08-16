/*
 * W - a tiny 2D game development library
 *
 * ==================
 *  W_internal.cpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "W_internal.h"
#include "oglInclude.h"
#include "TextureAtlas.h"
#include "UpdateTimer.h"
#include "types.h"

#ifdef WTARGET_MAC
	#import <Cocoa/Cocoa.h>
#elif defined WTARGET_WIN
	bool _w_quit = false;
#endif

//#define __W_DEBUG
#include "DebugMacro.h"

namespace W {
	WObjs wObjs;
	OpenGLState oglState;
}

bool _firstUpdate = true;
bool _popGS = false;

void _update();
void _updateAllViewPositions();
void _quit();

W::OpenGLState::OpenGLState() :
	curBoundTexAtlas(NULL),
	curBlendMode(BlendMode::Normal),
	texturingEnabled(false)
{
	// Hai OpenGLState
}
void W::OpenGLState::setUpInitially() {
	w_dout << " OpenGLState::setUpInitially()\n";
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);
	
	glEnable(GL_SCISSOR_TEST);

	glEnable(GL_BLEND);
	setBlendMode(BlendMode::Normal);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
}
void W::OpenGLState::setBlendMode(BlendMode::T m) {
	w_dout << " OpenGLState::setBlendMode() (mode " << m << ")\n";
	if (m == BlendMode::Normal)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else if (m == BlendMode::Additive)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}
void W::OpenGLState::bindAtlas(TextureAtlas *a) {
	w_dout << " OpenGLState::bindAtlas() (atlas " << a << ")\n";
	if (a != curBoundTexAtlas) {
		glBindTexture(GL_TEXTURE_2D, a->getGLTexId());
		curBoundTexAtlas = a;
	}
}
void W::OpenGLState::enableTexturing() {
	w_dout << " OpenGLState::enableTexturing()";
	if (!texturingEnabled) {
		w_dout << "  toggling...\n";
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		texturingEnabled = true;
	}
	else
		w_dout << "  skipping - already enabled\n";
}
void W::OpenGLState::disableTexturing() {
	w_dout << " OpenGLState::disableTexturing()";
	if (texturingEnabled) {
		w_dout << "  toggling...\n";
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
		curBoundTexAtlas = NULL;

		texturingEnabled = false;
	}
	else
		w_dout << "  skipping - already disabled\n";
}


W::WObjs::WObjs() :
	window(NULL),
	updateTimer(NULL),
	gameTimer(NULL),
	returny(ReturnyType::Empty)
{
	// Hai WObjs
}
W::WObjs::~WObjs()
{
	updateTimer->stop();
	delete updateTimer;
	delete gameTimer;
	if (window) delete window;
}


void W::_createWindow(const W::size &sz, const std::string &title) {
	// Initialize W objects
	if (wObjs.window) {
		std::string s("Warning: createWindow() called, but window already exists\n");
		w_dout << s;
		log << s;
		return;
	}
	wObjs.window = new Window(sz, title);
	wObjs.updateTimer = new UpdateTimer(_update);
	wObjs.gameTimer = new Timer();
}


void W::_start() {
	if (!wObjs.window)
		throw Exception("start() called, but no window has been created");
	if (wObjs.gsStack.empty())
		throw Exception("start() called, but no GameState has been pushed");
	
	wObjs.updateTimer->start();
	
	// Win: enter message pump
	#if defined WTARGET_WIN
		MSG msg;
		while (!_w_quit) {
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
		wObjs.window->setOpenGLThreadAffinity();
	#endif
}


void W::_pushState(GameState *g) {
//	if (!states.empty()) states.back()->pause();
	wObjs.gsStack.push_back(g);
	Messenger::_setActiveGamestate(g);
}
void W::_popState(const Returny &r) {
	_popGS = true;
	wObjs.returny = r;
}

const W::size& W::windowSize() {
	return wObjs.window->getSize();
}

void _doPopState() {
	W::WObjs &objs = W::wObjs;
	do {
		_popGS = false;
		W::GameState *last_gs = objs.gsStack.back();
		delete last_gs;
		W::Messenger::_gamestateDestroyed(last_gs);
		objs.gsStack.pop_back();
		if (objs.gsStack.size()) {
			W::GameState *new_gs = objs.gsStack.back();
			W::Messenger::_setActiveGamestate(new_gs);
			new_gs->resume(&objs.returny);
		}
	} while (_popGS && objs.gsStack.size());
}

void _update() {
	W::WObjs &objs = W::wObjs;
	
	if (_firstUpdate) {
		objs.window->setOpenGLThreadAffinity();
		objs.window->setUpViewport();
		_firstUpdate = false;
	}
	
	if (objs.window->winSizeHasChanged) {
		objs.window->setUpViewport();
		_updateAllViewPositions();
		objs.window->winSizeHasChanged = false;
	}
	
	if (objs.gsStack.empty()) {
		_quit();
		return;
	}
	
	W::updateMicroseconds = (int) objs.gameTimer->getMicroseconds();
	objs.gameTimer->reset();
	
	
	/* 1. Event sending */
	
	#ifndef WTARGET_IOS
		objs.window->generateMouseMoveEvent();
	#endif
		
	#ifdef WTARGET_WIN
		W::Event::_mutex.lock();
	#endif
	
	W::GameState *g = objs.gsStack.back();
	
	std::vector<W::Event*> &evs = W::Event::_events;
	for (std::vector<W::Event*>::iterator it = evs.begin(); it < evs.end(); ++it) {
		W::Event *ev = *it;
		if (ev->type == W::EventType::Closed)
			g->handleCloseEvent();
		else {
			// Correct any touch events that are misplaced due to calibration error
			const W::size &wSz = objs.window->getSize();
			if (ev->pos.x < 0) ev->pos.x = 0;
			else if (ev->pos.x >= wSz.width) ev->pos.x = wSz.width - 1;
			if (ev->pos.y < 0) ev->pos.y = 0;
			else if (ev->pos.y >= wSz.height) ev->pos.y = wSz.height - 1;
			// Dispatch event
			W::Messenger::dispatchEvent(ev);
		}
		delete *it;
	}
	evs.clear();
	
	#ifdef WTARGET_WIN
		W::Event::_mutex.unlock();
	#endif
	
	// Check for poppage due to event input
	if (_popGS) {
		_doPopState();
		_popGS = false;
	}
	
	
	/* 2. Update (if no poppage) */
	
	// Update if no poppage
	else if (objs.gsStack.size()) {
		g = objs.gsStack.back();
		g->update();
		
		// Check for poppage due to update
		if (_popGS) {
			_doPopState();
			_popGS = false;
		}
	}
	
	
	/* 3. TextureAtlas uploading */
	
	for (std::set<W::TextureAtlas*>::iterator it = W::TextureAtlas::modifiedAtlases.begin(); it != W::TextureAtlas::modifiedAtlases.end(); ++it)
		(*it)->upload();
	W::TextureAtlas::modifiedAtlases.clear();
	
	
	/* 4. Drawing */
	
	const W::size &window_size = objs.window->getSize();
	objs.window->beginDrawing();
	
	if (int n = (int)objs.gsStack.size()) {
		// Draw all GameStates back to the last that was translucent - 1
		int first_to_draw = n - 1;
		for (int i = n-1; i >= 0; --i)
			if (objs.gsStack[i]->isTranslucent())
				first_to_draw = (i ? i-1 : 0);
		for (int i = first_to_draw; i < n; ++i) {
			// Call _draw on all views
			W::GameState::Viewlist &views = objs.gsStack[i]->_vlist;
			for (W::GameState::Viewlist::iterator it = views.begin(); it != views.end(); ++it) {
				(*it)->_draw(window_size);
			}
		}
	}
	objs.window->flushBuffer();
}

void _updateAllViewPositions() {
	const W::size &s = W::wObjs.window->getSize();
	for (std::vector<W::GameState*>::iterator itgs = W::wObjs.gsStack.begin(); itgs < W::wObjs.gsStack.end(); ++itgs) {
		W::GameState::Viewlist &vlist = (*itgs)->_vlist;
		for (W::GameState::Viewlist::iterator itv = vlist.begin(); itv != vlist.end(); itv++)
			(*itv)->_updatePosition(s);
	}
}

void _quit() {
	W::wObjs.updateTimer->stop();
	
	#ifdef WTARGET_MAC
		[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0];
	#elif defined WTARGET_WIN
		W::wObjs.window->clearOpenGLThreadAffinity();
		_w_quit = true;
	#endif
}



