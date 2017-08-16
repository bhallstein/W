#import <Cocoa/Cocoa.h>

@interface W_View : NSView {
	NSOpenGLContext *context;
}
-(void)__convertMouseCoords:(NSPoint*)point;
@end


@interface W_WindowDelegate : NSObject <NSWindowDelegate> {
	
}
-(bool)windowShouldClose:(id)sender;
@end


@interface UpdateTimer : NSObject {
	NSTimer *t;
}
-(void)start;
-(void)stop;
-(void)callback:(NSTimer*)_t;
@end
