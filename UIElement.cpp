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
	buttonClickEvent(EventType::ButtonClick),
	btnrect(NULL)
{
	buttonClickEvent._payload = new std::string(name);
	
	Callback cb(&Button::recEv, this);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::MouseMove], cb, &rct);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::LMouseDown], cb, &rct);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::LMouseUp], cb, &rct);
}
W::Button::~Button()
{
	delete (std::string*)buttonClickEvent._payload;
	if (btnrect) delete btnrect;
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::MouseMove], this);
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::LMouseDown], this);
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::LMouseUp], this);
}
W::EventPropagation::T W::Button::recEv(W::Event *ev) {
	using namespace EventType;
	
	if (ev->type == evTypes[LMouseDown]) {
		active = true;
		Messenger::requestPrivilegedResponderStatusForEventType(evTypes[MouseMove], Callback(&Button::recEv, this));
		Messenger::requestPrivilegedResponderStatusForEventType(evTypes[LMouseUp], Callback(&Button::recEv, this));
	}
	else if (ev->type == evTypes[MouseMove]) {
		if (active) {
			if (!rct.overlapsWith(ev->pos)) {
				hover = false;
				Messenger::relinquishPrivilegedResponderStatusForEventType(evTypes[MouseMove], this);
				Messenger::relinquishPrivilegedResponderStatusForEventType(evTypes[LMouseUp], this);
			}
		}
		else {
			hover = true;
		}
	}
	else if (ev->type == evTypes[LMouseUp]) {
		if (active) {
			active = false;
			Messenger::relinquishPrivilegedResponderStatusForEventType(evTypes[MouseMove], this);
			Messenger::relinquishPrivilegedResponderStatusForEventType(evTypes[LMouseUp], this);
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
