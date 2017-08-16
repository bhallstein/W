/*
 * Window.h
 *
 * Window encapsulates both a Window on the device, and the View and OpenGL
 * state associated with it.
 *
 * The base class defines an abstract interface; different platform-specific
 * implementations are encapsulated as subclasses.
 *
 */

#ifndef __W_Window
#define __W_Window

#include "types.h"
#include "Colour.h"
#include "Event.h"

namespace W {

	class View;
	
	class Window {
	public:
		Window(const W::size &, const std::string &title);
		virtual ~Window();
		
		W::size getSize() { return sz; }
		void setTitle(const std::string &);
		
		void setOpenGLThreadAffinity();
		void clearOpenGLThreadAffinity();
			// Call from the desired thread to obtain access to the GL context
		
		void setUpViewport();
		
		void flushBuffer(); // Draw to the screen
		
		enum Mode { Windowed, FullScreen } mode;
		
		void generateMouseMoveEvent();
		
		#if defined _WIN32 || _WIN64
			LRESULT CALLBACK _WndProc(HWND, UINT, WPARAM, LPARAM);
		#endif
		
	private:
		void createWindow();
		void closeWindow();
		void setUpOpenGL();
		position getMousePosition();
		
		size sz;
		
		struct Objs;
		Objs *objs;
		
		struct Initializer;
		static Initializer *init;
	};
	
}

#endif
