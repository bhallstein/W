/*
 * W - a tiny 2D game development library
 *
 * =======
 *  W.h
 * =======
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// Implementations of user-facing global W functions
// - these call their equivalents in W_internal

#include "W.h"
#include "W_internal.h"


namespace W {
	int updateMicroseconds = 0;
}

/*** createWindow() ***/

#ifdef WTARGET_IOS
	void W::createWindow() {
		_createWindow(v2i(), "");
	}
#else
	void W::createWindow(const v2i &sz, const std::string &title) {
		_createWindow(sz, title);
	}
#endif


/*** start() ***/

void W::start() {
	_start();
}


/*** pushState() & popState() ***/

void W::pushState(W::GameState *s) {
	_pushState(s);
}
void W::popState(const W::Returny &r) {
	_popState(r);
}
