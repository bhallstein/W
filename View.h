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
		View(Positioner *);
		~View();
			// If NULL is supplied for positioner, subclass must
			// set one up and call _updatePosition.
		
		void _updatePosition(const size &winsize);
		
		void _draw();					// Wrapper: prepares W for drawing the view, then calls draw()
		virtual void draw();			// Override to implement your subclass’s drawing behaviour
		void drawRect(int x, int y, int w, int h, const W::Colour &, float rotationInDegrees = 0);
		void drawText(float x, float y, W::Colour &, const char *text, bool rAlign = false);
		
		void receiveEvent(Event *);					// Converts event to view’s coordinates, and calls processMouseEvent()
		virtual void processMouseEvent(Event *) { }	// Override to do something with mouse events
		
		void _setGameState(GameState *g) { _gamestate = g; }
		
	protected:
		Positioner *_positioner;
		GameState *_gamestate;
		
		virtual void updatePosition(const size &winsize) { }	// Let subsclasses perform own position update behaviours
		
	};
	
}

#endif
