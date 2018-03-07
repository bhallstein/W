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
		static bool dispatchEvent(Event);
		
		// Subscription
		static void subscribe(EventType::T, const Callback &);
		static void unsubscribe(EventType::T, void*);
		
		static void subscribeInView(View*, EventType::T, const Callback &, fRect*);
		static void unsubscribeInView(View*, EventType::T, void*);
		
		static void subscribeToMouseEvents(View*, const Callback &, fRect*);
		static void unsubscribeFromMouseEvents(View*, void*);
		
		static bool requestPrivilegedEventResponderStatus(View*, EventType::T, const Callback &, bool global = false);
		static void relinquishPrivilegedEventResponderStatus(View*, EventType::T, void*, bool global = false);
			// A P.E.R. receives positional events preferentially:
			// - If there is a global P.E.R. for a particular event type, it receives those
			//   events, whether or not the event lies over the same view
			// - If there is a non-global P.E.R. for a particular event type for a particular view,
			//   it receives those events, and other subscribers do not.
		
		static bool subscribeToTouchEvent(int _touchID, const Callback &);
		static void unsubscribeFromTouchEvent(int _touchID, void *);
		
		static void subscribeToUIEvent(const char *_element_name, EventType::T, const Callback &);
		static void unsubscribeFromUIEvent(const char *_element_name, EventType::T, void *);
		
		// GameState/State management
		static void _useTemporaryState();	// Save subscriptions in a temporary state, for transferral in setActive~
		static void _restorePreviousIfUsingTemporaryState();
		static void _setActiveGamestate(GameState *);
		static void _gamestateDestroyed(GameState *);
		
	private:
		Messenger() { }
		
		struct MState;
		static std::map<GameState*, MState*> stateMap;
		static MState *s;	// The active state
		
		static GameState *activeGS;
		static GameState *prev_activeGS;
		
		// Private dispatch methods
		static bool dispMouse(Event, View*);
		static bool dispTouch(Event, View*);
		static bool dispUI(Event);
		
		static bool dispPositionalInView(Event, View*);
		static View* lastViewBeneathEvent(Event);
		
		// Private PER subscription methods
		static bool reqPERGlobally(View*, EventType::T, const Callback &);
		static bool reqPERNonglobally(View*, EventType::T, const Callback &);
		static void relinqPERGlobally(View*, EventType::T, void*);
		static void relinqPERNonglobally(View*, EventType::T, void*);
		
		// Event typing methods
		static bool _isMouse(Event);
		static bool _isTouch(Event);
		static bool _isPositional(Event);
		static bool _isUI(Event);
	};
	
}

#endif
