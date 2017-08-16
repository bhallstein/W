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

// User-facing include file

#ifndef __W__W
#define __W__W

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "types.h"
#include "Log.h"
#include "Rand.h"
#include "Colour.h"
#include "DrawingClasses.h"
#include "Event.h"
#include "Window.h"
#include "View.h"
#include "UIView.h"
#include "Positioner.h"
#include "Messenger.h"
#include "GameState.h"
#include "Callback.h"
#include "NavMap.h"
#include "Texture.h"
#include "Timer.h"


// External global interface functions

namespace W {
	
	#ifdef WTARGET_IOS
		void createWindow();
	#else
		void createWindow(const W::size &sz, const std::string &title);
	#endif
	void start();
	void pushState(W::GameState *);
	void popState(const W::Returny &);
	
	extern int updateMicroseconds;
	
}

#endif
