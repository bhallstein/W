#import "ObjC-Classes.h"
#include "W.h"

void __setEventPositionAndType(W::Event *ev, int x, int y, W::EventType::T t) {
	ev->type = t;
	ev->pos.x = x;
	ev->pos.y = y;
}
W::KeyCode::T __charToKeycode(unsigned int c) {
	if (c >= 'a' && c <= 'z') return (W::KeyCode::T) ((int)W::KeyCode::K_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (W::KeyCode::T) ((int)W::KeyCode::K_A + c - 'A');
	if (c >= '0' && c <= '9') return (W::KeyCode::T) ((int)W::KeyCode::K_0 + c - '0');
	if (c == 27) return W::KeyCode::K_ESC;		// These are standard ASCII codes
	if (c == 13) return W::KeyCode::K_RETURN;		//
	if (c == 8)  return W::KeyCode::K_BACKSPACE;	//
	if (c == 9)  return W::KeyCode::K_TAB;		//
	if (c == NSLeftArrowFunctionKey)	return W::KeyCode::K_LEFT_ARROW;
	if (c == NSRightArrowFunctionKey)	return W::KeyCode::K_RIGHT_ARROW;
	if (c == NSUpArrowFunctionKey)		return W::KeyCode::K_UP_ARROW;
	if (c == NSDownArrowFunctionKey)	return W::KeyCode::K_DOWN_ARROW;
	return W::KeyCode::K_OTHER;
}


@implementation W_View

-(id)initWithFrame:(NSRect)frame {
    return [super initWithFrame:frame];
}

-(BOOL)acceptsFirstResponder {
	return YES;
}
-(bool)windowShouldClose:(id)sender {
	return NO;
}

-(void)mouseDown:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::_addEvent(W::Event(W::EventType::LEFTMOUSEDOWN, p.x, p.y));
}
-(void)mouseUp:(NSEvent*)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::_addEvent(W::Event(W::EventType::LEFTMOUSEUP, p.x, p.y));
}
-(void)rightMouseDown:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::_addEvent(W::Event(W::EventType::RIGHTMOUSEDOWN, p.x, p.y));
}
-(void)rightMouseUp:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::_addEvent(W::Event(W::EventType::RIGHTMOUSEUP, p.x, p.y));
}
-(void)keyDown:(NSEvent *)nsev {
	W::_addEvent(W::Event(
		W::EventType::KEYPRESS,
		__charToKeycode([[nsev characters] characterAtIndex:0])
	));
}

-(void)__convertMouseCoords:(NSPoint*)point {
	*point = [self convertPoint:*point toView:self];
	point->y -= 1;										// The y-axis is 1-based, for some reason
	point->y = [self bounds].size.height - point->y;	// Plus it's inverted
}

@end


@implementation W_WindowDelegate

-(bool)windowShouldClose:(id)sender {
	W::_addEvent(W::Event(W::EventType::CLOSED));
	return NO;
}

@end


@implementation W_Demon

-(void)gametimerStart {
	gametimer = [NSTimer scheduledTimerWithTimeInterval:1./40.
												 target:self
											   selector:@selector(gametimerFire:)
											   userInfo:nil
												repeats:YES];
}
-(void)gametimerStop {
	[gametimer invalidate];
}
-(void)gametimerFire:(NSTimer*)t {
	W::_runGameLoop();
}

@end
