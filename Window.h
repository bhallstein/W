#ifndef __W__Window
#define __W__Window

#include "types.h"
#include "Colour.h"
#include "Event.h"

#if defined WIN32 || WIN64
	#include "Windows.h"
#endif

namespace W {

	struct Window_NativeObjs;

	class View;
	
	class Window {
	public:
		Window();
		~Window();
		
		void setTitle(const char *);
		void _generateMouseMoveEvent();
		
		void _startDrawing();
		void _finishDrawing();
		void _setUpDrawingForView(View *);
		
		void _drawRect(float x, float y, float w, float h, const W::Colour &, float rot);

		size _getDimensions();
#if defined WIN32 || WIN64
		LRESULT CALLBACK _WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
		
	protected:
		void _createWindow();
		void _closeWindow();
		
		void _setUpOpenGL();
		
		Window_NativeObjs *_objs;
		View *_current_drawn_view;

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
