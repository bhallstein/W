/*
 * W - a tiny 2D game develpoment library
 *
 * ==========
 *  View.h
 * ==========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__View
#define __W__View

#include "Colour.h"
#include "Callback.h"

#include <map>

#define MR_CURRENCY '$'

namespace W {
	
	class Window;
	class Event;
	class Positioner;
	class GameState;
	class DObj;
	
	class View {
	public:
		View(Positioner *);
		~View();
			// If NULL is supplied for positioner, subclass must
			// set one up and call _updatePosition.
		
		void _updatePosition();	// Gets the window size automatically
		void _updatePosition(const size &winsize);
		
		W::EventPropagation::T receiveEvent(Event *);					// Converts event to view’s coordinates, and calls processMouseEvent()
		virtual void processMouseEvent(Event *) { }	// Override to do something with mouse events
		
		void _subscribeToMouseEvents();		// Called by GS in addView()
		void _unsubscribeFromMouseEvents();	// Called by GS in removeView()
		
		void _draw(const W::size &winSz);
		
		DObj *firstDObj, *lastDObj;
			// DObjs form a doubly-linked list.
			// View needs refs to the first & the last.
		
		int _getStorageForDObjOfLength(int);
			// Called by DObj when created, returns the start index of the chunk assigned to the object
		void _removeStorageForDObjOfLength(int);
		
		void _updateDObjs(); // Call recopy() on _DObjs_needing_recopy, clear
		void _setNeedsRecopy(DObj *_d) { _DObjs_needing_recopy.push_back(_d); }
		void _unsetNeedsRecopy(DObj *_d) {
			for (std::vector<DObj*>::iterator it = _DObjs_needing_recopy.begin(); it < _DObjs_needing_recopy.end(); )
				if (*it == _d) it = _DObjs_needing_recopy.erase(it);
				else ++it;
		}
		
	protected:
		Positioner *_positioner;
		rect rct;
		
		struct glDataArrays;
		glDataArrays *glData;		// Vertex, colour & texcoord info
		
		position _offset;	// Def 0. Used by scrolling subviews. Undocumented!
		
		virtual void updatePosition(const W::size &winsize) { }	// Let subsclasses perform own position update behaviours
		virtual void customOpenGLDrawing() { }
		
		std::vector<DObj*> _DObjs_needing_recopy;
	};
	
}

#endif
