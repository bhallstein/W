/*
 * W - a tiny 2D game development library
 *
 * ================
 *  W_internal.h
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// Define global internal state
// - The user-facing W.h class basically provides the public
//   interface for functionality implemented here

#ifndef __W__W_internal
#define __W__W_internal

#include "W.h"
#include "UpdateTimer.h"

namespace W {
	
	struct OpenGLState {
		OpenGLState();
		void setUpInitially();
		void enableTexturing();
		void disableTexturing();
		
		void setBlendMode(BlendMode::T);
		void bindAtlas(TextureAtlas *);

		TextureAtlas *curBoundTexAtlas;
		BlendMode::T curBlendMode;
		bool texturingEnabled;
	};
	extern OpenGLState oglState;
	
	void _createWindow(const v2i &sz, const std::string &title);
	void _start();
	void _pushState(GameState *);
	void _popState(const Returny &);
	
	struct WObjs {
		WObjs();
		~WObjs();
		Window *window;
		UpdateTimer *updateTimer;
		Timer *gameTimer;
		
		std::vector<GameState*> gsStack;
		Returny returny;
	};
	extern WObjs wObjs;
	
	const v2i& windowSize();
}

#endif
