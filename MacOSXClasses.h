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


@interface W_UpdateTimer : NSObject
-(id)initWithCallback:(void*)cb;
-(void)start;
-(void)stop;
-(void)callback:(NSTimer*)_t;
@end
