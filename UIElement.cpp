/*
 * W - a tiny 2D game development library
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
#include "DrawingClasses.h"

W::UIElement::UIElement(const std::string &_name, W::Positioner *_pos, View *_v) :
	name(_name), positioner(_pos), view(_v)
{
	// hai element
}
W::UIElement::~UIElement()
{
	delete positioner;
}

void W::UIElement::_updatePosition(v2i _s) {
	rct = positioner->refresh(_s);
	updatePosition();
}


/**********/
/* Button */
/**********/

W::Button::Button(std::string _name, W::Positioner *_pos, View *_v) :
	UIElement(_name, _pos, _v),
	hover(false), active(false),
	buttonClickEvent(EventType::ButtonClick),
	btnrect(NULL)
{
	buttonClickEvent.payload = name;
	
	Callback cb(&Button::recEv, this);
	Messenger::subscribeInView(view, EventType::MouseMove, cb, &rct);
	Messenger::subscribeInView(view, EventType::LMouseDown, cb, &rct);
	Messenger::subscribeInView(view, EventType::LMouseUp, cb, &rct);
}
W::Button::~Button()
{
	if (btnrect) delete btnrect;
	Messenger::unsubscribeInView(view, EventType::MouseMove, this);
	Messenger::unsubscribeInView(view, EventType::LMouseDown, this);
	Messenger::unsubscribeInView(view, EventType::LMouseUp, this);
}
W::EventPropagation::T W::Button::recEv(W::Event *ev) {
	using namespace EventType;
	
	if (ev->type == LMouseDown) {
		active = true;
		Messenger::requestPrivilegedEventResponderStatus(view, MouseMove, Callback(&Button::recEv, this));
		Messenger::requestPrivilegedEventResponderStatus(view, LMouseUp, Callback(&Button::recEv, this));
		btnrect->setCol(W::Colour::Red);
	}
	else if (ev->type == MouseMove) {
		if (active) {
			if (!rct.overlapsWith(ev->pos)) {
				hover = false;
				Messenger::relinquishPrivilegedEventResponderStatus(view, MouseMove, this);
				Messenger::relinquishPrivilegedEventResponderStatus(view, LMouseUp, this);
				btnrect->setCol(W::Colour::Black);
			}
		}
		else {
			hover = true;
		}
	}
	else if (ev->type == LMouseUp) {
		if (active) {
			active = false;
			Messenger::relinquishPrivilegedEventResponderStatus(view, MouseMove, this);
			Messenger::relinquishPrivilegedEventResponderStatus(view, LMouseUp, this);
			btnrect->setCol(W::Colour::Black);
			if (rct.overlapsWith(ev->pos)) {
				Messenger::dispatchEvent(&buttonClickEvent);
			}
		}
	}
	
	return EventPropagation::ShouldStop;
}
void W::Button::updatePosition() {
  if (btnrect) {
    btnrect->setPos(rct.position);
    btnrect->setSz(rct.size);
  }
}
void W::Button::activate() {
	btnrect = new Rectangle(view, rct.position, rct.size, Colour::Black);
}
void W::Button::deactivate() {
	if (btnrect) {
		delete btnrect;
		btnrect = NULL;
	}
}
