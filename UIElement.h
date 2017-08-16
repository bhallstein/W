/*
 * UIElement.h
 *
 */

#ifndef __W__UIElement
#define __W__UIElement

#include "types.h"
#include "MappedObj.h"
#include "Event.h"
#include "Positioner.h"

#include <string>
#include <map>

namespace W {

	class UIElement : public W::MappedObj {
	public:
		typedef std::map<EventType::T, EventType::T> EvTypeMap;
		
		UIElement(const std::string &_name, W::Positioner *, EvTypeMap &);
		~UIElement();
		
		void update(const size &containersize);		// Update positioner
		
	protected:
		std::string name;
		W::Positioner *positioner;
		std::map<EventType::T, EventType::T> &evTypes;
	};
	
	
	class Button : public UIElement {
	public:
		Button(const std::string &_name, W::Positioner *, EvTypeMap &);
		~Button();
		void recEv(Event *);
	protected:
		bool hover;
		bool active;
		Event buttonClickEvent;
	};

}

#endif
