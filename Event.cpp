/*
 * W - a simple, cross-platform 2D game develpoment library
 *
 * =============
 *  Event.cpp
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Event.h"
#if defined WTARGET_MAC
	#include <Cocoa/Cocoa.h>
#endif

W::EventType::T W::Event::_typecounter = 100;
std::vector<W::Event*> W::Event::_events;
W::Mutex W::Event::_mutex;

W::Event::Event(EventType::T _type) : type(_type) { }
W::Event::Event(EventType::T _type, const position &_pos) : type(_type), pos(_pos) { }
W::Event::Event(EventType::T _type, KeyCode::T _key) : type(_type), key(_key) { }
W::Event::Event(EventType::T _type, float _x) : type(_type), x(_x) { }

W::EventType::T W::Event::registerType() {
	return _typecounter++;
}
W::KeyCode::T W::Event::charToKeycode(unsigned int c) {
	if (c == ' ') return W::KeyCode::SPACE;
	if (c >= 'a' && c <= 'z') return (W::KeyCode::T) ((int)W::KeyCode::_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (W::KeyCode::T) ((int)W::KeyCode::_A + c - 'A');
	if (c >= '0' && c <= '9') return (W::KeyCode::T) ((int)W::KeyCode::_0 + c - '0');
	if (c == 27) return W::KeyCode::ESC;		// These are standard ASCII codes
	if (c == 13) return W::KeyCode::RETURN;		//
	if (c == 8)  return W::KeyCode::BACKSPACE;	//
	if (c == 9)  return W::KeyCode::TAB;		//
	#ifdef WTARGET_MAC
		if (c == NSLeftArrowFunctionKey)  return W::KeyCode::LEFT_ARROW;
		if (c == NSRightArrowFunctionKey) return W::KeyCode::RIGHT_ARROW;
		if (c == NSUpArrowFunctionKey)    return W::KeyCode::UP_ARROW;
		if (c == NSDownArrowFunctionKey)  return W::KeyCode::DOWN_ARROW;
	#endif
	return W::KeyCode::K_OTHER;
}
void W::Event::_addEvent(W::Event *ev) {
	#ifdef WTARGET_WIN
		_mutex.lock();
	#endif
	
	Event::_events.push_back(ev);
	
	#ifdef WTARGET_WIN
		_mutex.unlock();
	#endif
}
