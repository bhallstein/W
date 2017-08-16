/*
 * W - a tiny 2D game develpoment library
 *
 * =================
 *  Messenger.cpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Messenger.h"
#include "Callback.h"
#include <iostream>

namespace W {

	struct cbAndRect {
		Callback *cb;
		rect *rct;
		cbAndRect(Callback *_cb, rect *_rct) : cb(_cb), rct(_rct) { }
		~cbAndRect() {
			delete cb;
		}
	};
	struct Messenger::_messenger_state {
		std::map<EventType::T, std::vector<Callback*>> type_subscriptions;
		std::map<EventType::T, std::vector<cbAndRect*>> positional_subscriptions;
		std::map<int, Callback*> touch_subscriptions;
		
		Callback *privileged_event_responder;
		std::map<EventType::T, Callback *> type_pers;
		
		std::map<std::string, std::map<
			EventType::T, std::vector<Callback*>
		> > ui_subscriptions;
		
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
	std::map<GameState*, _messenger_state*>::iterator it;
	// If temporary state, save it as this GS’s state
	if (_s)
		_stateMap[_gs] = _s;
	// If GS has saved state, set as current
	else if ((it = _stateMap.find(_gs)) != _stateMap.end())
		_s = it->second;
	// Otherwise, create a new state for this GS
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
void W::Messenger::unsubscribeFromEventType(EventType::T t, void *r) {
	if (!_s) return;
	std::vector<Callback*> &cblist = _s->type_subscriptions[t];
	for (std::vector<Callback*>::iterator it = cblist.begin(); it < cblist.end(); )
		if ((*it)->resp == r) {
			delete *it;
			it = cblist.erase(it);
		}
		else it++;
}

void W::Messenger::subscribeToPositionalEventType(W::EventType::T t, const W::Callback &c, rect *rct) {
	if (!_s) return;
	unsubscribeFromPositionalEventType(t, c.resp);
	_s->positional_subscriptions[t].push_back(new cbAndRect(c.copy(), rct));
}
void W::Messenger::unsubscribeFromPositionalEventType(W::EventType::T t, void *r) {
	if (!_s) return;
	std::vector<cbAndRect*> &pos_sublist = _s->positional_subscriptions[t];
	for (std::vector<cbAndRect*>::iterator it = pos_sublist.begin(); it < pos_sublist.end(); )
		if ((*it)->cb->resp == r) {
			delete *it;
			it = pos_sublist.erase(it);
		}
		else
			it++;
}

void W::Messenger::subscribeToMouseEvents(const Callback &c, rect *rct) {
	if (!_s) return;
	subscribeToPositionalEventType(EventType::MOUSEMOVE, c, rct);
	subscribeToPositionalEventType(EventType::LEFTMOUSEDOWN, c, rct);
	subscribeToPositionalEventType(EventType::LEFTMOUSEUP, c, rct);
	subscribeToPositionalEventType(EventType::RIGHTMOUSEDOWN, c, rct);
	subscribeToPositionalEventType(EventType::RIGHTMOUSEUP, c, rct);
}
void W::Messenger::unsubscribeFromMouseEvents(void *r) {
	if (!_s) return;
	unsubscribeFromPositionalEventType(EventType::MOUSEMOVE, r);
	unsubscribeFromPositionalEventType(EventType::LEFTMOUSEDOWN, r);
	unsubscribeFromPositionalEventType(EventType::LEFTMOUSEUP, r);
	unsubscribeFromPositionalEventType(EventType::RIGHTMOUSEDOWN, r);
	unsubscribeFromPositionalEventType(EventType::RIGHTMOUSEUP, r);
}

bool W::Messenger::requestPrivilegedEventResponderStatus(const Callback &c) {
	if (!_s) return false;
	if (_s->privileged_event_responder != NULL) return false;
	_s->privileged_event_responder = c.copy();
	return true;
}
void W::Messenger::relinquishPrivilegedEventResponderStatus(void *r) {
	if (!_s) return;
	if (_s->privileged_event_responder)
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
void W::Messenger::relinquishPrivilegedResponderStatusForEventType(EventType::T t, void *r) {
	if (!_s) return;
	std::map<EventType::T, Callback*>::iterator it = _s->type_pers.find(t);
	if (it != _s->type_pers.end() && it->second->resp == r) {
		delete it->second;
		_s->type_pers.erase(it);
	}
}

bool W::Messenger::dispatchEvent(Event *ev) {
	if (!_s) return false;
	if (_dispatchToPERs(ev)) return true;
	
	// Event type subscriptions
	bool dispatched = false;
	std::map<EventType::T, std::vector<Callback*>>::iterator it = _s->type_subscriptions.find(ev->type);
	if (it != _s->type_subscriptions.end()) {
		std::vector<Callback*> &cblist = it->second;
		for (std::vector<Callback*>::reverse_iterator it2 = cblist.rbegin(); it2 != cblist.rend(); ++it2)
			if (cblist.size()) {
				Callback &cb = **it2;
				dispatched = true;
				if (cb.call(ev) == W::EventPropagation::ShouldStop)
					break;
			}
	}
	return dispatched;
}
bool W::Messenger::dispatchPositionalEvent(Event *ev) {
	if (!_s) return false;
	if (_dispatchToPERs(ev)) return true;
	
	// Positional dispatch
	bool dispatched = false;
	std::vector<cbAndRect*> &pos_sublist = _s->positional_subscriptions[ev->type];
	for (std::vector<cbAndRect*>::reverse_iterator it = pos_sublist.rbegin(); it != pos_sublist.rend(); it++) {
		cbAndRect &cnr = **it;
		if (cnr.rct->overlapsWith(ev->pos)) {
			dispatched = true;
			if (cnr.cb->call(ev) == W::EventPropagation::ShouldStop)
				break;
		}
	}
	return dispatched;
}
void W::Messenger::_dispatchUIEvent(W::Event *ev) {
	if (!_s) return;
	
	std::string *elname = (std::string *)ev->_payload;
	
	std::map<std::string, std::map<
		EventType::T, std::vector<Callback*>
	> >::iterator it1 = _s->ui_subscriptions.find(*elname);
	if (it1 == _s->ui_subscriptions.end())
		return;
	
	std::map<EventType::T, std::vector<Callback*>> &subs_for_element = it1->second;
	std::map<EventType::T, std::vector<Callback*>>::iterator it2 = subs_for_element.find(ev->type);
	if (it2 == subs_for_element.end())
		return;
	
	std::vector<Callback*> &cblist = it2->second;
	std::vector<Callback*>::iterator it3;
	for (it3 = cblist.begin(); it3 < cblist.end(); it3++)
		(*it3)->call(ev);
}
bool W::Messenger::_dispatchToPERs(W::Event *ev) {
	if (!_s) return false;
	std::map<EventType::T, Callback *>::iterator it;
	
	// If a general P.E.R. exists, send event to it
	if (_s->privileged_event_responder != NULL) {
		_s->privileged_event_responder->call(ev);
		return true;
	}
	
	// If a P.E.R. exists specifically for this event type, send event to it
	it = _s->type_pers.find(ev->type);
	if (it != _s->type_pers.end()) {
		it->second->call(ev);
		return true;
	}
	
	return false;
}
void W::Messenger::_dispatchTouchEvent(W::Event *ev) {
	if (!_s) return;
	std::map<int, Callback*>::iterator it = _s->touch_subscriptions.find(ev->touchID);
	if (it != _s->touch_subscriptions.end()) {
		it->second->call(ev);
	}
}

void W::Messenger::subscribeToUIEvent(const char *_elname, EventType::T t, const Callback &c) {
	if (!_s) return;
	unsubscribeFromUIEvent(_elname, t, c.resp);
	
	std::map<EventType::T, std::vector<Callback*> > &subs_for_element = _s->ui_subscriptions[_elname];
	std::vector<Callback*> &subs_to_evtype_for_element = subs_for_element[t];
	subs_to_evtype_for_element.push_back(c.copy());
}
void W::Messenger::unsubscribeFromUIEvent(const char *_elname, EventType::T t, void *r) {
	if (!_s) return;
	
	std::map<std::string, std::map<
		EventType::T, std::vector<Callback*>
	> >::iterator it1 = _s->ui_subscriptions.find(_elname);
	if (it1 == _s->ui_subscriptions.end())
		return;
	
	std::map<EventType::T, std::vector<Callback*>>::iterator it2 = it1->second.find(t);
	if (it2 == it1->second.end())
		return;
	
	std::vector<Callback*> &cblist = it2->second;
	std::vector<Callback*>::iterator it3;
	for (it3 = cblist.begin(); it3 < cblist.end(); )
		if ((*it3)->resp == r) {
			delete *it3;
			it3 = cblist.erase(it3);
		}
		else it3++;
}

bool W::Messenger::subscribeToTouchEvent(int _touchID, const W::Callback &c) {
	if (!_s) return false;
	if (_s->touch_subscriptions.find(_touchID) != _s->touch_subscriptions.end())
		return false;
	_s->touch_subscriptions[_touchID] = c.copy();
	return true;
}
void W::Messenger::unsubscribeFromTouchEvent(int _touchID, void *r) {
	if (!_s) return;
	std::map<int, Callback*>::iterator it = _s->touch_subscriptions.find(_touchID);
	if (it == _s->touch_subscriptions.end()) return;
	if (it->second->resp != r) return;
	delete it->second;
	_s->touch_subscriptions.erase(it);
}
