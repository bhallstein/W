/*
 * W - a simple, cross-platform 2D game develpoment library
 *
 * ================
 *  iOSClasses.h
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@class XViewController;

@interface EAGLView : UIView
@property (strong, nonatomic) EAGLContext *context;
-(id)initWithFrame:(CGRect)frame;
-(void)setOpenGLThreadAffinity;
-(void)clearOpenGLThreadAffinity;
-(void)flushBuffer;		// Draw
-(void)bindFramebuffer;	// Call before beginning to issue opengl commands
@end


@interface W_ViewController : UIViewController
@property (strong, nonatomic) EAGLView *v;
@end


typedef void (*vdfncb)(void);

@interface W_UpdateTimer : NSObject
-(id)initWithCallback:(vdfncb)fp;
-(void)start;
-(void)stop;
-(void)callback:(id)sender;
@end
