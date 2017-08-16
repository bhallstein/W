/*
 * W - a tiny 2D game develpoment library
 *
 * =================
 *  UIElement.cpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "UIElement.h"
#include "Messenger.h"
#include "Callback.h"
#include "DObj.h"

W::UIElement::UIElement(const std::string &_name, W::Positioner *_pos, EvTypeMap &_evTypes) :
	name(_name), positioner(_pos), evTypes(_evTypes)
{
	// hai element
}
W::UIElement::~UIElement()
{
	delete positioner;
}

void W::UIElement::_updatePosition(const W::size &_s) {
	rct = positioner->refresh(_s);
	updatePosition();
}


/**********/
/* Button */
/**********/

W::Button::Button(View *_view, const std::string &_name, W::Positioner *_pos, EvTypeMap &_evTypes) :
	UIElement(_name, _pos, _evTypes),
	hover(false), active(false), view(_view),
	buttonClickEvent(EventType::BUTTONCLICK),
	btnrect(NULL)
{
	buttonClickEvent._payload = new std::string(name);
	
	Callback cb(&Button::recEv, this);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::MOUSEMOVE], cb, &rct);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::LEFTMOUSEDOWN], cb, &rct);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::LEFTMOUSEUP], cb, &rct);
}
W::Button::~Button()
{
	delete (std::string*)buttonClickEvent._payload;
	if (btnrect) delete btnrect;
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::MOUSEMOVE], this);
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::LEFTMOUSEDOWN], this);
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::LEFTMOUSEUP], this);
}
W::EventPropagation::T W::Button::recEv(W::Event *ev) {
	using namespace EventType;
	
	if (ev->type == evTypes[LEFTMOUSEDOWN]) {
		active = true;
		Messenger::requestPrivilegedResponderStatusForEventType(evTypes[MOUSEMOVE], Callback(&Button::recEv, this));
		Messenger::requestPrivilegedResponderStatusForEventType(evTypes[LEFTMOUSEUP], Callback(&Button::recEv, this));
	}
	else if (ev->type == evTypes[MOUSEMOVE]) {
		if (active) {
			if (!rct.overlapsWith(ev->pos)) {
				hover = false;
				Messenger::relinquishPrivilegedResponderStatusForEventType(evTypes[MOUSEMOVE], this);
				Messenger::relinquishPrivilegedResponderStatusForEventType(evTypes[LEFTMOUSEUP], this);
			}
		}
		else {
			hover = true;
		}
	}
	else if (ev->type == evTypes[LEFTMOUSEUP]) {
		if (active) {
			active = false;
			Messenger::relinquishPrivilegedResponderStatusForEventType(evTypes[MOUSEMOVE], this);
			Messenger::relinquishPrivilegedResponderStatusForEventType(evTypes[LEFTMOUSEUP], this);
			if (rct.overlapsWith(ev->pos))
				Messenger::_dispatchUIEvent(&buttonClickEvent);
		}
	}
	
	return EventPropagation::ShouldStop;
}
void W::Button::updatePosition() {
	btnrect->setPos(rct.pos);		// Update D.O.
	btnrect->setSz(rct.sz);
}
void W::Button::activate() {
	btnrect = new DRect(view, rct.pos, rct.sz, Colour::Black);
}
void W::Button::deactivate() {
	if (btnrect) {
		delete btnrect;
		btnrect = NULL;
	}
}
