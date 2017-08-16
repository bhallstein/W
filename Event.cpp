#include "Event.h"
#ifdef __APPLE__
#include <Cocoa/Cocoa.h>
#endif

W::EventType::T W::Event::_typecounter = 100;

W::Event::Event(EventType::T _type) :
	type(_type)
{
	
}
W::Event::Event(EventType::T _type, const position &_pos) :
	type(_type), pos(_pos)
{
	
}
W::Event::Event(EventType::T _type, KeyCode::T _key) :
	type(_type), key(_key)
{
	
}

W::EventType::T W::Event::registerType() {
	return _typecounter++;
}
W::KeyCode::T W::Event::charToKeycode(unsigned int c) {
	if (c >= 'a' && c <= 'z') return (W::KeyCode::T) ((int)W::KeyCode::_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (W::KeyCode::T) ((int)W::KeyCode::_A + c - 'A');
	if (c >= '0' && c <= '9') return (W::KeyCode::T) ((int)W::KeyCode::_0 + c - '0');
	if (c == 27) return W::KeyCode::ESC;		// These are standard ASCII codes
	if (c == 13) return W::KeyCode::RETURN;		//
	if (c == 8)  return W::KeyCode::BACKSPACE;	//
	if (c == 9)  return W::KeyCode::TAB;		//
	#ifdef TARGET_OS_MAC
		if (c == NSLeftArrowFunctionKey)  return W::KeyCode::LEFT_ARROW;
		if (c == NSRightArrowFunctionKey) return W::KeyCode::RIGHT_ARROW;
		if (c == NSUpArrowFunctionKey)    return W::KeyCode::UP_ARROW;
		if (c == NSDownArrowFunctionKey)  return W::KeyCode::DOWN_ARROW;
	#endif
	return W::KeyCode::K_OTHER;
}
