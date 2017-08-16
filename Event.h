/*
 * W - a tiny 2D game develpoment library
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

namespace W {
	
	namespace EventType {
		typedef int T;
		enum {
			KEYDOWN, KEYUP,
			MOUSEMOVE,
			LEFTMOUSEUP, LEFTMOUSEDOWN, RIGHTMOUSEUP, RIGHTMOUSEDOWN,
			SCREENEDGE_TOP, SCREENEDGE_BOTTOM, SCREENEDGE_LEFT, SCREENEDGE_RIGHT,
			CLOSED,
			BUTTONCLICK,

			TouchDown, TouchMoved, TouchUp, TouchCancelled,
			Unknown
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
		Event(W::EventType::T);
		Event(W::EventType::T, const W::position &);
		Event(W::EventType::T, W::KeyCode::T);
		Event(W::EventType::T, float _x);
		Event(EventType::T, int _touchID, const position &_pos, const position &_prev_pos = position(-1,-1));
		
		W::position pos;
		W::position prev_pos;
		W::EventType::T type;
		W::KeyCode::T key;
		float x;
		int touchID;
		void *_payload;
		
		static W::EventType::T registerType();
		static W::KeyCode::T charToKeycode(unsigned int c);
		
		static std::vector<W::Event*> _events;
		static void _addEvent(W::Event *);
			// W's event queue
		
		static Mutex _mutex;
			// On windows due to MMT usage the event harvesting
			// code is on a sep. thread to the update cycle.
		
	private:
		static int _typecounter;
	};
	
}

#endif
