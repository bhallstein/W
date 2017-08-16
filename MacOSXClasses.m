/*
 * W - a tiny 2D game development library
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

#pragma mark W_View

@implementation W_View

-(id)initWithFrame:(NSRect)frame {
    return [super initWithFrame:frame];
}

-(BOOL)acceptsFirstResponder {
	return YES;
}

-(void)mouseDown:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::Event::_addEvent(new W::Event(W::EventType::LMouseDown, W::v2i((int)p.x, (int)p.y)));
}
-(void)mouseUp:(NSEvent*)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::Event::_addEvent(new W::Event(W::EventType::LMouseUp, W::v2i((int)p.x, (int)p.y)));
}
-(void)rightMouseDown:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::Event::_addEvent(new W::Event(W::EventType::RMouseDown, W::v2i((int)p.x, (int)p.y)));
}
-(void)rightMouseUp:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self __convertMouseCoords:&p];
	W::Event::_addEvent(new W::Event(W::EventType::RMouseUp, W::v2i((int)p.x, (int)p.y)));
}
-(void)keyDown:(NSEvent *)nsev {
	W::Event::_addEvent(new W::Event(
		W::EventType::KeyDown,
		W::Event::charToKeycode([[nsev characters] characterAtIndex:0])
	));
}
-(void)keyUp:(NSEvent *)nsev {
	W::Event::_addEvent(new W::Event(
		W::EventType::KeyUp,
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

#pragma mark - W_WindowDelegate

@implementation W_WindowDelegate

-(bool)windowShouldClose:(id)sender {
	W::Event::_addEvent(new W::Event(W::EventType::Closed));
	return NO;
}

@end


/*******************************/
/*** W_Window implementation ***/
/*******************************/

#pragma mark - W_Window

@interface W_Window ()
@property(nonatomic, strong) W_WindowDelegate *windowDelegate;
@property(nonatomic, strong) W_View *view;
@property(nonatomic, strong) NSOpenGLContext *context;
-(void)frameChanged:(id)notification;
@end

@implementation W_Window

@synthesize windowDelegate, view, context, window;

#define W_MULTISAMPLE

-(id)initWithWidth:(int)w height:(int)h {
	NSRect frame = NSMakeRect(0,0,w,h);
	if (self = [super initWithContentRect:frame
								styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
								  backing:NSBackingStoreBuffered
									defer:NO]) {
		[self center];
		
		// Create OpenGL context
		NSOpenGLPixelFormat *pf;
		NSOpenGLPixelFormatAttribute attrs[] = {
			#ifdef W_MULTISAMPLE
				NSOpenGLPFAMultisample,
				NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1,
				NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)4,
			#endif
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
		
		// Add view to window
		if ((view = [[W_View alloc] initWithFrame:frame]) == nil) {
			W::log << "Window: Error creating W_View\n";
			return nil;
		}
		[self setContentView:view];
		
		// Set view as context's drawable object
		[context setView:view];
		
		// Set delegate
		if ((windowDelegate = [[W_WindowDelegate alloc] init]) == nil) {
			W::log << "Window: Error creating delegate\n";
			return nil;
		}
		[self setDelegate:windowDelegate];
		
		// Make frontmost
		[self makeKeyAndOrderFront:NSApp];
		[self makeFirstResponder:view];
		
		// Subscribe to view frame changes (which happen automatically on resize)
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(frameChanged:)
													 name:NSViewGlobalFrameDidChangeNotification
												   object:view];
	}
	return self;
}
-(void) dealloc {
	[context clearDrawable];
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:NSViewGlobalFrameDidChangeNotification
												  object:view];
}
-(void)setOpenGLThreadAffinity { [context makeCurrentContext]; }
-(void)clearOpenGLThreadAffinity { [NSOpenGLContext clearCurrentContext]; }
-(void)flushBuffer { [context flushBuffer]; }
-(NSPoint)getMousePosition {
	NSPoint p = [self mouseLocationOutsideOfEventStream];
	[view __convertMouseCoords:&p];
	return p;
}
-(void)frameChanged:(id)notification {
	[self.context update];

	W::v2i sz(view.bounds.size.width, view.bounds.size.height);
	
	// Resize the backing buffer
	GLint size[] = { sz.a, sz.b };
	CGLContextObj ctx = (CGLContextObj) [context CGLContextObj];
	CGLSetParameter(ctx, kCGLCPSurfaceBackingSize, size);
	CGLEnable(ctx, kCGLCESurfaceBackingSize);
	
	// Notify W::Window of the change
	W::Window *win = (W::Window*) window;
	if (win)
		win->updateSize(sz);
}
@end


/**********************************/
/*** UpdateTimer implementation ***/
/**********************************/

#pragma mark - W_UpdateTimer

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
