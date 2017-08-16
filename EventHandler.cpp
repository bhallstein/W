#include "EventHandler.h"
#include "Callback.h"
#include "EventResponder.h"
#include "MappedObj.h"

#include <iostream>

W::EventHandler::EventHandler() : privileged_event_responder(NULL)
{
	// hai eventhandler
}
W::EventHandler::~EventHandler()
{
	if (privileged_event_responder) delete privileged_event_responder;
	for (std::map<EventType::T, Callback*>::iterator it = type_pers.begin(); it != type_pers.end(); it++) {
		delete it->second;
	}
	// Is more cleanup required here?
	// Hopefully what has been added will be unsubscribed before the EH is
	// destroyed. However...
}

void W::EventHandler::subscribeToMouseEvents(const Callback &c) {
	subscribeToEventType(EventType::MOUSEMOVE, c);
	subscribeToEventType(EventType::LEFTMOUSEDOWN, c);
	subscribeToEventType(EventType::LEFTMOUSEUP, c);
	subscribeToEventType(EventType::RIGHTMOUSEDOWN, c);
	subscribeToEventType(EventType::RIGHTMOUSEUP, c);
}
void W::EventHandler::unsubscribeFromMouseEvents(EventResponder *r) {
	unsubscribeFromEventType(EventType::MOUSEMOVE, r);
	unsubscribeFromEventType(EventType::LEFTMOUSEDOWN, r);
	unsubscribeFromEventType(EventType::LEFTMOUSEUP, r);
	unsubscribeFromEventType(EventType::RIGHTMOUSEDOWN, r);
	unsubscribeFromEventType(EventType::RIGHTMOUSEUP, r);
}

bool W::EventHandler::requestPrivilegedEventResponderStatus(const Callback &c) {
	if (privileged_event_responder != NULL) return false;
	privileged_event_responder = c.copy();
	return true;
}
void W::EventHandler::relinquishPrivilegedEventResponderStatus(EventResponder *r) {
	if (privileged_event_responder->resp == r) {
		delete privileged_event_responder;
		privileged_event_responder = NULL;
	}
}
bool W::EventHandler::requestPrivilegedResponderStatusForEventType(EventType::T t, const Callback &c) {
	if (type_pers.find(t) != type_pers.end())
		return false;
	type_pers[t] = c.copy();
	return true;
}
void W::EventHandler::relinquishPrivilegedResponderStatusForEventType(EventType::T t, EventResponder *r) {
	std::map<EventType::T, Callback*>::iterator it = type_pers.find(t);
	if (it == type_pers.end())
		return;
	if (it->second->resp == r) {
		delete it->second;
		type_pers.erase(it);
	}
}

void W::EventHandler::subscribeToKey(KeyCode::T k, const Callback &c) {
	unsubscribeFromKey(k, c.resp);
	keycode_subscriptions[k].push_back(c.copy());
}
void W::EventHandler::unsubscribeFromKey(KeyCode::T k, EventResponder *r) {
	callback_list *l = &keycode_subscriptions[k];
	for (callback_list::iterator it = l->begin(); it < l->end(); it++)
		if ((*it)->resp == r) {
			delete *it;
			l->erase(it);
			return;
		}
}
void W::EventHandler::unsubscribeFromAllKeys(EventResponder *r) {
	for (std::map<KeyCode::T, callback_list>::iterator it = keycode_subscriptions.begin(); it != keycode_subscriptions.end(); it++)
		unsubscribeFromKey(it->first, r);
}

void W::EventHandler::subscribeToEventType(EventType::T t, const Callback &c) {
	unsubscribeFromEventType(t, c.resp);
	type_subscriptions[t].push_back(c.copy());
}
void W::EventHandler::unsubscribeFromEventType(EventType::T t, EventResponder *r) {
	callback_list *l = &type_subscriptions[t];
	for (callback_list::iterator it = l->begin(); it < l->end(); it++)
		if ((*it)->resp == r) {
//			std::cout << "unsub from event type " << t  << ": deleting subscription..." << std::endl;
			delete *it;
			l->erase(it);
			return;
		}
}

void W::EventHandler::dispatchEvent(Event *ev) {
	// Mouses
	if (ev->treat_as_mouse_event) 
		_dispatchMouseEvent(ev);
	// Subscriptions to event types
	else if (type_subscriptions.count(ev->type))
		type_subscriptions[ev->type].back()->call(ev);
	// Keys
	else if (ev->type == EventType::KEYPRESS && keycode_subscriptions.count(ev->key))
		keycode_subscriptions[ev->key].back()->call(ev);
}
#include <iostream>
void W::EventHandler::_dispatchMouseEvent(Event *ev) {
//	printf("Dispatching mouse event (type %d) at %d,%d %.1f,%.1f\n", ev->type, ev->pos.x, ev->pos.y, ev->pos.a, ev->pos.b);

	// If general P.E.R. exists, send event to it
	if (privileged_event_responder != NULL) {
		privileged_event_responder->call(ev);
		return;
	}
	
	// If P.E.R. exists specifically for this event type, send event to it
	std::map<EventType::T, Callback*>::iterator it = type_pers.find(ev->type);
	if (it != type_pers.end()) {
		it->second->call(ev);
		return;
	}
	
	// Find last MappedObj that overlaps the event’s location
	Callback *last_sub = NULL;
	callback_list *sublst = &type_subscriptions[ev->type];
	for (callback_list::iterator it = sublst->begin(); it != sublst->end(); it++) {
		MappedObj *o = dynamic_cast<MappedObj*>((*it)->resp);
		if (o == NULL)
			continue;
		for (std::vector<rect>::iterator itpl = o->plan.begin(); itpl < o->plan.end(); itpl++) {
			float xObj = o->pos.x + o->pos.a + itpl->pos.x + itpl->pos.a;
			float yObj = o->pos.y + o->pos.b + itpl->pos.y + itpl->pos.b;
			float xEv = ev->pos.x + ev->pos.a;
			float yEv = ev->pos.y + ev->pos.b;
			if (xEv >= xObj && yEv >= yObj && xEv <= xObj + itpl->sz.width && yEv <= yObj + itpl->sz.height)
				last_sub = *it;
		}
	}
	if (last_sub != NULL) {
		last_sub->call(ev);
		return;
	}
	else {
//		std::cout << "  no sub" << std::endl;
	}
}
