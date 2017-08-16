#ifndef __W__MisterEvent
#define __W__MisterEvent

#include "types.h"

namespace W {
	
	namespace EventType {
		typedef int T;
		enum {
			KEYPRESS,
			MOUSEMOVE,
			LEFTMOUSEUP, LEFTMOUSEDOWN, RIGHTMOUSEUP, RIGHTMOUSEDOWN,
			SCREENEDGE_TOP, SCREENEDGE_BOTTOM, SCREENEDGE_LEFT, SCREENEDGE_RIGHT,
			CLOSED,
			BUTTONCLICK,
			UNKNOWN
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
		
		void setPos(W::position &_pos)      { pos = _pos; }
		void setType(W::EventType::T _type) { type = _type; }
		void setKey(W::KeyCode::T _key)     { key = _key; }
		void setPayload(void *_p)           { _payload = _p; }
		
		position pos;
		EventType::T type;
		KeyCode::T key;
		void *_payload;
		
		static W::EventType::T registerType();
		static W::KeyCode::T charToKeycode(unsigned int c);
		
	private:
		static int _typecounter;
	};
	
}

#endif
