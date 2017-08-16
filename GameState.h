/*
 * W - a tiny 2D game development library
 *
 * ===============
 *  GameState.h
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__GameState
#define __W__GameState

#include "Messenger.h"
#include "Returny.h"
#include <vector>
#include <list>

namespace W {
	
	class Event;
	class View;
	
	enum GameStateTranslucency { GS_OPAQUE = 0, GS_TRANSLUCENT };
	
	class GameState {
	public:
		typedef std::list<View*> Viewlist;
		
		GameState(GameStateTranslucency _tr = GS_OPAQUE);
		virtual ~GameState();
		
		virtual void resume(Returny *) = 0;
		virtual void update() = 0;
		
		void addView(View *);
		void removeView(View *);
		Viewlist _vlist;
		
		virtual void handleCloseEvent();	// By default, initiates the killer returny tango: override to customise
		
		bool isTranslucent() { return translucent; }
		
	private:
		bool translucent;	// If true, the previous state will be drawn too.
		
	};

}

#endif
