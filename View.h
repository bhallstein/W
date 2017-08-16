#ifndef __W__View
#define __W__View

#include "MappedObj.h"
#include "Colour.h"

#define MR_CURRENCY '$'

namespace W {
	
	class Window;
	class Event;
	class Positioner;
	class GameState;
	
	class View : public MappedObj {
	public:
		View(Positioner *, Window *);
		~View();
		
		void setWindow(Window *w);
		void _updatePosition(const size &winsize);
		
		void _draw();					// Wrapper: prepares W for drawing the view, then calls draw()
		virtual void draw();			// Override to implement your subclass’s drawing behaviour
		void drawRect(int x, int y, int w, int h, const W::Colour &, float rotationInDegrees = 0);
		void drawText(float x, float y, W::Colour &, const char *text, bool rAlign = false);
		
		void receiveEvent(Event *);					// Converts event to view’s coordinates, and calls processMouseEvent()
		virtual void processMouseEvent(Event *) { }	// Override to do something with mouse events
		
		Window *_getWindow() { return _window; }
		void _setGameState(GameState *g) { _gamestate = g; }
		
	protected:
		View(Window *);				// Let subsclasses set up their own positioner
		Positioner *_positioner;
		Window *_window;
		GameState *_gamestate;
		
		virtual void updatePosition(const size &winsize) { }	// Let subsclasses perform own position update behaviours
		
	};
	
}

#endif
