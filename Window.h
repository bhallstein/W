#ifndef __W__Window
#define __W__Window

#include "types.h"
#include "Colour.h"

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
		
	protected:
		void createWindow();
		void closeWindow();
		
		void setUpOpenGL();
		
		Window_NativeObjs *objs;
		View *current_drawn_view;
	};
	
}

#endif
