/*
 * UIElement.h
 *
 */

#ifndef __W__UIElement
#define __W__UIElement

#include "types.h"
#include "Event.h"
#include "Positioner.h"
#include "Callback.h"

#include <string>
#include <map>

namespace W {

	class View;
	class DRect;
	
	class UIElement {
	public:
		typedef std::map<EventType::T, EventType::T> EvTypeMap;
		
		UIElement(const std::string &_name, W::Positioner *, EvTypeMap &);
		~UIElement();
		
		void _updatePosition(const size &containersize);
		virtual void updatePosition() { }
			// Override to implement updates to D.O.s etc
		
		virtual void activate() = 0;
		virtual void deactivate() = 0;
		
	protected:
		std::string name;
		W::Positioner *positioner;
		rect rct;
		std::map<EventType::T, EventType::T> &evTypes;
	};
	
	
	class Button : public UIElement {
	public:
		Button(View *, const std::string &_name, W::Positioner *, EvTypeMap &);
		~Button();
		EventPropagation::T recEv(Event *);
		void activate();
		void deactivate();
	protected:
		void updatePosition();
		bool hover;
		bool active;
		Event buttonClickEvent;
		DRect *btnrect;
		View *view;
	};

}

#endif
