#include "Event.h"
#ifdef __APPLE__
#include <Cocoa/Cocoa.h>
#endif

W::EventType::T W::Event::_typeid = 100;

W::KeyCode::T W::Event::charToKeycode(unsigned int c) {
	if (c >= 'a' && c <= 'z') return (W::KeyCode::T) ((int)W::KeyCode::K_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (W::KeyCode::T) ((int)W::KeyCode::K_A + c - 'A');
	if (c >= '0' && c <= '9') return (W::KeyCode::T) ((int)W::KeyCode::K_0 + c - '0');
	if (c == 27) return W::KeyCode::K_ESC;			// These are standard ASCII codes
	if (c == 13) return W::KeyCode::K_RETURN;		//
	if (c == 8)  return W::KeyCode::K_BACKSPACE;	//
	if (c == 9)  return W::KeyCode::K_TAB;			//
#ifdef TARGET_OS_MAC
	if (c == NSLeftArrowFunctionKey)	return W::KeyCode::K_LEFT_ARROW;
	if (c == NSRightArrowFunctionKey)	return W::KeyCode::K_RIGHT_ARROW;
	if (c == NSUpArrowFunctionKey)		return W::KeyCode::K_UP_ARROW;
	if (c == NSDownArrowFunctionKey)	return W::KeyCode::K_DOWN_ARROW;
#endif
	return W::KeyCode::K_OTHER;
}
