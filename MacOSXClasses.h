/*
 * W - a tiny 2D game development library
 *
 * ===================
 *  MacOSXClasses.h
 * ===================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import <Cocoa/Cocoa.h>

@interface W_View : NSView {
	NSOpenGLContext *context;
}
-(void)__convertMouseCoords:(NSPoint*)point;
@end


@interface W_WindowDelegate : NSObject <NSWindowDelegate>
-(bool)windowShouldClose:(id)sender;
@end


@interface W_Window : NSWindow
-(id)initWithWidth:(int)w height:(int)h;
-(void)setOpenGLThreadAffinity;
-(void)clearOpenGLThreadAffinity;
-(void)flushBuffer;
-(NSPoint)getMousePosition;
@end;


typedef void (*vdfncb)(void);

@interface W_UpdateTimer : NSObject
-(id)initWithCallback:(vdfncb)fp;
-(void)start;
-(void)stop;
-(void)callback:(NSTimer*)_t;
@end
