#ifndef __W_WINdow
#define __W_WINdow

#include "types.h"
#include "Colour.h"
#include "Event.h"

#if defined WIN32 || WIN64
	#include "Windows.h"
#endif

namespace W {

	class View;
	
	class Window {
	public:
		Window(const W::size &, const char *_title);
		~Window();
		
		W::size getDimensions();
		
		void setTitle(const std::string &);
		void setTitle(const char *);
		void generateMouseMoveEvent();
		
		void setOpenGLThreadAffinity();
		void clearOpenGLThreadAffinity();
		
#if defined WIN32 || WIN64
		LRESULT CALLBACK _WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
		struct NativeObjs;
		void swapBuffers();
		
	protected:
		void createWindow(const W::size &, const char *_title);
		void closeWindow();
		void setUpOpenGL();
		
		struct NativeObjs *_objs;
		
#if defined WIN32 || WIN64
		struct _initializer;
		static _initializer *_init;
		static HINSTANCE _appInstance;
		void _convertToWEvent(W::Event &, UINT, WPARAM, LPARAM);
#endif
	};

#if defined WIN32 || WIN64
	LRESULT CALLBACK Window_WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
	
}

#endif
