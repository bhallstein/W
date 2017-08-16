#ifndef __W__GameState
#define __W__GameState

#include "EventHandler.h"
#include <vector>
#include <list>

namespace W {
	
	namespace ReturnyType {
		typedef int T;
		enum {
			EMPTY_RETURNY,
			KILLER_RETURNY,
			PAYLOAD_RETURNY		// has a string payload
		};
	}
	
	class Returny {
	public:
		Returny(ReturnyType::T _type) : type(_type) { }
		Returny(const Returny &r) : type(r.type), payload(r.payload) { }
		bool operator==(Returny &r) {
			return type == r.type;
		}
		ReturnyType::T type;
		std::string payload;
	};
	
	class Event;
	class View;
	
	enum GameStateTranslucency { GS_OPAQUE = 0, GS_TRANSLUCENT };

	class GameState {
	public:
		GameState(GameStateTranslucency _tr = GS_OPAQUE) : translucent(_tr) { }
		virtual ~GameState() { }
		
		virtual void resume(Returny *) = 0;
		virtual void update() = 0;
		
		void addView(View *);
		void removeView(View *);
		
		void handleEvent(Event *ev);		// Send the event on, via the gamestate's responderMap
		virtual void handleCloseEvent();	// By default, initiates the killer returny tango: override to customise
		
//		void setResolution(int _w, int _h);
		
		bool isTranslucent() { return translucent; }
		
		void _updateWV(Window *, View *);
		void _removeFromWV(View *);
		std::map<Window*, std::list<View*> >* _getViews() { return &_wv; }
		
	protected:
		// Properties
		EventHandler eh;
		std::map<Window*, std::list<View*> > _wv;
		bool translucent;	// If true, the previous state will be drawn too.
	};
	
	extern Returny KillerReturny;
	extern Returny EmptyReturny;

}

#endif
