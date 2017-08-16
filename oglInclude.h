#ifndef W__oglInclude
#define W__oglInclude

#if defined WTARGET_MAC
	#import <OpenGL/gl.h>
#elif defined WTARGET_IOS
	#import <OpenGLES/ES1/gl.h>
	#import <OpenGLES/ES1/glext.h>
#elif defined WTARGET_WIN
	#include <gl\gl.h>
	#include <gl\glu.h>
#endif

#endif
