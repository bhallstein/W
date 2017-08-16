/*
 * UIElement.h
 *
 */

#ifndef __W__UIElement
#define __W__UIElement

#include "MappedObj.h"
#include "Event.h"

#include <string>
#include <map>

namespace W {

	class UIElement : public W::MappedObj {
	public:
		typedef std::map<EventType::T, EventType::T> evTypeMap;
		
		UIElement(const char *_name, evTypeMap &);
		~UIElement();
		
		virtual void mouseEvent(Event *);
		
	protected:
		std::string name;
		std::map<EventType::T, EventType::T> &evTypes;
	};
	
	
	class Button : public UIElement {
	public:
		Button(const char *_name, evTypeMap &);
		~Button();
		void recEv(Event *);
	protected:
		bool hover;
		bool active;
		Event buttonClickEvent;
	};

}

#endif
