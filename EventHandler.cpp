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

void W::EventHandler::dispatchEvent(Event *ev) {
	std::map<EventType::T, callback_list>::iterator it1;
	std::map<EventType::T, Callback *>::iterator it2;
	
	// If a general P.E.R. exists, send event to it
	if (privileged_event_responder != NULL)
		privileged_event_responder->call(ev);
	
	// If a P.E.R. exists specifically for this event type, send event to it
	else if ((it2 = type_pers.find(ev->type)) != type_pers.end())
		it2->second->call(ev);
	
	// Mouse dispatch
	else if (ev->treat_as_mouse_event) _dispatchMouseEvent(ev);
	
	// Event type subscriptions
	else if ((it1 = type_subscriptions.find(ev->type)) != type_subscriptions.end())
		it1->second.back()->call(ev);
}
#include <iostream>
void W::EventHandler::_dispatchMouseEvent(Event *ev) {
//	printf("Dispatching mouse event (type %d) at %d,%d %.1f,%.1f\n", ev->type, ev->pos.x, ev->pos.y, ev->pos.a, ev->pos.b);
	
	// Find last MappedObj that overlaps the event’s location
	Callback *last_sub = NULL;
	callback_list *sublst = &type_subscriptions[ev->type];
	for (callback_list::iterator it = sublst->begin(); it != sublst->end(); it++) {
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
