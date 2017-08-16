/*
 * W - a simple, cross-platform 2D game develpoment library
 *
 * ============
 *  UIView.h
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __W__UIView
#define __W__UIView

#include "View.h"
#include "UIElement.h"
#include "Event.h"
#include "types.h"

namespace W {
	
	class EventHandler;
	class Callback;
	
	class UIView : public View {
	public:
		UIView(const std::string &viewname);
		~UIView();
		
		void processMouseEvent(Event *);
//		void draw();
		
	protected:
		virtual void drawCustomBackground() { }
			// Override for custom drawing behind elements in UIView
		void updatePosition(const size &winsize);
			// Override; note we do not call _updatePosition because that method of
			// View doesn’t have access to all the positioner arrays of UIView
		
		bool allowDrag;		// Set from positioner when refreshed
		bool dragloop;
		position drag_initial;
		
		typedef std::vector<UIElement*> element_list;
		typedef std::vector<Positioner*> positioner_list;
		
		bool orientation_check;
		enum orientation_enum { O_LANDSCAPE, O_PORTRAIT };
		
		int cur_positioning_index;		// index of cur positioner in positioners vector
		Positioner *cur_positioner;	
		orientation_enum orientation;
		
		std::vector<int> landscape_positioning_limits;	// e.g. 400, 800, 1200
		std::vector<int> portrait_positioning_limits;
		positioner_list landscape_positioners;			// positioners for the uiview
		positioner_list portrait_positioners;
		std::vector<element_list> landscape_elements;	// elements for the given layout
		std::vector<element_list> portrait_elements;
		
		// Initialization from Lua
		bool initialize(const std::string &viewname);
		
		void addPositioner(const std::string &limit, LuaObj *, orientation_enum);
		void addElements(const std::string &limit, LuaObj *, orientation_enum);
		UIElement* createElement(const std::string &limit, const std::string &name, LuaObj *, orientation_enum);
			// These will throw, w/ useful error msg, if problem
		
		void createEvTypeMap();			// Translation table for event types subscribed
		UIElement::EvTypeMap evTypeMap;	// to by UIElements. e.g. MOUSEMOVE => X
										// (Specific to the UIView, since buttons will subscribe
										// to view-specific mousey events submitted in processMouseEvent)
		
		DRect *bgDRect;
	};

}

#endif
