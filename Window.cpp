/*
 * W - a tiny 2D game develpoment library
 *
 * ==============
 *  Window.cpp
 * ==============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Window.h"
#include "W.h"
#include "Log.h"
#include "MegaTexture.h"

#include "oglInclude.h"

#ifdef WTARGET_IOS
	#define glOrtho glOrthof
#endif

/*************************************/
/*** Window: common implementation ***/
/*************************************/

W::Window::Window(const size &_sz, const std::string &_title) :
	sz(_sz),
	objs(NULL)
{
	createWindow();
	setTitle(_title);
	setUpOpenGL();
}
W::Window::~Window()
{
	closeWindow();
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
void W::Window::setUpViewport() {
	glViewport(0, 0, sz.width, sz.height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, sz.width, sz.height, 0, -1, 1);
	
	glMatrixMode(GL_MODELVIEW);
}
void W::Window::beginDrawing(const size &winSize) {
	glScissor(0, 0, winSize.width, winSize.height);
	glClearColor(0.525, 0.187, 0.886, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, MegaTexture::getGLTexId());
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_MAG_FILTER, GL_LINEAR);
	
	#ifdef WTARGET_IOS
		setUpForDrawing();	// On iOS, need to bind the frame buffer
	#endif
}
void W::Window::generateMouseMoveEvent() {
	position p = getMousePosition();
	if (p.x >= 0 && p.y >= 0 && p.x < sz.width && p.y < sz.height)
		W::Event::_addEvent(new W::Event(EventType::MOUSEMOVE, p));
	
	static int generosity = 20, scrollMargin = 20;
	// If within the view + generosity margin...
	if (p.x >= -generosity && p.y >= -generosity && p.x < sz.width+generosity && p.y < sz.height+generosity) {
		if (p.x < scrollMargin) {
			float d = float(scrollMargin-p.x)/float(scrollMargin);
			W::Event::_addEvent(new Event(EventType::SCREENEDGE_LEFT, float(d>1.0?1.0:d)));
		}
		else if (p.x >= sz.width-scrollMargin) {
			float d = float(p.x-(sz.width-scrollMargin))/float(scrollMargin);
			W::Event::_addEvent(new Event(EventType::SCREENEDGE_RIGHT, float(d>1.0?1.0:d)));
		}
		if (p.y < scrollMargin) {
			float d = float(scrollMargin-p.y)/float(scrollMargin);
			W::Event::_addEvent(new Event(EventType::SCREENEDGE_TOP, float(d>1.0?1.0:d)));
		}
		else if (p.y >= sz.height-scrollMargin) {
			float d = float(p.y-(sz.height-scrollMargin))/float(scrollMargin);
			W::Event::_addEvent(new Event(EventType::SCREENEDGE_BOTTOM, float(d>1.0?1.0:d)));
		}
	}
}


/*****************************************/
/*** Window: Mac-specific implemention ***/
/*****************************************/

#ifdef WTARGET_MAC

#include "MacOSXClasses.h"

struct W::Window::Objs {
	W_Window *w_window;
};

W::Window::Initializer *W::Window::init;

void W::Window::createWindow() {
	if (objs) {
		W::log << "createWindow() called, but seems already to exist\n";
		return;
	}
		// At some point, want to take mode, check if different, then destroy
		// & recreate the window in the new mode.
	
	objs = new Objs();
	
	objs->w_window = [[W_Window alloc] initWithWidth:sz.width height:sz.height];
	if (objs->w_window == nil)
		throw Exception("Could not create window; see log file for details");
}
void W::Window::closeWindow() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
}
void W::Window::setTitle(const std::string &t) {
	[objs->w_window setTitle:[NSString stringWithUTF8String:t.c_str()]];
}
void W::Window::setOpenGLThreadAffinity() {
	[objs->w_window setOpenGLThreadAffinity];
}
void W::Window::clearOpenGLThreadAffinity() {
	[objs->w_window clearOpenGLThreadAffinity];
}
void W::Window::flushBuffer() {
	[objs->w_window flushBuffer];
}
W::position W::Window::getMousePosition() {
	NSPoint p = [objs->w_window getMousePosition];
	return position((int)p.x, (int)p.y);
}


/******************************************/
/*** iOS-specific Window implementation ***/
/******************************************/

#elif defined WTARGET_IOS

#include "iOSClasses.h"

struct W::Window::Objs {
	UIWindow *window;
	W_ViewController *vc;
	EAGLView *view;
	EAGLContext *context;
};

void W::Window::createWindow() {
	if (objs) {
		W::log << "createWindow() called, but seems to already exist\n";
		return;
	}
	
	objs = new Objs();
	
	CGRect windowFrame = [[UIScreen mainScreen] bounds];
	sz = size(windowFrame.size.width, windowFrame.size.height);
	
	objs->window = [[UIWindow alloc] initWithFrame:windowFrame];
	if (objs->window == nil)
		throw Exception("Could not create UIWindow");
	
	objs->window.rootViewController = objs->vc = [[W_ViewController alloc] init];
	[objs->window makeKeyAndVisible];
	
	objs->view = objs->vc.v;
	objs->context = objs->view.context;
	
	sz *= objs->view.contentScaleFactor;
}
void W::Window::closeWindow() {
	if (objs) {
		delete objs;
		objs = NULL;
	}
}
void W::Window::setOpenGLThreadAffinity() {
	[objs->view setOpenGLThreadAffinity];
}
void W::Window::clearOpenGLThreadAffinity() {
	[objs->view clearOpenGLThreadAffinity];
}
void W::Window::flushBuffer() {
	[objs->view flushBuffer];
}
void W::Window::setUpForDrawing() {
	[objs->view bindFramebuffer];
}
void W::Window::setTitle(const std::string &t) { }
W::position W::Window::getMousePosition() { return position(); }


/******************************************/
/*** Win-specific Window implementation ***/
/******************************************/

#elif defined _WIN32 || _WIN64

#include "Windows.h"
//#include "shlobj.h"
#include <map>

struct W::Window::Objs {
	HWND windowHandle;
	HDC deviceContext;
	HGLRC renderingContext;
};

// Initialization

LRESULT CALLBACK Window_WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
	// If message if WM_NCCREATE, set pointer to MyWindow
	// (Since in CreateWindowEx we set lpParam to 'this', lParam is here a ptr to the window)
	if (msg == WM_NCCREATE) {
		LPCREATESTRUCT cs = (LPCREATESTRUCT) lParam;
		SetWindowLongPtr(windowHandle, GWL_USERDATA, (long) cs->lpCreateParams);
	}
	// Otherwise, get window pointer previously set as above
	W::Window *win = (W::Window *) GetWindowLongPtr(windowHandle, GWL_USERDATA);
	return win ? win->_WndProc(windowHandle, msg, wParam, lParam) : DefWindowProc(windowHandle, msg, wParam, lParam);
}

HINSTANCE _appInstance;
std::map<UINT, W::EventType::T> _win_event_type_map;

struct W::Window::Initializer {
	Initializer() {
		// Set appInstance ref
		_appInstance = GetModuleHandle(NULL);
		if (_appInstance == NULL)
			throw Exception("Couldn't get app instance.");
		
		// Register window class
		WNDCLASSEX wc;	// Before creating a window, you have to register a class for it
		
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Force redraw on resize; has own device context
		wc.lpfnWndProc = &Window_WndProc;
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

W::Window::Initializer *W::Window::init = new W::Window::Initializer();

// Class implementation

void W::Window::createWindow() {
	if (objs) {
		W::log << "createWindow() called, but seems to already exist\n";
		return;
	}
	
	objs = new Objs();

	// Set window style & size
	DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD extendedWindowStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	
	int width = 800, height = 600;

	RECT rect;
	rect.left = 0, rect.right = width;
	rect.top = 0, rect.bottom = height;
	AdjustWindowRectEx(&rect, windowStyle, FALSE, extendedWindowStyle);
	
	// Create window
	objs->windowHandle = CreateWindowEx(
		extendedWindowStyle,
		"W_Window",				// window class
		"My W Application",		// window title
		windowStyle,
		0, 0,					// position
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,					// parent window
		NULL,					// menu
		_appInstance,
		this
	);
	if(objs->windowHandle == NULL) {
		DWORD err = GetLastError();
		char errorMsg[200];
		wsprintf(errorMsg, "Error creating window. Error code: %d, %X.", err, err);
		closeWindow();
		throw Exception(errorMsg);
	}
	// Get device context
	if (!(objs->deviceContext = GetDC(objs->windowHandle))) {
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
	if (!(pf = ChoosePixelFormat(objs->deviceContext, &pfd))) {
		closeWindow();
		throw Exception("Unable to get a suitable pixel format");
	}
	if(!SetPixelFormat(objs->deviceContext, pf, &pfd)) {
		closeWindow();
		throw Exception("Unable to set the pixel format");
	}
	// Create rendering context
	if (!(objs->renderingContext = wglCreateContext(objs->deviceContext))) {
		closeWindow();
		throw Exception("Error creating a rendering context");
	}
	
	ShowWindow(objs->windowHandle, SW_SHOW);
	SetForegroundWindow(objs->windowHandle);
	SetFocus(objs->windowHandle);
}
void W::Window::closeWindow() {
	if (objs->renderingContext) {
		// The window is deleted from the quit() function, after the draw thread has released the context and exited.
		// Therefore we don't need to unmake the context current before exiting - in general, this must be done before
		// the window is deleted.
		// NB: if there's a problem during window creation, may need to call clearOpenGLThreadAffinity() before closeWindow().
		//if (!wglMakeCurrent(NULL, NULL))
		//	MessageBox(NULL, "Error releasing device and rendering contexts", "Error", MB_OK | MB_ICONEXCLAMATION);
		if (!wglDeleteContext(objs->renderingContext))
			MessageBox(NULL, "Error deleting the rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);
		objs->renderingContext = NULL;
	}
	if (objs->deviceContext && !ReleaseDC(objs->windowHandle, objs->deviceContext))
		MessageBox(NULL, "Error releasing the device context", "Error", MB_OK | MB_ICONEXCLAMATION);
	objs->deviceContext = NULL;
	if (objs->windowHandle && !DestroyWindow(objs->windowHandle))
		MessageBox(NULL, "Error destroying the window", "Error", MB_OK | MB_ICONEXCLAMATION);
	objs->windowHandle = NULL;
	if (!UnregisterClass("W_Window", _appInstance))
		MessageBox(NULL, "Error unregistering the window class", "Error", MB_OK | MB_ICONEXCLAMATION);
	_appInstance = NULL;
}
void W::Window::setTitle(const std::string &t) {
	SetWindowText(objs->windowHandle, t.c_str());
}
void W::Window::setOpenGLThreadAffinity() {
	if (!wglMakeCurrent(objs->deviceContext, objs->renderingContext)) {
		closeWindow();
		throw Exception("Error setting the OpenGL context thread affinity");
	}
}
void W::Window::clearOpenGLThreadAffinity() {
	if (!wglMakeCurrent(objs->deviceContext, NULL))
		throw Exception("Error clearing the OpenGL context thread affinity");
}
void W::Window::flushBuffer() {
	SwapBuffers(objs->deviceContext);
}
W::position W::Window::getMousePosition() {
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(objs->windowHandle, &p);
	return position((int)p.x, (int)p.y);
}
LRESULT CALLBACK W::Window::_WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) {
		W::Event::_addEvent(new W::Event(
			_win_event_type_map[msg],
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
		W::Event::_addEvent(new W::Event(
			msg == WM_KEYDOWN ? W::EventType::KEYDOWN : W::EventType::KEYUP,
			k
		));
		return 0;
	}
	else if (msg ==  WM_CLOSE) {
		W::Event::_addEvent(new W::Event(W::EventType::CLOSED));
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

//#include "texture.h"

//W::Window::Window(const size &_size, const char *_title)
//{
//	// Check window has not already been created
//	if (W::_window) {
//		std::stringstream ss;
//		ss << "Cannot create window: window has already been created (" << _window << ")";
//		throw Exception(ss.str());
//	}
//}

//W::size W::Window::getSize() {
//	#ifdef __APPLE__
//		NSSize bounds = [objs->view bounds].size;
//	//	CGSize bounds = [ob->view bounds].size;
//		return size((int)bounds.width, (int)bounds.height);
//	#elif defined _WIN32 || _WIN64
//		RECT rect;
//		GetClientRect(objs->windowHandle, &rect);
//		return size(rect.right - rect.left, rect.bottom - rect.top);
//	#endif
//}
