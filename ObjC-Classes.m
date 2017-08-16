#import "ObjC-Classes.h"
#include "W.h"
#include "Event.h"


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
		W::Event::charToKeycode([[nsev characters] characterAtIndex:0])
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
