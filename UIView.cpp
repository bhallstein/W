#include "W.h"

#include "UIView.h"
#include "Callback.h"
#include "EventHandler.h"
#include "Positioner.h"
#include "Window.h"

#include <iostream>

W::Button::Button(int _x, int _y, int _width, int _height, const char *_action) :
	_col(Colour::TransparentBlack), _hovcol(Colour::Red), hover(false)
{
	pos.x = _x, pos.y = _y;
	plan.resize(1);
	rect *r = &plan[0];
	r->pos.x = r->pos.y = 0;
	r->sz.width = _width;
	r->sz.height = _height;
	event.setType(EventType::BUTTONCLICK);
	event.setPayload(new std::string(_action));
}
W::Button::~Button()
{
	delete (std::string *)event._payload;
}

void W::Button::setHover() {
	hover = true;
}
W::Colour& W::Button::col() {
	if (hover) { hover = false; return _hovcol; }
	return _col;
}
W::Event* W::Button::getEvent() {
	return &event;
}


W::UIView::UIView(Positioner *_pos, Window *_win, EventHandler *_eh, Dragability _allowDrag) :
	View(_pos, _win),
	eh(_eh), allowDrag(_allowDrag),
	dragloop(false)
{
	
}
W::UIView::~UIView()
{
	// Destroy callbacks
	for (std::vector<Callback*>::iterator it = subscribers.begin(); it != subscribers.end(); it++)
		delete *it;
}

void W::UIView::processMouseEvent(Event *ev) {
	if (dragloop) {
		if (ev->type == EventType::MOUSEMOVE) {
			_positioner->setCorner(pos.x + ev->pos.x - drag_initial.x, pos.y + ev->pos.y - drag_initial.y);
			size s = _window->_getDimensions();
			_updatePosition(&s);
		}
		else if (ev->type == EventType::LEFTMOUSEUP) {
			eh->relinquishPrivilegedEventResponderStatus(this);
			dragloop = false;
		}
		return;
	}
	Button *b = NULL;
	// Get last button that lies under the mouse
	for (std::vector<Button*>::iterator it = buttons.begin(); it < buttons.end(); it++) {
		Button *c = *it;
		if (c->pos.x <= ev->pos.x && c->pos.x + c->plan[0].sz.width > ev->pos.x
			&& c->pos.y <= ev->pos.y && c->pos.y + c->plan[0].sz.height > ev->pos.y)
			b = c;
	}
	if (b == NULL) {
		if (allowDrag && ev->type == EventType::LEFTMOUSEDOWN && eh->requestPrivilegedEventResponderStatus(Callback(&View::receiveEvent, (View*)this))) {
			dragloop = true;
			drag_initial = ev->pos;
		}
		return;
	}
	if (ev->type == EventType::MOUSEMOVE) {
		b->setHover();	// Set button to hovered
	}
	else if (ev->type == EventType::LEFTMOUSEDOWN) {
		// Call callbacks
		for (int i=0; i < subscribers.size(); i++)
			subscribers[i]->call(b->getEvent());
		/*for (std::vector<Callback*>::iterator it = subscribers.begin(); it < subscribers.end(); it++)
			(*it)->call(b->getEvent());
		*/
	}
}

void W::UIView::subscribeToButtons(Callback *_callback) {
	subscribers.push_back(_callback);
}
