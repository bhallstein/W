/*
 * W - a tiny 2D game develpoment library
 *
 * ================
 *  iOSClasses.m
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import "iOSClasses.h"
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "Event.h"
#include <iostream>

/*******************************/
/*** EAGLView implementation ***/
/*******************************/

@interface EAGLView () {
	GLint backingWidth;
	GLint backingHeight;	
	GLuint framebuffer, renderbuffer;
}
@end

@implementation EAGLView

@synthesize context;

+(Class)layerClass {
    return [CAEAGLLayer class];
}
-(id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
		/* Set up OpenGL ES 1.1 */
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
		self.contentScaleFactor = [UIScreen mainScreen].scale;
		self.multipleTouchEnabled = YES;
		
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        if (!context || ![EAGLContext setCurrentContext:context])
            return nil;
		
		// Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
		glGenFramebuffersOES(1, &framebuffer);
		glGenRenderbuffersOES(1, &renderbuffer);
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderbuffer);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, renderbuffer);
		
		/* Add gesture recognizers */
//		UITapGestureRecognizer *r = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(respondToTapGesture:)];
//		r.numberOfTapsRequired = 1;
//		[self addGestureRecognizer:r];
    }
    return self;
}
-(void)setOpenGLThreadAffinity {
	[EAGLContext setCurrentContext:context];
}
-(void)clearOpenGLThreadAffinity {
	// No method for this seems to exist on iOS
}
-(void)flushBuffer {
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}
-(void)bindFramebuffer {
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
}
-(void)layoutSubviews {
	// Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return;
    }
}

//-(IBAction)respondToTapGesture:(UITapGestureRecognizer *)recognizer {
//	CGPoint p = [recognizer locationInView:self];
//	p.x *= self.contentScaleFactor, p.y *= self.contentScaleFactor;
//	W::Event::_addEvent(new W::Event(W::EventType::TAP, W::position((int)p.x, (int)p.y)));
//}

inline W::position touchLocInView(UITouch *t, UIView *v) {
	CGPoint p = [t locationInView:v];
	p.x *= v.contentScaleFactor, p.y *= v.contentScaleFactor;
	return W::position((int)p.x, (int)p.y);
}
inline W::position prevTouchLocInView(UITouch *t, UIView *v) {
	CGPoint p = [t previousLocationInView:v];
	p.x *= v.contentScaleFactor, p.y *= v.contentScaleFactor;
	return W::position((int)p.x, (int)p.y);
}
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *t in touches)
		W::Event::_addEvent(new W::Event(W::EventType::TouchDown, (int)t, touchLocInView(t, self)));
}
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *t in touches) {
		W::position p = touchLocInView(t, self);
		W::position pp = prevTouchLocInView(t, self);
		W::Event::_addEvent(new W::Event(W::EventType::TouchMoved, (int)t, p, pp));
	}
}
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *t in touches)
		W::Event::_addEvent(new W::Event(W::EventType::TouchUp, (int)t, touchLocInView(t, self)));
}
-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *t in touches)
		W::Event::_addEvent(new W::Event(W::EventType::TouchCancelled, (int)t, touchLocInView(t, self)));
}

-(void)dealloc {
	// Tear down GL
	if (framebuffer) {
		glDeleteFramebuffersOES(1, &framebuffer);
		framebuffer = 0;
	}
	if (renderbuffer) {
		glDeleteRenderbuffersOES(1, &renderbuffer);
		renderbuffer = 0;
	}
	
	// Tear down context
	if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	context = nil;
}

@end


/**************************************/
/*** XViewController implementation ***/
/**************************************/

@interface W_ViewController ()
@end

@implementation W_ViewController

@synthesize v;

-(void)loadView {
	CGRect applicationFrame = [[UIScreen mainScreen] applicationFrame];
	v = [[EAGLView alloc] initWithFrame:applicationFrame];
	self.view = v;
}
-(void)viewDidLoad {
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}
-(void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end


/**********************************/
/*** UpdateTimer implementation ***/
/**********************************/

@interface W_UpdateTimer () {
	CADisplayLink *displayLink;	// NB: iOS 3.1 or higher req'd to use displaylink
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
	displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(callback:)];
	displayLink.frameInterval = 1;
	[displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
}
-(void)stop {
	[displayLink invalidate];
}
-(void)callback:(id)sender {
	(*fp)();
}

@end
