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
#include <vector>
#include <list>

namespace W {
	
	namespace ReturnyType {
		enum T {
			Empty,
			Killer,
			Payload		// has a string payload
		};
	}
	
	class Returny {
	public:
		Returny(ReturnyType::T _type) : type(_type) { }
		bool operator==(Returny &r) {
			return type == r.type;
		}
		ReturnyType::T type;
		std::string payload;
	};
	
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
		
		// GameState pushing/popping
		static void pushState(W::GameState *);
		static void popState(W::Returny &);
		static std::vector<GameState*> _gsStack;
		static bool _pop;
		static Returny _returny;
		static void _popState();
		
		// DObj texcoord updating
		static void _updateAllDObjTexcoords();
		void _updateAllDObjTexcoordsInThisState();
		
	private:
		bool translucent;	// If true, the previous state will be drawn too.
		
	};
	
	extern Returny KillerReturny;
	extern Returny EmptyReturny;
	
	void pushState(W::GameState *);
	void popState(W::Returny &);

}

#endif
