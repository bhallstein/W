#include "Window.h"	
#include "W.h"

#ifdef __APPLE__
	#include "ObjC-Classes.h"
	#include <OpenGL/gl.h>
#elif defined WIN32 || WIN64
	#include <gl\gl.h>
	#include <gl\glu.h>
	//#include "shlobj.h"
#endif

#include <iostream>

struct W::Window_NativeObjs {
#ifdef __APPLE__
	NSWindow *window;
	W_WindowDelegate *windowDelegate;
	W_View *view;
	NSOpenGLPixelFormat *pf;
	NSOpenGLContext *context;
#elif defined WIN32 || WIN64
	HWND windowHandle;
	HDC deviceContext;
	HGLRC renderingContext;
#endif
};


#if defined WIN32 || WIN64
	HINSTANCE W::Window::_appInstance;
	LRESULT CALLBACK W::Window_WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
		// If message if WM_NCCREATE, set pointer to MyWindow
		// (Since in CreateWindowEx we set lpParam to 'this', lParam is here a ptr to the window
		if (msg == WM_NCCREATE) {
			LPCREATESTRUCT cs = (LPCREATESTRUCT) lParam;
			SetWindowLongPtr(windowHandle, GWL_USERDATA, (long) cs->lpCreateParams);
		}
		// Otherwise, get window pointer previous set as above
		W::Window *win = (W::Window *) GetWindowLongPtr(windowHandle, GWL_USERDATA);
		return win ? win->_WndProc(windowHandle, msg, wParam, lParam) : DefWindowProc(windowHandle, msg, wParam, lParam);
	}

	struct W::Window::_initializer {
		_initializer() {
			// Set appInstance ref
			_appInstance = GetModuleHandle(NULL);
			if (_appInstance == NULL)
				throw Exception("Couldn't get app instance.");

			// Register window class
			WNDCLASSEX wc;	// Before creating a window, you have to register a class for it

			wc.cbSize = sizeof(wc);
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Force redraw on resize; has own device context
			wc.lpfnWndProc = W::Window_WndProc;
			wc.cbClsExtra = 0;						// No extra class memory 
			wc.cbWndExtra = sizeof(W::Window*);		// Extra window memory for pointer to corresponding Window instance
			wc.hInstance = _appInstance;
			wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = NULL;		// GetStockObject(WHITE_BRUSH) ?
			wc.lpszMenuName = NULL;			// Menu resource name... "MainMenu" ?
			wc.lpszClassName = "DBTWindow";	// Window class name
			wc.hIconSm = (HICON) LoadImage(wc.hInstance, MAKEINTRESOURCE(5), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

			if (!RegisterClassEx(&wc))
				throw Exception("Failed to register window class.");
		}
	};
	struct W::Window::_initializer *W::Window::_init = new W::Window::_initializer();
#endif


W::Window::Window() {
	_objs = new Window_NativeObjs();

#ifdef __APPLE__
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
#elif defined WIN32 || WIN64
	// [ all done in createWindow() ]
#endif
	
	// Create window
	_createWindow();
	
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
#ifdef __APPLE__
	[objs->window setTitle:[NSString stringWithUTF8String:t]];
#elif defined WIN32 || WIN64
	SetWindowText(_objs->windowHandle, t);
#endif
}

void W::Window::_generateMouseMoveEvent() {
	int scrollmargin = 20;
	// TODO: check if mouse is within window bounds (plus some margin, for ease of scroll?)
#ifdef __APPLE__
	NSPoint p = [objs->window mouseLocationOutsideOfEventStream];
	[objs->view __convertMouseCoords:&p];
	W::Event ev(EventType::MOUSEMOVE, p.x, p.y);
	W::_addEvent(ev);
#elif defined WIN32 || WIN64
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(_objs->windowHandle, &p);
	W::Event ev(EventType::MOUSEMOVE, p.x, p.y);
	W::_addEvent(ev);
#endif
	// Generate screenedge events, useful for scrolling the map
	size s = _getDimensions();
	int w = s.width, h = s.height;
	if (ev.pos.x < scrollmargin)           W::_addEvent(Event(EventType::SCREENEDGE_LEFT));
	else if (ev.pos.x >= w - scrollmargin) W::_addEvent(Event(EventType::SCREENEDGE_RIGHT));
	if (ev.pos.y < scrollmargin)           W::_addEvent(Event(EventType::SCREENEDGE_TOP));
	else if (ev.pos.y >= h - scrollmargin) W::_addEvent(Event(EventType::SCREENEDGE_BOTTOM));
}

void W::Window::_createWindow() {
#ifdef __APPLE__
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

#elif defined WIN32 || WIN64
	// Set window style & size
	DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	
	int width = 800, height = 600;

	RECT rect;
	rect.left = 0, rect.right = width;
	rect.top = 0, rect.bottom = height;
	AdjustWindowRectEx(&rect, windowStyle, FALSE, extendedWindowStyle);
	
	// Create window
	_objs->windowHandle = CreateWindowEx(
		extendedWindowStyle,				//
		"DBTWindow",
		"Oh hai",		// title
		windowStyle,
		0, 0,			// position
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,			// parent window
		NULL,			// menu
		_appInstance,
		this
	);
	if(_objs->windowHandle == NULL) {
		DWORD err = GetLastError();
		char errorMsg[200];
		wsprintf(errorMsg, "Error creating window. Error code: %d, %X.", err, err);
		_closeWindow();
		throw Exception(errorMsg);
	}
	// Get device context
	if (!(_objs->deviceContext = GetDC(_objs->windowHandle))) {
		_closeWindow();
		throw Exception("Error creating an OpenGL device context");
	}
	// Create pixel format
	int pf;
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,						// Version number
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Support windows, opengl, and double buffering
		PFD_TYPE_RGBA,
		16,						// Colour bit depth
		0, 0, 0, 0, 0, 0,		// Color bits (ignored)
		0,						// Alpha buffer
		0,						// Shift bit
		0,						// Accumulation buffer
		0, 0, 0, 0,				// Accumulation bits (ignored)
		16,						// 16 bit depth buffer
		0,						// Stencil buffer
		0,						// Auxiliary buffer
		PFD_MAIN_PLANE,			// Main drawing layer
		0,						// Reserved
		0, 0, 0					// Layer Masks Ignored
	};
	if (!(pf = ChoosePixelFormat(_objs->deviceContext, &pfd))) {
		_closeWindow();
		throw Exception("Unable to get a suitable pixel format");
	}
	if(!SetPixelFormat(_objs->deviceContext, pf, &pfd)) {
		_closeWindow();
		throw Exception("Unable to set the pixel format");
	}
	// Create rendering context
	if (!(_objs->renderingContext = wglCreateContext(_objs->deviceContext))) {
		_closeWindow();
		throw Exception("Error creating a rendering context");
	}
	// Make rendering context current
	if (!wglMakeCurrent(_objs->deviceContext, _objs->renderingContext)) {
		_closeWindow();
		throw Exception("Error activating the rendering context");
	}
	
	ShowWindow(_objs->windowHandle, SW_SHOW);
	SetForegroundWindow(_objs->windowHandle);
	SetFocus(_objs->windowHandle);
#endif
}
void W::Window::_closeWindow() {
#ifdef __APPLE__
	[objs->context clearDrawable];
	[objs->window release];
	[objs->windowDelegate release];
#elif defined WIN32 || WIN64
	if (_objs->renderingContext) {
		if (!wglMakeCurrent(NULL, NULL))
			MessageBox(NULL, "Error releasing device and rendering contexts", "Error", MB_OK | MB_ICONEXCLAMATION);
		if (!wglDeleteContext(_objs->renderingContext))
			MessageBox(NULL, "Error deleting the rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);
		_objs->renderingContext = NULL;
	}
	if (_objs->deviceContext && !ReleaseDC(_objs->windowHandle, _objs->deviceContext))
		MessageBox(NULL, "Error releasing the device context", "Error", MB_OK | MB_ICONEXCLAMATION);
	_objs->deviceContext = NULL;
	if (_objs->windowHandle && !DestroyWindow(_objs->windowHandle))
		MessageBox(NULL, "Error destroying the window", "Error", MB_OK | MB_ICONEXCLAMATION);
	_objs->windowHandle = NULL;
	if (!UnregisterClass("OpenGL", _appInstance))
		MessageBox(NULL, "Error unregistering the window class", "Error", MB_OK | MB_ICONEXCLAMATION);
	_appInstance = NULL;
#endif
}

void W::Window::_setUpOpenGL() {
#ifdef __APPLE__
	[objs->context makeCurrentContext];
#endif
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_SCISSOR_TEST);
	}
#ifdef __APPLE__
	[NSOpenGLContext clearCurrentContext];
#endif
}

void W::Window::_startDrawing() {
#ifdef __APPLE__
	[objs->context makeCurrentContext];
#endif
	
	size s = _getDimensions();
	int w = s.width, h = s.height;
	
	_current_drawn_view = NULL;
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
#ifdef __APPLE__
	[NSOpenGLContext clearCurrentContext];
	[objs->context flushBuffer];
#elif defined WIN32 || WIN64
	SwapBuffers(_objs->deviceContext);
#endif
}
void W::Window::_setUpDrawingForView(View *v) {
	_current_drawn_view = v;
	size winsize = _getDimensions();
	int h = winsize.height;
	size viewsize = v->plan[0].sz;
	glScissor(v->pos.x, h - v->pos.y - viewsize.height, viewsize.width, viewsize.height);
}

void W::Window::_drawRect(float x, float y, float w, float h, const W::Colour &col, float rot) {
	if (_current_drawn_view != NULL)
		x += _current_drawn_view->pos.x, y += _current_drawn_view->pos.y;
	
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
#ifdef __APPLE__
	NSSize bounds = [objs->view bounds].size;
//	CGSize bounds = [objs->view bounds].size;
	return size((int)bounds.width, (int)bounds.height);
#elif defined WIN32 || WIN64
	RECT rect;
	GetClientRect(_objs->windowHandle, &rect);
	return size(rect.right - rect.left, rect.bottom - rect.top);
#endif
}

#if defined WIN32 || WIN64
void W::Window::_convertToWEvent(W::Event &ev, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_LBUTTONUP) {
		ev.type = W::EventType::LEFTMOUSEUP;
		ev.pos.x = LOWORD(lParam);
		ev.pos.y = HIWORD(lParam);
	}
	else if (msg == WM_LBUTTONDOWN) {
		ev.type = W::EventType::LEFTMOUSEDOWN;
		ev.pos.x = LOWORD(lParam);
		ev.pos.y = HIWORD(lParam);
	}
	else if (msg == WM_RBUTTONUP) {
		ev.type = W::EventType::RIGHTMOUSEUP;
		ev.pos.x = LOWORD(lParam);
		ev.pos.y = HIWORD(lParam);
	}
	else if (msg == WM_CHAR) {
		ev.type = W::EventType::KEYPRESS;
		ev.key = W::Event::charToKeycode(wParam);
	}
	else if (msg == WM_KEYDOWN) {
		ev.type = EventType::KEYPRESS;
		if (wParam == VK_LEFT)			ev.key = KeyCode::K_LEFT_ARROW;
		else if (wParam == VK_RIGHT)	ev.key = KeyCode::K_RIGHT_ARROW;
		else if (wParam == VK_UP)		ev.key = KeyCode::K_UP_ARROW;
		else if (wParam == VK_DOWN)		ev.key = KeyCode::K_DOWN_ARROW;
		else if (wParam == VK_HOME)		ev.key = KeyCode::K_HOME;
		else if (wParam == VK_END)		ev.key = KeyCode::K_END;
		else if (wParam == VK_DELETE)	ev.key = KeyCode::K_DELETE;
		else ev.key = KeyCode::K_OTHER;
	}
	else if (msg == WM_CLOSE)
		ev.type = EventType::CLOSED;
	else
		ev.type = EventType::UNKNOWN;
}
LRESULT CALLBACK W::Window::_WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_CHAR || msg == WM_KEYDOWN || msg ==  WM_CLOSE) {
		Event ev;
		_convertToWEvent(ev, msg, wParam, lParam);
		W::_addEvent(ev);
		return 0;
	}
	else if (msg == WM_KEYUP) {
		return 0;
	}
	else if (msg == WM_GETMINMAXINFO) {
		MINMAXINFO *minmaxinfo = (MINMAXINFO*) lParam;
		minmaxinfo->ptMaxTrackSize.x = minmaxinfo->ptMinTrackSize.x = minmaxinfo->ptMaxSize.x = 816;	// ptMin/MaxTrackSize used while resizing, ptMaxSize used while maximizing
		minmaxinfo->ptMaxTrackSize.y = minmaxinfo->ptMinTrackSize.y = minmaxinfo->ptMaxSize.y = 638;
	}
	else if (msg == WM_SIZE) {	// Sent after resize
//		dealWithResize(NULL);
		return 0;
	}
	else if (msg == WM_ACTIVATE) {
		bool active = HIWORD(wParam) ? false : true;	// May want to un/pause the game loop
		return 0;
	}
	if (msg == WM_SYSCOMMAND && (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER))
		return 0;		// Prevent these from happening

	return DefWindowProc(windowHandle, msg, wParam, lParam);
}

#endif
