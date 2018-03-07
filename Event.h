/*
 * W - a tiny 2D game development library
 *
 * ===========
 *  Event.h
 * ===========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__MisterEvent
#define __W__MisterEvent

#include "types.h"
#include "Mutex.h"
#include <string>

namespace W {
	
	namespace EventType {
		typedef int T;
		enum {
			// Key
			KeyDown, KeyUp,
			
			// Positional
			MouseMove, LMouseUp, LMouseDown, RMouseUp, RMouseDown,	// Mouse
			TouchDown, TouchMoved, TouchUp, TouchCancelled,			// Touch
			
			// ScreenEdge
			ScreenEdgeTop, ScreenEdgeBottom, ScreenEdgeLeft, ScreenEdgeRight,
			
			// UI
			ButtonClick,
			
			// Other
			Closed, Unknown
		};
	}
	
	namespace KeyCode {
		typedef int T;
		enum {
			_A,	_B,	_C,	_D,
			_E,	_F,	_G,	_H,
			_I,	_J,	_K,	_L,
			_M,	_N,	_O,	_P,
			_Q,	_R,	_S,	_T,
			_U,	_V,	_W,	_X,
			_Y,	_Z,
			
			_0,	_1,	_2,
			_3,	_4,	_5,
			_6,	_7,	_8,	_9,
			
			SPACE,
			
			LEFT_ARROW,	RIGHT_ARROW,
			UP_ARROW,	DOWN_ARROW,
			
			ESC,   RETURN,  BACKSPACE,  _DELETE,  TAB,
			HOME,  END,
			
			K_OTHER
		};
	}
	
	class Event {
	public:
		Event(EventType::T);
		Event(EventType::T, v2i );
		Event(EventType::T, KeyCode::T);
		Event(EventType::T, float _x);
		Event(EventType::T, int _touchID, v2i _pos, v2i _prev_pos = v2i(-1,-1));
		
		std::string _printType();
		
		v2f pos;
		v2f prev_pos;
		EventType::T type;
		KeyCode::T key;
		float x;
		int touchID;
		std::string payload;
		
		static EventType::T registerType();
		static KeyCode::T charToKeycode(unsigned int c);
		
		static std::vector<Event> _events;
		static void _addEvent(Event);
			// W's event queue
		
		static Mutex _mutex;
			// On windows due to MMT usage the event harvesting
			// code is on a sep. thread to the update cycle.
		
	private:
		static int _typecounter;
	};
	
}

#endif
