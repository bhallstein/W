#include "UIElement.h"
#include "Messenger.h"
#include "Callback.h"


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
	buttonClickEvent(EventType::BUTTONCLICK)
{
	buttonClickEvent._payload = new std::string(name);
	btnrect = new DrawnRect(view, rct.pos, rct.sz, Colour::Black);
	
	Callback cb(&Button::recEv, this);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::MOUSEMOVE], cb, this);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::LEFTMOUSEDOWN], cb, this);
	Messenger::subscribeToPositionalEventType(evTypes[EventType::LEFTMOUSEUP], cb, this);
}
W::Button::~Button()
{
	delete (std::string*)buttonClickEvent._payload;
	delete btnrect;
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::MOUSEMOVE], this);
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::LEFTMOUSEDOWN], this);
	Messenger::unsubscribeFromPositionalEventType(evTypes[EventType::LEFTMOUSEUP], this);
}
void W::Button::recEv(W::Event *ev) {
	using namespace EventType;
	
	if (ev->type == evTypes[LEFTMOUSEDOWN]) {
		active = true;
		Messenger::requestPrivilegedResponderStatusForEventType(evTypes[MOUSEMOVE], Callback(&Button::recEv, this));
		Messenger::requestPrivilegedResponderStatusForEventType(evTypes[LEFTMOUSEUP], Callback(&Button::recEv, this));
	}
	else if (ev->type == evTypes[MOUSEMOVE]) {
		if (active) {
			if (!this->overlapsWith(ev->pos)) {
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
			if (this->overlapsWith(ev->pos))
				Messenger::_dispatchUIEvent(&buttonClickEvent);
		}
	}
}
void W::Button::updatePosition() {
	btnrect->setRect(rct);		// Update D.O.
}
void W::Button::activate() {
	view->addDO(btnrect);
}
void W::Button::deactivate() {
	view->removeDO(btnrect);
}
