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
			K_A,	K_B,	K_C,	K_D,
			K_E,	K_F,	K_G,	K_H,
			K_I,	K_J,	K_K,	K_L,
			K_M,	K_N,	K_O,	K_P,
			K_Q,	K_R,	K_S,	K_T,
			K_U,	K_V,	K_W,	K_X,
			K_Y,	K_Z,
			
			K_0,	K_1,	K_2,
			K_3,	K_4,	K_5,
			K_6,	K_7,	K_8,	K_9,
			
			K_LEFT_ARROW,	K_RIGHT_ARROW,
			K_UP_ARROW,		K_DOWN_ARROW,
			
			K_ESC,	K_RETURN, K_BACKSPACE, K_DELETE, K_TAB,
			K_HOME, K_END,
			
			K_OTHER
		};
	}
	
	class Event {
	public:
		Event() : treat_as_mouse_event(false)
		{
			
		}
		Event(EventType::T t, bool _treat_as_mouse_event = false) :
			type(t), treat_as_mouse_event(_treat_as_mouse_event)
		{
			
		}
		Event(EventType::T t, int x, int y, bool _treat_as_mouse_event = true) :
			type(t), treat_as_mouse_event(_treat_as_mouse_event)
		{
			pos.x = x, pos.y = y;
		}
		Event(EventType::T t, KeyCode::T k) : type(t), key(k), treat_as_mouse_event(false)
		{
			
		}

		static W::KeyCode::T charToKeycode(unsigned int c);
		
		void setCoords(int x, int y, int a = 0, int b = 0) {
			pos.x = x, pos.y = y;
			pos.a = a, pos.b = b;
		}
		void setType(EventType::T _type)  { type = _type; }
		void setKey(KeyCode::T _key)      { key = _key; }
		void setPayload(void *_p) { _payload = _p; }
		
		static W::EventType::T registerType() { return _typeid++; }
		
		bool treat_as_mouse_event;
		position pos;
		EventType::T type;
		KeyCode::T key;
		void *_payload;

	private:
		static int _typeid;
	};
	
}

#endif
