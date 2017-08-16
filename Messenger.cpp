/*
 * W - a tiny 2D game development library
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
#include "View.h"
#include "GameState.h"
#include <iostream>

using std::map;
using std::vector;
using std::string;

#pragma mark MState

struct cbAndRect {
	W::Callback *cb;
	W::rect *rct;
	cbAndRect(W::Callback *_cb, W::rect *_rct)
	{
		cb = _cb;
		rct = _rct;
	}
	~cbAndRect()
	{
		delete cb;
	}
};

struct W::Messenger::MState {
	MState()
	{
		// hai
	}
	~MState()
	{
		// TODO: destroy everything the user has omitted to unsubscribe
	}
	
	map<EventType::T, vector<Callback*>>              typeSubs;
	map<View*, map<EventType::T, vector<cbAndRect*>>> positionalSubs;
	map<int, Callback*>                               touchSubs;
	map<string, map<EventType::T, vector<Callback*>>> uiSubs;
	
	map<View*, map<EventType::T, Callback *>> typePers;
};


/********************************/
/*** Messenger implementation ***/
/********************************/

std::map<W::GameState*, W::Messenger::MState*> W::Messenger::stateMap;
W::Messenger::MState *W::Messenger::s = NULL;
W::GameState *W::Messenger::activeGS = NULL;

/*
 * Dispatch:
 *	All events are dispatched through the dispatchEvent() method.
 *  - Positional events:
 *		- finds the appropriate view V, if any
 *		- if event is a RawX type, its pos is translated via V
 *		- event then dispatched positionally, to V's subscribers, if any, or
 *		- touch-ically, if a TouchMoved/Up/Cancelled event
 * - UIEvents:
 *		- dispatched UI-istically
 * - Normal events:
 *		- dispatched normally (by type)
 */

#pragma mark - Dispatch methods

bool W::Messenger::dispatchEvent(Event *ev) {
	if (!s) return false;
	
	if (ev->_isPositional()) {
		if (!activeGS) return false;
		
		View *v = NULL;
		for (GameState::Viewlist::reverse_iterator itV = activeGS->_vlist.rbegin(); itV != activeGS->_vlist.rend(); ++itV)
			if ((*itV)->getRct().overlapsWith(ev->pos)) {
				v = *itV;
				break;
			}
		if (!v) return false;
		
		if (ev->_isRaw()) {
			v->_convertEventCoords(ev);
			ev->_derawify();
		}
		
		if (dispatchToPERs(ev, v)) return true;
		if (ev->_isTouch()) return dispatchTouchically(ev);
		else                return dispatchPositionally(ev, v);
	}
	
	if (ev->_isUI()) return dispatchUIEvent(ev);
	
	// Event type subscriptions
	bool dispatchedByType = false;
	map<EventType::T, vector<Callback*>>::iterator it = s->typeSubs.find(ev->type);
	if (it != s->typeSubs.end()) {
		vector<Callback*> &cblist = it->second;
		for (std::vector<Callback*>::reverse_iterator it2 = cblist.rbegin(); it2 != cblist.rend(); ++it2)
			if (cblist.size()) {
				Callback &cb = **it2;
				dispatchedByType = true;
				if (cb.call(ev) == W::EventPropagation::ShouldStop)
					break;
			}
	}
	return dispatchedByType;
}

bool W::Messenger::dispatchPositionally(Event *ev, View *v) {
	if (!s) return false;
	
	// If no view in s.positionalSubs, return false
	map<View*, map<EventType::T, vector<cbAndRect*>>>::iterator itV = s->positionalSubs.find(v);
	if (itV == s->positionalSubs.end())
		return false;
	
	// If no subscriptions to this event type for that view, return false
	map<EventType::T, vector<cbAndRect*>> &typeSubsForView = itV->second;
	map<EventType::T, vector<cbAndRect*>>::iterator itT = typeSubsForView.find(ev->type);
	if (itT == typeSubsForView.end())
		return false;
	
	// Call callbacks sub'd to this event type for this view, in reverse order
	bool dispatched = false;
	std::vector<cbAndRect*> &callbacks = itT->second;
	for (std::vector<cbAndRect*>::reverse_iterator it = callbacks.rbegin(); it != callbacks.rend(); ++it) {
		cbAndRect &cnr = **it;
		if (cnr.rct->overlapsWith(ev->pos)) {
			dispatched = true;
			if (cnr.cb->call(ev) == W::EventPropagation::ShouldStop)
				break;
		}
	}
	return dispatched;
}

bool W::Messenger::dispatchTouchically(W::Event *ev) {
	if (!s) return false;
	
	std::map<int, Callback*>::iterator it = s->touchSubs.find(ev->touchID);
	if (it != s->touchSubs.end()) {
		it->second->call(ev);
		return true;
	}

	return false;
}

bool W::Messenger::dispatchUIEvent(W::Event *ev) {
	if (!s) return false;
	
	const std::string &elname = *(std::string*)(ev->_payload);
	
	// If no subscriptions to this element, return false
	map<string, map<EventType::T, vector<Callback*>>>::iterator it1 = s->uiSubs.find(elname);
	if (it1 == s->uiSubs.end())
		return false;
	
	// If no subscriptions to this event type for this element, return false
	map<EventType::T, vector<Callback*>> &subs_for_element = it1->second;
	map<EventType::T, vector<Callback*>>::iterator it2 = subs_for_element.find(ev->type);
	if (it2 == subs_for_element.end())
		return false;
	
	// Call callbacks sub'd to this event type for this element
	bool dispatched = false;
	std::vector<Callback*> &callbacks = it2->second;
	std::vector<Callback*>::iterator it3;
	for (it3 = callbacks.begin(); it3 < callbacks.end(); it3++) {
		dispatched = true;
		(*it3)->call(ev);
	}
	
	return dispatched;
}

bool W::Messenger::dispatchToPERs(W::Event *ev, View *v) {
	if (!s) return false;
	
	// If no P.E.R. type maps exist for this view, return false
	map<View*, map<EventType::T, Callback*>>::iterator itV = s->typePers.find(v);
	if (itV == s->typePers.end())
		return false;
	
	// Call PER callback if exists for this event type in this view
	map<EventType::T, Callback*> &persForView = itV->second;
	map<EventType::T, Callback*>::iterator it = persForView.find(ev->type);
	if (it != persForView.end()) {
		it->second->call(ev);
		return true;
	}
	
	return false;
}


/*
 * Subscription & unsubscription methods
 *
 */

#pragma mark - Subscription methods

void W::Messenger::subscribe(EventType::T t, const Callback &c) {
	if (!s) return;
	unsubscribe(t, c.resp);
	s->typeSubs[t].push_back(c.copy());
}
void W::Messenger::unsubscribe(EventType::T t, void *r) {
	if (!s) return;
	vector<Callback*> &cblist = s->typeSubs[t];
	for (vector<Callback*>::iterator it = cblist.begin(); it < cblist.end(); )
		if ((*it)->resp == r) {
			delete *it;
			it = cblist.erase(it);
		}
		else it++;
}

void W::Messenger::subscribeInView(View *v, W::EventType::T t, const W::Callback &c, rect *rct) {
	if (!s) return;
	unsubscribeInView(v, t, c.resp);
	s->positionalSubs[v][t].push_back(new cbAndRect(c.copy(), rct));
}
void W::Messenger::unsubscribeInView(View *v, W::EventType::T t, void *r) {
	if (!s) return;
	
	// If no map of type subs for this view, return
	map<View*, map<EventType::T, vector<cbAndRect*>>>::iterator itV = s->positionalSubs.find(v);
	if (itV == s->positionalSubs.end()) return;
	
	// If map of type subs does not have entry for this type, return
	map<EventType::T, vector<cbAndRect*>> &typeSubsForView = itV->second;
	map<EventType::T, vector<cbAndRect*>>::iterator itT = typeSubsForView.find(t);
	if (itT == typeSubsForView.end()) return;
	
	// Iterate over entries, deleting if resp == r
	std::vector<cbAndRect*> &callbacks = itT->second;
	for (std::vector<cbAndRect*>::iterator it = callbacks.begin(); it < callbacks.end(); )
		if ((*it)->cb->resp == r) {
			delete *it;
			it = callbacks.erase(it);
		}
		else ++it;
	
	// If no more callbacks for this type, delete type subscription for view
	if (callbacks.empty())
		typeSubsForView.erase(itT);
	
	// If no more type subscriptions for this view, delete view from s.positionalSubs
	if (typeSubsForView.empty())
		s->positionalSubs.erase(itV);
}

void W::Messenger::subscribeToMouseEvents(W::View *v, const W::Callback &c, W::rect *rct) {
	if (!s) return;
	using namespace EventType;
	subscribeInView(v, MouseMove, c, rct);
	subscribeInView(v, LMouseDown, c, rct);
	subscribeInView(v, RMouseDown, c, rct);
	subscribeInView(v, LMouseUp, c, rct);
	subscribeInView(v, RMouseUp, c, rct);
}
void W::Messenger::unsubscribeFromMouseEvents(W::View *v, void *r) {
	if (!s) return;
	using namespace EventType;
	unsubscribeInView(v, MouseMove, r);
	unsubscribeInView(v, LMouseDown, r);
	unsubscribeInView(v, RMouseDown, r);
	unsubscribeInView(v, LMouseUp, r);
	unsubscribeInView(v, RMouseUp, r);
}

bool W::Messenger::subscribeToTouchEvent(int touchID, const W::Callback &c) {
	if (!s) return false;
	if (s->touchSubs.find(touchID) != s->touchSubs.end())
		return false;
	s->touchSubs[touchID] = c.copy();
	return true;
}
void W::Messenger::unsubscribeFromTouchEvent(int touchID, void *r) {
	if (!s) return;
	
	map<int, Callback*>::iterator it = s->touchSubs.find(touchID);
	if (it == s->touchSubs.end()) return;
	if (it->second->resp == r) {
		delete it->second;
		s->touchSubs.erase(it);
	}
}

void W::Messenger::subscribeToUIEvent(const char *_elname, EventType::T t, const Callback &c) {
	if (!s) return;
	unsubscribeFromUIEvent(_elname, t, c.resp);
	s->uiSubs[_elname][t].push_back(c.copy());
}
void W::Messenger::unsubscribeFromUIEvent(const char *elname, EventType::T t, void *r) {
	if (!s) return;
	
	// If no map of type subs for this element, return
	map<string, map<EventType::T, vector<Callback*>>>::iterator itE = s->uiSubs.find(elname);
	if (itE == s->uiSubs.end())
		return;
	
	// If map of type subs does not have an entry for this type, return
	map<EventType::T, vector<Callback*>> &typeSubsForElement = itE->second;
	map<EventType::T, vector<Callback*>>::iterator itT = typeSubsForElement.find(t);
	if (itT == typeSubsForElement.end())
		return;
	
	// Iterate over entries, deleting if resp == r
	vector<Callback*> &callbacks = itT->second;
	for (vector<Callback*>::iterator it = callbacks.begin(); it < callbacks.end(); )
		if ((*it)->resp == r) {
			delete *it;
			it = callbacks.erase(it);
		}
		else it++;
	
	// If no more callbacks for this type, delete type subscription for element
	if (callbacks.empty())
		typeSubsForElement.erase(itT);
	
	// If no more type subscriptoins for this element, delete element from uiSubs
	if (typeSubsForElement.empty())
		s->uiSubs.erase(itE);
}


#pragma mark - Privileged Event Responder methods

bool W::Messenger::requestPrivilegedEventResponderStatus(View *v, EventType::T t, const Callback &c) {
	if (!s) return false;
	
	// Check if exists already (without creating map entries)
	map<View*, map<EventType::T, Callback*>>::iterator itV = s->typePers.find(v);
	if (itV != s->typePers.end()) {
		map<EventType::T, Callback*> &typePersForView = itV->second;
		map<EventType::T, Callback*>::iterator itT = typePersForView.find(t);
		if (itT != typePersForView.end())
			return false;
	}
	// Add PER
	s->typePers[v][t] = c.copy();
	return true;
}
void W::Messenger::relinquishPrivilegedEventResponderStatus(View *v, EventType::T t, void *r) {
	if (!s) return;
	
	// Check type sub map exists for this view
	map<View*, map<EventType::T, Callback*>>::iterator itV = s->typePers.find(v);
	if (itV == s->typePers.end())
		return;
	
	// Delete callback if exists for this type & resp == r
	map<EventType::T, Callback*> &typePersForView = itV->second;
	map<EventType::T, Callback*>::iterator it = typePersForView.find(t);
	if (it != typePersForView.end() && it->second->resp == r) {
		delete it->second;
		typePersForView.erase(it);
		// If no more PERs for view, remove from s.typePers
		if (typePersForView.empty())
			s->typePers.erase(itV);
	}
}


#pragma mark - GameState/MState management

void W::Messenger::_useTemporaryState() {
	_gamestateDestroyed(NULL);
	s = new MState;
}
void W::Messenger::_setActiveGamestate(W::GameState *_gs) {
	map<GameState*, MState*>::iterator it;
	// If temporary state, save it as this GS’s state
	if (s)
		stateMap[_gs] = s;
	// If GS has saved state, set as current
	else if ((it = stateMap.find(_gs)) != stateMap.end())
		s = it->second;
	// Otherwise, create a new state for this GS
	else
		s = stateMap[_gs] = new MState;
	
	activeGS = _gs;
}
void W::Messenger::_gamestateDestroyed(W::GameState *_gs) {
	delete stateMap[_gs];
	stateMap.erase(_gs);
	s = NULL;
	activeGS = NULL;
}

