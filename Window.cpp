#include "Window.h"	
#include "W.h"

#ifdef __APPLE__
	#include "MacOSXClasses.h"
	#include <OpenGL/gl.h>
#elif defined _WIN32 || _WIN64
	#include <gl\gl.h>
	#include <gl\glu.h>
	//#include "shlobj.h"
#endif

#include "texture.h"

#include <iostream>

struct W::Window::NativeObjs {
	#ifdef __APPLE__
		NSWindow *window;
		W_WindowDelegate *windowDelegate;
		W_View *view;
		NSOpenGLPixelFormat *pf;
		NSOpenGLContext *context;
	#elif defined _WIN32 || _WIN64
		HWND windowHandle;
		HDC deviceContext;
		HGLRC renderingContext;
	#endif
};


#if defined _WIN32 || _WIN64
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

	std::map<UINT, W::EventType::T> W::Window::_win_event_type_map;

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
			wc.lpszClassName = "W_Window";	// Window class name
			wc.hIconSm = (HICON) LoadImage(wc.hInstance, MAKEINTRESOURCE(5), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

			if (!RegisterClassEx(&wc))
				throw Exception("Failed to register window class.");

			// Set translation map for windows events to W events
			_win_event_type_map[WM_LBUTTONDOWN] = W::EventType::LEFTMOUSEDOWN;
			_win_event_type_map[WM_RBUTTONDOWN] = W::EventType::RIGHTMOUSEDOWN;
			_win_event_type_map[WM_LBUTTONUP]   = W::EventType::LEFTMOUSEUP;
			_win_event_type_map[WM_RBUTTONUP]   = W::EventType::RIGHTMOUSEUP;
		}
	};
	struct W::Window::_initializer *W::Window::_init = new W::Window::_initializer();
#endif


W::Window::Window(const size &_size, const char *_title)
{
	// Check window has not already been created
	if (W::_window) {
		std::stringstream ss;
		ss << "Cannot create window: window has already been created (" << _window << ")";
		throw Exception(ss.str());
	}
	
	_objs = new NativeObjs();
	
	// Create window
	createWindow(_size, _title);
}
W::Window::~Window()
{
	closeWindow();
	delete _objs;
}

void W::Window::setTitle(const std::string &t) {
	setTitle(t.c_str());
}
void W::Window::setTitle(const char *t) {
	#ifdef __APPLE__
		[_objs->window setTitle:[NSString stringWithUTF8String:t]];
	#elif defined _WIN32 || _WIN64
		SetWindowText(_objs->windowHandle, t);
	#endif
}

void W::Window::generateMouseMoveEvent() {
	int scrollmargin = 20;
	// TODO: check if mouse is within window bounds (plus some margin, for ease of scroll?)
	#ifdef __APPLE__
		NSPoint p = [_objs->window mouseLocationOutsideOfEventStream];
		[_objs->view __convertMouseCoords:&p];
		W::Event ev(EventType::MOUSEMOVE, W::position((int)p.x, (int)p.y));
		W::_addEvent(ev);
	#elif defined _WIN32 || _WIN64
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(_objs->windowHandle, &p);
		position _pos((int)p.x, (int)p.y);
		W::Event ev(EventType::MOUSEMOVE, _pos);
		W::_addEvent(ev);
	#endif
	// Generate screenedge events, useful for scrolling the map
	size s = getDimensions();
	int w = s.width, h = s.height;
	if (ev.pos.x < scrollmargin)           W::_addEvent(Event(EventType::SCREENEDGE_LEFT));
	else if (ev.pos.x >= w - scrollmargin) W::_addEvent(Event(EventType::SCREENEDGE_RIGHT));
	if (ev.pos.y < scrollmargin)           W::_addEvent(Event(EventType::SCREENEDGE_TOP));
	else if (ev.pos.y >= h - scrollmargin) W::_addEvent(Event(EventType::SCREENEDGE_BOTTOM));
}

void W::Window::createWindow(const size &_size, const char *_title) {
	#ifdef __APPLE__
		// Create OpenGL context
		NSOpenGLPixelFormatAttribute attrs[] = {
			NSOpenGLPFAMultisample,
			NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1,
			NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)4,
			NSOpenGLPFADoubleBuffer,
			0
		};
		_objs->pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
		if (_objs->pf == nil)
			throw Exception("Couldn't get an appropriate pixel format");
		_objs->context = [[NSOpenGLContext alloc] initWithFormat:_objs->pf shareContext:nil];
		if (_objs->context == nil) {
			[_objs->pf release];
			throw Exception("Couldn't create opengl context");
		}
		// Turn on VSync
		int vsync = 1;
		[_objs->context setValues:&vsync forParameter:NSOpenGLCPSwapInterval];
		
		// Create Window
		NSRect frame = NSMakeRect(0, 0, _size.width, _size.height);
		_objs->window = [[NSWindow alloc] initWithContentRect:frame
												   styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask /*| NSResizableWindowMask*/
													 backing:NSBackingStoreBuffered
													   defer:NO];
	//	NSWindowCollectionBehavior coll = [_objs->window collectionBehavior];	// Enable lion fullscreenery
	//	coll |= NSWindowCollectionBehaviorFullScreenPrimary;					//
	//	[_objs->window setCollectionBehavior:coll];								//
		
		[_objs->window center];
		
		// Add view to window
		NSRect viewRect = NSMakeRect(0, 0, frame.size.width, frame.size.height);
		_objs->view = [[W_View alloc] initWithFrame:viewRect];
		[_objs->window setContentView:_objs->view];
		[_objs->context setView:_objs->view];		// Set view as context’s drawable object
		
		// Create window delegate
		_objs->windowDelegate = [[W_WindowDelegate alloc] init];
		[_objs->window setDelegate:_objs->windowDelegate];
		
		[_objs->window makeKeyAndOrderFront:NSApp];
		[_objs->window makeFirstResponder:_objs->view];
		
	#elif defined _WIN32 || _WIN64
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
			extendedWindowStyle,
			"W_Window",				// window class
			"My W Application",		// window title
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
			closeWindow();
			throw Exception(errorMsg);
		}
		// Get device context
		if (!(_objs->deviceContext = GetDC(_objs->windowHandle))) {
			closeWindow();
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
			closeWindow();
			throw Exception("Unable to get a suitable pixel format");
		}
		if(!SetPixelFormat(_objs->deviceContext, pf, &pfd)) {
			closeWindow();
			throw Exception("Unable to set the pixel format");
		}
		// Create rendering context
		if (!(_objs->renderingContext = wglCreateContext(_objs->deviceContext))) {
			closeWindow();
			throw Exception("Error creating a rendering context");
		}
		
		ShowWindow(_objs->windowHandle, SW_SHOW);
		SetForegroundWindow(_objs->windowHandle);
		SetFocus(_objs->windowHandle);
	#endif
	
	setTitle(_title);
	setUpOpenGL();
}
void W::Window::closeWindow() {
	#ifdef __APPLE__
		[_objs->context clearDrawable];
		[_objs->window release];
		[_objs->windowDelegate release];
	#elif defined _WIN32 || _WIN64
		if (_objs->renderingContext) {
			// The window is deleted from the quit() function, after the draw thread has released the context and exited.
			// Therefore we don't need to unmake the context current before exiting - in general, this must be done before 
			// the window is deleted.
			// NB: if there's a problem during window creation, may need to call clearOpenGLThreadAffinity() before closeWindow().
			//if (!wglMakeCurrent(NULL, NULL))
			//	MessageBox(NULL, "Error releasing device and rendering contexts", "Error", MB_OK | MB_ICONEXCLAMATION);
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
		if (!UnregisterClass("W_Window", _appInstance))
			MessageBox(NULL, "Error unregistering the window class", "Error", MB_OK | MB_ICONEXCLAMATION);
		_appInstance = NULL;
	#endif
}

void W::Window::setUpOpenGL() {
	setOpenGLThreadAffinity();
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_SCISSOR_TEST);
	glEnable(GL_TEXTURE_2D);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	clearOpenGLThreadAffinity();
}

void W::Window::setOpenGLThreadAffinity() {
	// Make opengl context current on the calling thread
	#ifdef __APPLE__
		[_objs->context makeCurrentContext];
	#elif defined _WIN32 || _WIN64
		if (!wglMakeCurrent(_objs->deviceContext, _objs->renderingContext)) {
			closeWindow();
			throw Exception("Error setting the OpenGL context thread affinity");
		}
	#endif
}
void W::Window::clearOpenGLThreadAffinity() {
	// Unmake opengl context current on the calling thread
	#ifdef __APPLE__
		[NSOpenGLContext clearCurrentContext];
	#elif defined _WIN32 || _WIN64
		if (!wglMakeCurrent(_objs->deviceContext, NULL))
			throw Exception("Error clearing the OpenGL context thread affinity");
	#endif
}

W::size W::Window::getDimensions() {
	#ifdef __APPLE__
		NSSize bounds = [_objs->view bounds].size;
	//	CGSize bounds = [ob->view bounds].size;
		return size((int)bounds.width, (int)bounds.height);
	#elif defined _WIN32 || _WIN64
		RECT rect;
		GetClientRect(_objs->windowHandle, &rect);
		return size(rect.right - rect.left, rect.bottom - rect.top);
	#endif
}

void W::Window::swapBuffers() {
	#ifdef __APPLE__
		[_objs->context flushBuffer];
	#elif defined _WIN32 || _WIN64
		SwapBuffers(_objs->deviceContext);
	#endif
}

#if defined _WIN32 || _WIN64
LRESULT CALLBACK W::Window::_WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) {
		W::_addEvent(W::Event(
			Window::_win_event_type_map[msg],
			W::position((int)LOWORD(lParam), (int)HIWORD(lParam))
		));
		return 0;
	}
	//else if (msg == WM_CHAR) {
	//	W::_addEvent(W::Event(W::EventType::KEYPRESS, W::Event::charToKeycode(wParam)));
	//	return 0;
	//}
	else if  (msg == WM_KEYDOWN || msg == WM_KEYUP) {
		W::KeyCode::T k = KeyCode::K_OTHER;
		if (wParam == VK_LEFT)        k = KeyCode::LEFT_ARROW;
		else if (wParam == VK_RIGHT)  k = KeyCode::RIGHT_ARROW;
		else if (wParam == VK_UP)	  k = KeyCode::UP_ARROW;
		else if (wParam == VK_DOWN)	  k = KeyCode::DOWN_ARROW;
		else if (wParam == VK_HOME)	  k = KeyCode::HOME;
		else if (wParam == VK_END)	  k = KeyCode::END;
		else if (wParam == VK_DELETE) k = KeyCode::_DELETE;
		else k = Event::charToKeycode(wParam);
		W::_addEvent(W::Event(
			msg == WM_KEYDOWN ? W::EventType::KEYDOWN : W::EventType::KEYUP,
			k
		));
		return 0;
	}
	else if (msg ==  WM_CLOSE) {
		W::_addEvent(W::Event(W::EventType::CLOSED));
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
	if (msg == WM_SYSCOMMAND && (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)) {
		return 0;	// Prevent these from happening
	}

	return DefWindowProc(windowHandle, msg, wParam, lParam);
}

#endif
