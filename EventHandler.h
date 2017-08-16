#ifndef __W__EventHandler
#define __W__EventHandler

#include <vector>
#include <map>

#include "Event.h"

namespace W {
	
	class EventResponder;
	class Callback;

	struct _subscription;
	
	class EventHandler {
	public:
		EventHandler();
		~EventHandler();
		
		void subscribeToMouseEvents(const W::Callback &);
		void unsubscribeFromMouseEvents(W::EventResponder *);
		
		bool requestPrivilegedEventResponderStatus(const W::Callback &);
		void relinquishPrivilegedEventResponderStatus(W::EventResponder *);
		bool requestPrivilegedResponderStatusForEventType(W::EventType::T, const W::Callback &);
		void relinquishPrivilegedResponderStatusForEventType(W::EventType::T, W::EventResponder *);
		
		void subscribeToKey(W::KeyCode::T, const W::Callback &);
		void unsubscribeFromKey(W::KeyCode::T, W::EventResponder *);
		void unsubscribeFromAllKeys(W::EventResponder *);
		
		void subscribeToEventType(W::EventType::T, const W::Callback &);
		void unsubscribeFromEventType(W::EventType::T, W::EventResponder *);
		
		void dispatchEvent(W::Event *);
		
	private:
		typedef std::vector<Callback *> callback_list;
		
		std::map<KeyCode::T, callback_list> keycode_subscriptions;
		std::map<EventType::T, callback_list> type_subscriptions;
		
		Callback *privileged_event_responder;
		std::map<EventType::T, Callback *> type_pers;
		
		void _dispatchMouseEvent(Event *);
	};
	
}

#endif
