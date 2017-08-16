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


@interface W_Demon : NSObject {
	NSTimer *gametimer;
}
-(void)gametimerStart;
-(void)gametimerStop;
-(void)gametimerFire:(NSTimer*)t;
@end
