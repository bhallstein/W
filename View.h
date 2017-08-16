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
	class DColouredShape;
	class DTexturedShape;
	class TextureAtlas;
	class StorageObjForColouredShapes;
	class StorageObjForTexturedShapes;
	
	
	struct Layer {
		Layer();
		~Layer();
		std::map<BlendMode::T, StorageObjForColouredShapes*> cGroups;
		std::map<BlendMode::T, std::map<TextureAtlas*, StorageObjForTexturedShapes*>> tGroups;
		void compact();
	};

	
	class View {
	public:
		View(const Positioner *);
		~View();
			// If NULL is supplied for positioner, subclass must
			// set one up and call _updatePosition.
		
		void _updatePosition();	// Gets the window size automatically
		void _updatePosition(const size &winsize);
		
		void _convertEventCoords(Event *);				// Converts to view's coordinates, and calls convertEventCoords() virtual
		virtual void convertEventCoords(Event *) { }	// Override to perform further conversion of mouse events
		
		virtual void mouseEvent(Event *) { }
		virtual void touchDown(Event *) { }
		
		void _draw(const W::size &winSz);
		
		const rect& getRct() { return rct; }
		
		// New Drawable Stuff
		void addDrawable(DColouredShape *);
		void addDrawable(DTexturedShape *);
		void removeDrawable(DColouredShape *);
		void removeDrawable(DTexturedShape *);
		void compactAllLayers();
		
		std::map<int, Layer> layers;
		
	protected:
		Positioner *_positioner;
		rect rct;
		
		position _offset;	// Def 0. Used by scrolling subviews. Undocumented!
		
		virtual void updatePosition(const W::size &winsize) { }	// Let subsclasses perform own position update behaviours
		virtual void customOpenGLDrawing() { }
		
	private:
		int frameCounter;
		
	};
	
}

#endif
