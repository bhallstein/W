#ifndef __W__View
#define __W__View

#include "MappedObj.h"
#include "Colour.h"

#include <map>

#define MR_CURRENCY '$'

namespace W {
	
	class Window;
	class Event;
	class Positioner;
	class GameState;
	class DrawnObj;
	
	class View : public MappedObj {
	public:
		View(Positioner *);
		~View();
			// If NULL is supplied for positioner, subclass must
			// set one up and call _updatePosition.
		
		void _updatePosition(const size &winsize);
		
		void receiveEvent(Event *);					// Converts event to view’s coordinates, and calls processMouseEvent()
		virtual void processMouseEvent(Event *) { }	// Override to do something with mouse events
		
		void _subscribeToMouseEvents();		// Called by GS in addView()
		void _unsubscribeFromMouseEvents();	// Called by GS in removeView()
		
		// Drawn Object methods
		void addDO(DrawnObj *, int layer = 1);
		void removeDO(DrawnObj *);
		void _markDOAsDirty(DrawnObj *);
		void _updateDOs();
			// Update status of all drawn objects.
			// Must be called after locking the graphics mutex.

		void _draw(const W::size &winSz);
	protected:
		Positioner *_positioner;
		
		// Drawn Object vectors
		typedef std::vector<DrawnObj*> DO_list;
		std::map<int, DO_list> scene;
		DO_list
			deletedDOs,
			dirtyDOs;
		struct DOAndLayer {
			DOAndLayer(DrawnObj *_DO, int _layer) : DO(_DO), layer(_layer) { }
			DrawnObj *DO;
			int layer;
		};
		std::vector<DOAndLayer> newDOs;

		void _removeDO(DrawnObj *);
		
		virtual void updatePosition(const size &winsize) { }	// Let subsclasses perform own position update behaviours
		
	};
	
}

#endif
