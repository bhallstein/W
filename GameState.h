#ifndef __W__GameState
#define __W__GameState

#include "Messenger.h"
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
		typedef std::list<View*> Viewlist;
		
		GameState(GameStateTranslucency _tr = GS_OPAQUE);
		virtual ~GameState();
		
		virtual void resume(Returny *) = 0;
		virtual void update() = 0;
		
		void addView(View *);
		void removeView(View *);
		Viewlist _vlist;
		
		virtual void handleCloseEvent();	// By default, initiates the killer returny tango: override to customise
		
		bool isTranslucent() { return translucent; }
		
	protected:
		// Properties
		bool translucent;	// If true, the previous state will be drawn too.
	};
	
	extern Returny KillerReturny;
	extern Returny EmptyReturny;

}

#endif
