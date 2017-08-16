/*
 * W - a simple, cross-platform 2D game develpoment library
 *
 * ===================
 *  MacOSXClasses.m
 * ===================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import "MacOSXClasses.h"
#include "W.h"
#include "Event.h"

/*****************************/
/*** W_View implementation ***/
/*****************************/

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
	W::Event::_addEvent(new W::Event(W::EventType::LEFTMOUSEDOWN, W::position((int)p.x, (int)p.y)));
}
-(void)mouseUp:(NSEvent*)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::Event::_addEvent(new W::Event(W::EventType::LEFTMOUSEUP, W::position((int)p.x, (int)p.y)));
}
-(void)rightMouseDown:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::Event::_addEvent(new W::Event(W::EventType::RIGHTMOUSEDOWN, W::position((int)p.x, (int)p.y)));
}
-(void)rightMouseUp:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::Event::_addEvent(new W::Event(W::EventType::RIGHTMOUSEUP, W::position((int)p.x, (int)p.y)));
}
-(void)keyDown:(NSEvent *)nsev {
	W::Event::_addEvent(new W::Event(
		W::EventType::KEYDOWN,
		W::Event::charToKeycode([[nsev characters] characterAtIndex:0])
	));
}
-(void)keyUp:(NSEvent *)nsev {
	W::Event::_addEvent(new W::Event(
		W::EventType::KEYUP,
		W::Event::charToKeycode([[nsev characters] characterAtIndex:0])
	));
}

-(void)__convertMouseCoords:(NSPoint*)point {
	*point = [self convertPoint:*point toView:self];
	point->y -= 1;										// The y-axis is 1-based, for some reason
	point->y = [self bounds].size.height - point->y;	// Plus it's inverted
}

@end


/***************************************/
/*** W_WindowDelegate implementation ***/
/***************************************/

@implementation W_WindowDelegate

-(bool)windowShouldClose:(id)sender {
	W::Event::_addEvent(new W::Event(W::EventType::CLOSED));
	return NO;
}

@end


/*******************************/
/*** W_Window implementation ***/
/*******************************/

@interface W_Window () {
//	W_WindowDelegate *windowDelegate;
//	W_View *view;
//	NSOpenGLContext *context;
}
@property(nonatomic, strong) W_WindowDelegate *windowDelegate;
@property(nonatomic, strong) W_View *view;
@property(nonatomic, strong) NSOpenGLContext *context;
@end

@implementation W_Window

@synthesize windowDelegate, view, context;

-(id)initWithWidth:(int)w height:(int)h {
	NSRect frame = NSMakeRect(0,0,w,h);
	if (self = [super initWithContentRect:frame
								styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask /*| NSResizableWindowMask*/
								  backing:NSBackingStoreBuffered
									defer:NO]) {
		[self center];
		
		// Create OpenGL context
		NSOpenGLPixelFormat *pf;
		NSOpenGLPixelFormatAttribute attrs[] = {
			NSOpenGLPFAMultisample,
			NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1,
			NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)4,
			NSOpenGLPFADoubleBuffer,
			0
		};
		if ((pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs]) == nil) {
			W::log << "Window: Error creating NSOpenGLPixelFormat\n";
			return nil;
		}
		if ((context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil]) == nil) {
			W::log << "Window: Error creating NSOpenGLContext\n";
			return nil;
		}
		// Turn on VSync
		int vsync = 1;
		[context setValues:&vsync forParameter:NSOpenGLCPSwapInterval];
		
		// Enable lion fullscreenery
		//	NSWindowCollectionBehavior coll = [_objs->window collectionBehavior];
		//	coll |= NSWindowCollectionBehaviorFullScreenPrimary;
		//	[_objs->window setCollectionBehavior:coll];
		
		// Add view to window
		if ((view = [[W_View alloc] initWithFrame:frame]) == nil) {
			W::log << "Window: Error creating W_View\n";
			return nil;
		}
		[self setContentView:view];
		[context setView:view];		// Set view as context’s drawable object
		
		// Set delegate
		if ((windowDelegate = [[W_WindowDelegate alloc] init]) == nil) {
			W::log << "Window: Error creating delegate\n";
			return nil;
		}
		[self setDelegate:windowDelegate];
		
		[self makeKeyAndOrderFront:NSApp];
		[self makeFirstResponder:view];
	}
	return self;
}
-(void) dealloc {
	[context clearDrawable];
}
-(void)setOpenGLThreadAffinity { [context makeCurrentContext]; }
-(void)clearOpenGLThreadAffinity { [NSOpenGLContext clearCurrentContext]; }
-(void)flushBuffer { [context flushBuffer]; }
-(NSPoint)getMousePosition {
	NSPoint p = [self mouseLocationOutsideOfEventStream];
	[view __convertMouseCoords:&p];
	return p;
}

@end


/**********************************/
/*** UpdateTimer implementation ***/
/**********************************/

#include "Callback.h"

@interface W_UpdateTimer () {
	NSTimer *t;
	vdfncb fp;
}
@end

@implementation W_UpdateTimer

-(id)initWithCallback:(vdfncb)_fp {
	if (self = [super init]) {
		fp = _fp;
	}
	return self;
}

-(void)start {
	t = [NSTimer scheduledTimerWithTimeInterval:1./40.
										 target:self
									   selector:@selector(callback:)
									   userInfo:nil
										repeats:YES];
}
-(void)stop {
	[t invalidate];
	t = nil;
}
-(void)callback:(NSTimer*)_t {
	(*fp)();
}

@end
