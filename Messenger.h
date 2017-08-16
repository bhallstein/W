#ifndef __W__Messenger
#define __W__Messenger

#include <vector>
#include <map>

#include "Event.h"

namespace W {
	
	class EventResponder;
	class Callback;
	class GameState;
	
	class Messenger {
	public:
		static void subscribeToEventType(W::EventType::T, const W::Callback &);
		static void unsubscribeFromEventType(W::EventType::T, W::EventResponder *);
		
		static void subscribeToMouseEvents(const W::Callback &);
		static void unsubscribeFromMouseEvents(W::EventResponder *);
		
		static bool requestPrivilegedEventResponderStatus(const W::Callback &);
		static void relinquishPrivilegedEventResponderStatus(W::EventResponder *);
		static bool requestPrivilegedResponderStatusForEventType(W::EventType::T, const W::Callback &);
		static void relinquishPrivilegedResponderStatusForEventType(W::EventType::T, W::EventResponder *);
		
		static void dispatchEvent(W::Event *);
		static void _dispatchUIEvent(W::Event *);
		
		static void subscribeToUIEvent(const char *_element_name, W::EventType::T, const W::Callback &);
		static void unsubscribeFromUIEvent(const char *_element_name, W::EventType::T, W::EventResponder *);
		
		static void _useTemporaryState();	// Save subscriptions in a temporary state, for transferal in setActive~
		static void _setActiveGamestate(GameState *);
		static void _gamestateDestroyed(GameState *);
		
	private:
		static void _dispatchMouseEvent(Event *);
		
		typedef std::vector<Callback *> _callback_list;
		struct _messenger_state;
		static std::map<GameState*, _messenger_state*> _stateMap;
		static _messenger_state *_s;	// The active state
		
	};
	
}

#endif
