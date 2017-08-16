/*
 * W - a tiny 2D game development library
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
		View(const Positioner *);
		~View();
			// If NULL is supplied for positioner, subclass must
			// set one up and call _updatePosition.
		
		void _updatePosition();	// Gets the window size automatically
		void _updatePosition(const size &winsize);
		
		void _convertEventCoords(Event *);				// Converts to view's coordinates, and calls convertEventCoords() virtual
		virtual void convertEventCoords(Event *) { }	// Override to perform further conversoin conversion of mouse events
		
		virtual void mouseEvent(Event *) { }
		virtual void touchDown(Event *) { }
		
		void _draw(const W::size &winSz);
		
		void _addDObj(DObj *);	// Allocate storage, add to list, set ptrs
		void _remDObj(DObj *);	// Remove from list, make triangles degen, set preceding_empty on following DObj
		
		void _updateDObjTexcoords();
			// Calls updateTexcoords() on all DObjs. Called when MT changes in size:
			//    -> Controller::regenDObjTexcoords()
			// Then at update:
			//  -> GS::_updateAllDObjTexcoords()
			//  -> GS::_updateAllDObjTexcoordsInThisState()
			//  -> View::_updateDObjTexcoords()
			//  -> DObj::updateTexCoords();
		
		void dumpDObjs();
		
		const rect& getRct() { return rct; }
		
	protected:
		Positioner *_positioner;
		rect rct;
		
		position _offset;	// Def 0. Used by scrolling subviews. Undocumented!
		
		virtual void updatePosition(const W::size &winsize) { }	// Let subsclasses perform own position update behaviours
		virtual void customOpenGLDrawing() { }
		
	private:
		v3f *vertArray;
		c4f *colArray;
		t2f *texcoordArray;
		int array_size;
		int array_used_size;
		
		void increaseArraySize();	// Double size of arrays
		void compact();				// Remove padding & halve size of arrays if below threshold usage
		void updateDObjPtrs();		// Update all DObj ptrs (when reallocating the arrays)
		
		int frameCount;
		
		DObj *firstDObj, *lastDObj;
			// DObjs form a doubly-linked list. View needs refs to the first &
			// last, as do DObjs (for inserting into the list).
		
	};
	
}

#endif
