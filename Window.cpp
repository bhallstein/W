#include "Window.h"
#include "W.h"
#include "ObjC-Classes.h"
#include <OpenGL/gl.h>

#include <iostream>

struct W::Window_NativeObjs {
	NSWindow *window;
	W_WindowDelegate *windowDelegate;
	W_View *view;
	NSOpenGLPixelFormat *pf;
	NSOpenGLContext *context;
};

W::Window::Window() {
	objs = new Window_NativeObjs();

	// Create OpenGL context
	NSOpenGLPixelFormatAttribute attrs[] = { NSOpenGLPFADoubleBuffer, 0 };
	objs->pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	if (objs->pf == nil)
		throw Exception("Couldn't get an appropriate pixel format");
	objs->context = [[NSOpenGLContext alloc] initWithFormat:objs->pf shareContext:nil];
	if (objs->context == nil) {
		[objs->pf release];
		throw Exception("Couldn't create opengl context");
	}
	
	// Create window
	createWindow();
	
	// Add to W::_windows vector
	W::_windows.push_back(this);
}
W::Window::~Window() {
	// Remove from _windows vector
	for (std::vector<Window*>::iterator it = _windows.begin(); it < _windows.end(); it++)
		if (*it == this) {
			_windows.erase(it);
			break;
		}
}

void W::Window::setTitle(const char *t) {
	[objs->window setTitle:[NSString stringWithUTF8String:t]];
}

void W::Window::_generateMouseMoveEvent() {
	NSPoint p = [objs->window mouseLocationOutsideOfEventStream];
	[objs->view __convertMouseCoords:&p];
	W::Event ev(EventType::MOUSEMOVE, p.x, p.y);
	W::_addEvent(ev);
	
	// Generate screenedge events, useful for scrolling the map
	int scrollmargin = 20;
	size s = _getDimensions();
	int w = s.width, h = s.height;
	if (ev.pos.x < scrollmargin)           W::_addEvent(Event(EventType::SCREENEDGE_LEFT));
	else if (ev.pos.x >= w - scrollmargin) W::_addEvent(Event(EventType::SCREENEDGE_RIGHT));
	if (ev.pos.y < scrollmargin)           W::_addEvent(Event(EventType::SCREENEDGE_TOP));
	else if (ev.pos.y >= h - scrollmargin) W::_addEvent(Event(EventType::SCREENEDGE_BOTTOM));
}

void W::Window::createWindow() {
	NSRect frame = NSMakeRect(0, 0, 800, 600);
	objs->window = [[NSWindow alloc] initWithContentRect:frame
											   styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask /*| NSResizableWindowMask*/
												 backing:NSBackingStoreBuffered
												   defer:NO];
//	NSWindowCollectionBehavior coll = [objs->window collectionBehavior];	// Enable lion fullscreenery
//	coll |= NSWindowCollectionBehaviorFullScreenPrimary;					//
//	[objs->window setCollectionBehavior:coll];								//
	
	[objs->window center];
	
	NSRect viewRect = NSMakeRect(0, 0, frame.size.width, frame.size.height);
	objs->view = [[W_View alloc] initWithFrame:viewRect];
	
	[objs->window setContentView:objs->view];	// Add view to window
	
	[objs->context setView:objs->view];			// Set view as context’s drawable object
	
	objs->windowDelegate = [[W_WindowDelegate alloc] init];	// Create delegate to handle window close
	[objs->window setDelegate:objs->windowDelegate];
	
	[objs->window makeKeyAndOrderFront:NSApp];
	[objs->window makeFirstResponder:objs->view];
	setUpOpenGL();
}
void W::Window::closeWindow() {
	[objs->context clearDrawable];
	[objs->window release];
	[objs->windowDelegate release];
}

void W::Window::setUpOpenGL() {
	[objs->context makeCurrentContext];
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_SCISSOR_TEST);
	}
	[NSOpenGLContext clearCurrentContext];
}

void W::Window::_startDrawing() {
	[objs->context makeCurrentContext];
	
	size s = _getDimensions();
	int w = s.width, h = s.height;
	
	current_drawn_view = NULL;
	glScissor(0, 0, w, h);
	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	
	glClearColor(0.525, 0.187, 0.886, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}
void W::Window::_finishDrawing() {
	[NSOpenGLContext clearCurrentContext];
	[objs->context flushBuffer];
}
void W::Window::_setUpDrawingForView(View *v) {
	current_drawn_view = v;
	int h = [objs->view bounds].size.height;
	size viewsize = v->plan[0].sz;
	glScissor(v->pos.x, h - v->pos.y - viewsize.height, viewsize.width, viewsize.height);
}

void W::Window::_drawRect(float x, float y, float w, float h, const W::Colour &col, float rot) {
	if (current_drawn_view != NULL)
		x += current_drawn_view->pos.x, y += current_drawn_view->pos.y;
	
	glColor4f(col.r, col.g, col.b, col.a);
	
	glLoadIdentity();
	glTranslatef(x + w/2, y + h/2, 0);
	glRotatef(rot, 0, 0, 1);
	glBegin(GL_QUADS);
	{
		glVertex2f(-w/2, -h/2);
		glVertex2f(w/2, -h/2);
		glVertex2f(w/2, h/2);
		glVertex2f(-w/2, h/2);
	}
	glEnd();
}

W::size W::Window::_getDimensions() {
	NSSize bounds = [objs->view bounds].size;
//	CGSize bounds = [objs->view bounds].size;
	return size((int)bounds.width, (int)bounds.height);
}
