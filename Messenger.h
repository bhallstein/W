/*
 * W - a tiny 2D game development library
 *
 * ===============
 *  Messenger.h
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__Messenger
#define __W__Messenger

#include <vector>
#include <map>

#include "Event.h"

namespace W {
	
	class EventResponder;
	class Callback;
	class GameState;
	class View;
	
	class Messenger {
	public:
		// Dispatch
		static bool dispatchEvent(W::Event *);
		
		// Subscription
		static void subscribe(W::EventType::T, const W::Callback &);
		static void unsubscribe(W::EventType::T, void *);
		
		static void subscribeInView(View *, W::EventType::T, const W::Callback &, W::rect *);
		static void unsubscribeInView(View *, W::EventType::T, void *);
		
		static void subscribeToMouseEvents(View *, const W::Callback &, W::rect *);
		static void unsubscribeFromMouseEvents(View *, void *);
		
		static bool requestPrivilegedEventResponderStatus(View *, W::EventType::T, const W::Callback &);
		static void relinquishPrivilegedEventResponderStatus(View *, W::EventType::T, void *);
			// A P.E.R. receives positional events preferentially:
			// - If there is a P.E.R. for a particular event type for a particular view,
			//   it receives those events, and other subscribers do not.
		
		static bool subscribeToTouchEvent(int _touchID, const W::Callback &);
		static void unsubscribeFromTouchEvent(int _touchID, void *);
		
		static void subscribeToUIEvent(const char *_element_name, W::EventType::T, const W::Callback &);
		static void unsubscribeFromUIEvent(const char *_element_name, W::EventType::T, void *);
		
		// GameState/State management
		static void _useTemporaryState();	// Save subscriptions in a temporary state, for transferal in setActive~
		static void _setActiveGamestate(GameState *);
		static void _gamestateDestroyed(GameState *);
		
	private:
		Messenger() { }
		
		struct MState;
		static std::map<GameState*, MState*> stateMap;
		static MState *s;	// The active state
		
		static GameState *activeGS;
		
		// Private dispatch methods
		static bool dispatchPositionally(W::Event *, View *);
		static bool dispatchTouchically(W::Event *);
		static bool dispatchUIEvent(W::Event *);
		static bool dispatchToPERs(W::Event *, View *);

		
	};
	
}

#endif
