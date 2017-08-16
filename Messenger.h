#ifndef __W__Messenger
#define __W__Messenger

#include <vector>
#include <map>

#include "Event.h"

namespace W {
	
	class EventResponder;
	class Callback;
	class GameState;
	class MappedObj;
	
	class Messenger {
	public:
		static void subscribeToEventType(W::EventType::T, const W::Callback &);
		static void unsubscribeFromEventType(W::EventType::T, void *);
		
		static void subscribeToPositionalEventType(W::EventType::T, const W::Callback &, W::MappedObj *);
		static void unsubscribeFromPositionalEventType(W::EventType::T, void *);
		
		static void subscribeToMouseEvents(const W::Callback &, W::MappedObj *);
		static void unsubscribeFromMouseEvents(void *);
		
		static bool requestPrivilegedEventResponderStatus(const W::Callback &);
		static void relinquishPrivilegedEventResponderStatus(void *);
		static bool requestPrivilegedResponderStatusForEventType(W::EventType::T, const W::Callback &);
		static void relinquishPrivilegedResponderStatusForEventType(W::EventType::T, void *);
		
		static void dispatchEvent(W::Event *);
		static bool dispatchPositionalEvent(W::Event *);
		static void _dispatchUIEvent(W::Event *);
		static bool _dispatchToPERs(W::Event *);
			// Returns true if event was sent to a P.E.R., false otherwise
		
		static void subscribeToUIEvent(const char *_element_name, W::EventType::T, const W::Callback &);
		static void unsubscribeFromUIEvent(const char *_element_name, W::EventType::T, void *);
		
		static void _useTemporaryState();	// Save subscriptions in a temporary state, for transferal in setActive~
		static void _setActiveGamestate(GameState *);
		static void _gamestateDestroyed(GameState *);
		
	private:
		struct _messenger_state;
		static std::map<GameState*, _messenger_state*> _stateMap;
		static _messenger_state *_s;	// The active state
		
	};
	
}

#endif
