#include "W.h"

#include "UIView.h"
#include "Callback.h"
#include "Messenger.h"
#include "Positioner.h"
#include "Window.h"

#include <iostream>

//W::Button::Button(const rect &_r, const char *_action) :
//	_col(Colour::TransparentBlack), _hovcol(Colour::Red), hover(false)
//{
//	pos = _r.pos;
//	plan.resize(1);
//	plan[0].sz = _r.sz;
//	event.setType(EventType::BUTTONCLICK);
//	event.setPayload(new std::string(_action));
//}
//W::Button::~Button()
//{
//	delete (std::string *)event._payload;
//}
//
//void W::Button::setHover() {
//	hover = true;
//}
//W::Colour& W::Button::col() {
//	if (hover) { hover = false; return _hovcol; }
//	return _col;
//}
//W::Event* W::Button::getEvent() {
//	return &event;
//}


W::UIView::UIView(Positioner *_pos, Window *_win, Draggability _allowDrag) :
	View(_pos, _win),
	allowDrag(_allowDrag), dragloop(false)
{
	createEvTypeMap();	// Make event type translation map for this uiview
}
W::UIView::~UIView()
{
	// bai then
}

void W::UIView::processMouseEvent(Event *ev) {
	if (dragloop) {
		if (ev->type == EventType::MOUSEMOVE) {
			_positioner->setCorner(pos.x + ev->pos.x - drag_initial.x, pos.y + ev->pos.y - drag_initial.y);
			size s = _window->_getDimensions();
			_updatePosition(&s);
		}
		else if (ev->type == EventType::LEFTMOUSEUP) {
			Messenger::relinquishPrivilegedEventResponderStatus(this);
			dragloop = false;
		}
		return;
	}
	
	// If an element is under event, translate & resubmit.
	
	// Otherwise, if allowDrag, do dragloopery
	
//	Button *b = NULL;
//	// Get last button that lies under the mouse
//	for (std::vector<Button*>::iterator it = buttons.begin(); it < buttons.end(); it++) {
//		Button *c = *it;
//		if (c->pos.x <= ev->pos.x && c->pos.x + c->plan[0].sz.width > ev->pos.x
//			&& c->pos.y <= ev->pos.y && c->pos.y + c->plan[0].sz.height > ev->pos.y)
//			b = c;
//	}
//	if (b == NULL) {
//		if (allowDrag && ev->type == EventType::LEFTMOUSEDOWN && Messenger::requestPrivilegedEventResponderStatus(Callback(&View::receiveEvent, (View*)this))) {
//			dragloop = true;
//			drag_initial = ev->pos;
//		}
//		return;
//	}
}

void W::UIView::createEvTypeMap() {
	using namespace EventType;
	evTypeMap[MOUSEMOVE]      = Event::registerType();
	evTypeMap[LEFTMOUSEDOWN]  = Event::registerType();
	evTypeMap[LEFTMOUSEUP]    = Event::registerType();
	evTypeMap[RIGHTMOUSEDOWN] = Event::registerType();
	evTypeMap[RIGHTMOUSEUP]   = Event::registerType();
}
