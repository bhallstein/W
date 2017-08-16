/*
 * W - a tiny 2D game development library
 *
 * ============
 *  Window.h
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// Window encapsulates a window on the device, and the View and OpenGL
// state associated with it.

#ifndef __W_Window
#define __W_Window

#include "types.h"
#include "Colour.h"
#include "Event.h"

#ifdef WTARGET_WIN
	#include "Windows.h"
#endif

namespace W {

	class View;
	
	class Window {
	public:
		Window(const W::size &, const std::string &title);
		virtual ~Window();
		
		const size& getSize() { return sz; }
		void setTitle(const std::string &);
		
		void setOpenGLThreadAffinity();
		void clearOpenGLThreadAffinity();
			// Call to give the current thread access to the GL context
		
		void setUpViewport();
		void beginDrawing();	// Prepare to start new drawing cycle
		void flushBuffer();		// Draw to the screen
		
		enum Mode { Windowed, FullScreen } mode;
		
		void generateMouseMoveEvent();
		
		void updateSize(const W::size &);
			// Mac - called by the Window object
			// iOS - called by the EAGLView object
			// Win - ...
		
		#if defined WTARGET_WIN
			LRESULT CALLBACK _WndProc(HWND, UINT, WPARAM, LPARAM);
		#endif
		
		bool winSizeHasChanged;
		
	private:
		void createWindow();
		void closeWindow();
		void setUpOpenGL();
		void setUpForDrawing();
		position getMousePosition();
		
		size sz;
		
		struct Objs;
		Objs *objs;
		
		struct Initializer;
		static Initializer *init;
	};
	
	extern bool Retina;
	
}

#endif
