#include "Messenger.h"
#include "Callback.h"
#include "EventResponder.h"
#include "MappedObj.h"

namespace W {

	struct Messenger::_messenger_state {
		std::map<EventType::T, _callback_list> type_subscriptions;
		Callback *privileged_event_responder;
		std::map<EventType::T, Callback *> type_pers;
		
		_messenger_state() : privileged_event_responder(NULL) { }
		~_messenger_state() {
			if (privileged_event_responder) delete privileged_event_responder;
			for (std::map<EventType::T, Callback*>::iterator it = type_pers.begin(); it != type_pers.end(); it++)
				delete it->second;
			// Is more cleanup required here?
			// Hopefully what has been added will be unsubscribed before the EH is
			// destroyed. However...
		}
	};

}


std::map<W::GameState*, W::Messenger::_messenger_state*> W::Messenger::_stateMap;
W::Messenger::_messenger_state *W::Messenger::_s = NULL;

void W::Messenger::_useTemporaryState() {
	_gamestateDestroyed(NULL);
	_s = new _messenger_state;
}
void W::Messenger::_setActiveGamestate(W::GameState *_gs) {
	// If temporary state, save it as that for this GS
	if (_s)
		_stateMap[_gs] = _s;
	// Otherwise, create a new state
	else
		_s = _stateMap[_gs] = new _messenger_state;
}
void W::Messenger::_gamestateDestroyed(W::GameState *_gs) {
	delete _stateMap[_gs];
	_stateMap.erase(_gs);
	_s = NULL;
}

void W::Messenger::subscribeToEventType(EventType::T t, const Callback &c) {
	if (!_s) return;
	unsubscribeFromEventType(t, c.resp);
	_s->type_subscriptions[t].push_back(c.copy());
}
void W::Messenger::unsubscribeFromEventType(EventType::T t, EventResponder *r) {
	if (!_s) return;
	_callback_list *l = &_s->type_subscriptions[t];
	for (_callback_list::iterator it = l->begin(); it < l->end(); it++)
		if ((*it)->resp == r) {
			//			std::cout << "unsub from event type " << t  << ": deleting subscription..." << std::endl;
			delete *it;
			l->erase(it);
			return;
		}
}

void W::Messenger::subscribeToMouseEvents(const Callback &c) {
	if (!_s) return;
	subscribeToEventType(EventType::MOUSEMOVE, c);
	subscribeToEventType(EventType::LEFTMOUSEDOWN, c);
	subscribeToEventType(EventType::LEFTMOUSEUP, c);
	subscribeToEventType(EventType::RIGHTMOUSEDOWN, c);
	subscribeToEventType(EventType::RIGHTMOUSEUP, c);
}
void W::Messenger::unsubscribeFromMouseEvents(EventResponder *r) {
	if (!_s) return;
	unsubscribeFromEventType(EventType::MOUSEMOVE, r);
	unsubscribeFromEventType(EventType::LEFTMOUSEDOWN, r);
	unsubscribeFromEventType(EventType::LEFTMOUSEUP, r);
	unsubscribeFromEventType(EventType::RIGHTMOUSEDOWN, r);
	unsubscribeFromEventType(EventType::RIGHTMOUSEUP, r);
}

bool W::Messenger::requestPrivilegedEventResponderStatus(const Callback &c) {
	if (!_s) return false;
	if (_s->privileged_event_responder != NULL) return false;
	_s->privileged_event_responder = c.copy();
	return true;
}
void W::Messenger::relinquishPrivilegedEventResponderStatus(EventResponder *r) {
	if (!_s) return;
	if (_s->privileged_event_responder->resp == r) {
		delete _s->privileged_event_responder;
		_s->privileged_event_responder = NULL;
	}
}
bool W::Messenger::requestPrivilegedResponderStatusForEventType(EventType::T t, const Callback &c) {
	if (!_s) return false;
	if (_s->type_pers.find(t) != _s->type_pers.end())
		return false;
	_s->type_pers[t] = c.copy();
	return true;
}
void W::Messenger::relinquishPrivilegedResponderStatusForEventType(EventType::T t, EventResponder *r) {
	if (!_s) return;
	std::map<EventType::T, Callback*>::iterator it = _s->type_pers.find(t);
	if (it == _s->type_pers.end())
		return;
	if (it->second->resp == r) {
		delete it->second;
		_s->type_pers.erase(it);
	}
}

#include <iostream>

void W::Messenger::dispatchEvent(Event *ev) {
	if (!_s) return;
	
	std::map<EventType::T, _callback_list>::iterator it1;
	std::map<EventType::T, Callback *>::iterator it2;
	
	// If a general P.E.R. exists, send event to it
	if (_s->privileged_event_responder != NULL)
		_s->privileged_event_responder->call(ev);
	
	// If a P.E.R. exists specifically for this event type, send event to it
	else if ((it2 = _s->type_pers.find(ev->type)) != _s->type_pers.end())
		it2->second->call(ev);
	
	// Mouse dispatch
	else if (ev->treat_as_mouse_event)
		_dispatchMouseEvent(ev);
	
	// Event type subscriptions
	else if ((it1 = _s->type_subscriptions.find(ev->type)) != _s->type_subscriptions.end())
		it1->second.back()->call(ev);
}
#include <iostream>
void W::Messenger::_dispatchMouseEvent(Event *ev) {
//	printf("Dispatching mouse event (type %d) at %d,%d %.1f,%.1f\n", ev->type, ev->pos.x, ev->pos.y, ev->pos.a, ev->pos.b);
	
	// Find last MappedObj that overlaps the event’s location
	Callback *last_sub = NULL;
	_callback_list *sublst = &_s->type_subscriptions[ev->type];
	for (_callback_list::iterator it = sublst->begin(); it != sublst->end(); it++) {
		MappedObj *o = (MappedObj*) (*it)->resp;
		if (o->overlapsWith(ev->pos))
			last_sub = *it;
	}
	if (last_sub != NULL) {
		last_sub->call(ev);
		return;
	}
//	else {
//		std::cout << "  no sub" << std::endl;
//	}
}
