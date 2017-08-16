#include "UIElement.h"
#include "Messenger.h"
#include "Callback.h"


W::UIElement::UIElement(const char *_name, evTypeMap &_evTypes) :
	name(_name), evTypes(_evTypes)
{
	// 
}
W::UIElement::~UIElement()
{
	// 
}


W::Button::Button(const char *_name, evTypeMap &_evTypes) :
	UIElement(_name, _evTypes),
	hover(false), active(false)
{
	buttonClickEvent.setType(EventType::BUTTONCLICK);
	buttonClickEvent.setPayload(new std::string(name));

	Callback cb(&Button::recEv, this);
	Messenger::subscribeToEventType(evTypes[EventType::MOUSEMOVE], cb);
	Messenger::subscribeToEventType(evTypes[EventType::LEFTMOUSEDOWN], cb);
	Messenger::subscribeToEventType(evTypes[EventType::LEFTMOUSEUP], cb);
}
W::Button::~Button()
{
	delete (std::string*)buttonClickEvent._payload;
	Messenger::unsubscribeFromEventType(evTypes[EventType::MOUSEMOVE], this);
	Messenger::unsubscribeFromEventType(evTypes[EventType::LEFTMOUSEDOWN], this);
	Messenger::unsubscribeFromEventType(evTypes[EventType::LEFTMOUSEUP], this);
}
void W::Button::recEv(W::Event *ev) {
	if (ev->type == EventType::LEFTMOUSEDOWN) {
		active = true;
		Messenger::requestPrivilegedEventResponderStatus(Callback(&Button::recEv, this));
	}
	else if (ev->type == EventType::MOUSEMOVE) {
		hover = true;
		if (!this->overlapsWith(ev->pos)) {
			hover = false;
			Messenger::relinquishPrivilegedEventResponderStatus(this);
		}
	}
	else if (ev->type == EventType::LEFTMOUSEUP) {
		active = false;
		Messenger::relinquishPrivilegedEventResponderStatus(this);
		if (this->overlapsWith(ev->pos))
			Messenger::_dispatchUIEvent(&buttonClickEvent);
	}
}
