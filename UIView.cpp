/*
 * W - a tiny 2D game development library
 *
 * ==============
 *  UIView.cpp
 * ==============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "W.h"

#include "UIView.h"
#include "Callback.h"
#include "Messenger.h"
#include "Positioner.h"
#include "Window.h"
#include "DrawingClasses.h"
#include "W_internal.h"

#include "LuaObj.h"
extern "C" {
	#include "lauxlib.h"
}

#include <algorithm>

W::UIView::UIView(const std::string &viewname) :
	View(),
	dragloop(false),
	cur_positioner_index(-1)
{
	// Initialize from Lua
	using std::string;
	if (!initialize(viewname))
		throw Exception(
			string("Error creating UIView '") + string(viewname) +
			string("' - see log file for further details")
		);
	
	bgrect = new Rectangle(this, v2i(), rct.size, W::Colour::TransparentBlack);
  updatePosition(windowSize());
}

W::UIView::UIView() :
  View(),
  dragloop(false),
  cur_positioner_index(-1)
{

}

W::UIView::~UIView()
{
	delete bgrect;
}

void W::UIView::mouseEvent(Event ev) {
	using namespace EventType;
	if (ev.type == LMouseDown && allowDrag
		&& Messenger::requestPrivilegedEventResponderStatus(this, MouseMove, Callback(&UIView::dragLoopEvent, this), true)
		&& Messenger::requestPrivilegedEventResponderStatus(this, LMouseUp, Callback(&UIView::dragLoopEvent, this), true)
  ) {
		drag_initial = ev.pos;
		dragloop = true;
	}
}
W::EventPropagation::T W::UIView::dragLoopEvent(Event ev) {
	using namespace EventType;
	if (ev.type == MouseMove) {
		cur_positioner->nudge(ev.pos - drag_initial);
		_updatePosition();
	}
	else if (ev.type == LMouseUp) {
		Messenger::relinquishPrivilegedEventResponderStatus(this, MouseMove, this, true);
		Messenger::relinquishPrivilegedEventResponderStatus(this, LMouseUp, this, true);
		dragloop = false;
	}
	return W::EventPropagation::ShouldStop;
}

void W::UIView::updatePosition(v2i winsize) {
  if (orientation_check) {
		orientation = (winsize.a >= winsize.b ? O_LANDSCAPE : O_PORTRAIT);
  }
	
	std::vector<int> *limit_vec;
	positioner_list *psnr_vec;
	std::vector<element_list> *ellist_vec;
	
	if (orientation == O_LANDSCAPE) {
		limit_vec  = &landscape_positioning_limits;
		psnr_vec   = &landscape_positioners;
		ellist_vec = &landscape_elements;
	}
	else {
		limit_vec  = &portrait_positioning_limits;
		psnr_vec   = &portrait_positioners;
		ellist_vec = &portrait_elements;
	}
	
	// Get positioning index
	// - Limits are "up to": a limit of 400 applies up to & including width 400
	// - If width larger than greatest limit, the last limit is used
  int new_positioner_index = (int) (std::find_if(limit_vec->begin(), limit_vec->end(), [=](int x) { return x > winsize.a; }) - limit_vec->begin() - 1);
	
	// If change in index, deactivate current elements, activate new ones
	if (new_positioner_index != cur_positioner_index) {
		if (cur_positioner_index != -1) {
      for (auto el : ellist_vec->at(cur_positioner_index)) {
        el->deactivate();
      }
		}
    for (auto el : ellist_vec->at(new_positioner_index)) {
			el->activate();
    }
	}
	cur_positioner_index = new_positioner_index;
	
	// Update position of self
  cur_positioner = &(*psnr_vec)[cur_positioner_index];
	rct = cur_positioner->refresh(winsize);
	
	allowDrag = cur_positioner->isDraggable();
	
	// Update element positions
  for (auto el : ellist_vec->at(cur_positioner_index)) {
		el->_updatePosition(rct.size);
  }

  // Update BG rect
  bgrect->setPos({0,0});
  bgrect->setSz(rct.size);

  // Allow subclasses to position custom elements
  updatePosition__uiview(winsize);
}

bool W::UIView::initialize(const std::string &viewname) {
	using std::string;
	
	// Load file
	lua_State *L = luaL_newstate();
	bool loadError = luaL_loadfile(L, viewname.c_str());
	if (loadError) {
		W::log
			<< "UIView: couldn't open file '" << viewname << "' - error was: "
			<< lua_tostring(L, -1) << std::endl;
		lua_close(L);
		return false;
	}
	bool runError = lua_pcall(L, 0, 0, 0);
	if (runError) {
		W::log
			<< "UIView: couldn't execute file '" << viewname << "' - error was: "
			<< lua_tostring(L, -1) << std::endl;
		lua_close(L);
		return false;
	}
	
	// Get portrait & landscape LuaObjs
	lua_getglobal(L, "portrait");  LuaObj portraitObj(L);
	lua_getglobal(L, "landscape"); LuaObj landscapeObj(L);
	lua_close(L);
	if (!portraitObj.isTable() && !landscapeObj.isTable()) {
		W::log
			<< "UIView: in '" << viewname
			<< "', couldn't find either 'portrait' or 'landscape' layout object(s)" << std::endl;
		return false;
	}
	
	orientation_check =
    portraitObj.isTable() && portraitObj.descendants().size() > 0 &&
    landscapeObj.isTable() && landscapeObj.descendants().size() > 0;
	
	try {
		if (orientation_check) {
			// Add all landscape descendants as positioners
			LuaObj::_descendantmap d1 = landscapeObj.descendants();
			if (d1.size() == 0) {
				W::log <<
					"UIView: '" << viewname << "': Landscape: error: no layouts found" << std::endl;
				return false;
			}
      for (auto l : d1) {
				addPositioner(l.first, &l.second, O_LANDSCAPE);
				addElements(l.first, &l.second["elements"], O_LANDSCAPE);
			}
			// Add all portrait descendants as positioners
			LuaObj::_descendantmap d2 = portraitObj.descendants();
			if (d2.size() == 0) {
				W:log <<
					"UIView: '" << viewname << "': Portrait: error: no layouts found" << std::endl;
				return false;
			}
      for (auto l : d2) {
				const std::string &descName = l.first;
				LuaObj &descendant = l.second;
				addPositioner(descName, &descendant, O_PORTRAIT);
				addElements(descName, &descendant["elements"], O_PORTRAIT);
			}
		}
		else {
			// Add all descendants from whichever orientation obj exists as positioners
			LuaObj *ob;
			if (landscapeObj.isTable()) {
				orientation = O_LANDSCAPE;
				ob = &landscapeObj;
			}
			else {
				orientation = O_PORTRAIT;
				ob = &portraitObj;
			}
			LuaObj::_descendantmap d = ob->descendants();
			if (d.size() == 0) {
				W::log
					<< "UIView: '" << viewname << "': "
					<< (orientation == O_LANDSCAPE ? "Landscape" : "Portrait")
					<< ": error: no layouts found" << std::endl;
				return false;
			}
      for (auto l : d) {
        const std::string &descName = l.first;
				LuaObj &descendant = l.second;
				addPositioner(descName, &descendant, orientation);
				addElements(descName, &descendant["elements"], orientation);
			}
		}
	}
	catch (Exception &exc) {
		// Creation of Positioner will throw if LuaObj is improperly formed
		W::log << "UIView: '" << viewname << "': " << exc.what() << std::endl;
		return false;
	}
	
	return true;
}

void W::UIView::addPositioner(const std::string &limit, LuaObj *luaObj, W::UIView::orientation_enum _or) {
	using std::string;
	// Check limit is valid
	int lim;
	if (!strToT<int>(lim, limit))
		throw Exception(
			string(_or == O_LANDSCAPE ? "Landscape: " : "Portrait: ") +
			string("Limit '") + limit + string("' is invalid: ") +
			string("must be numeric")
		);
	// Create positioner
	Positioner pos;
	try {
		pos = Positioner(luaObj);
		// Add to relevant vectors if sucessful
		if (_or == O_LANDSCAPE) {
			landscape_positioning_limits.push_back(lim);
			landscape_positioners.push_back(pos);
		}
		else {
			portrait_positioning_limits.push_back(lim);
			portrait_positioners.push_back(pos);
		}
	}
	catch (Exception &exc) {
		throw Exception(
			string(_or == O_LANDSCAPE ? "Landscape: " : "Portrait: ") +
			string("Limit '" + limit + "': error: ") + exc.what()
		);
	}
}
void W::UIView::addElements(const std::string &limit, LuaObj *luaObj, W::UIView::orientation_enum _or) {
	// Add new element list to appropriate vector
	std::vector<element_list> &l = (_or == O_LANDSCAPE ? landscape_elements : portrait_elements);
	l.push_back(element_list());
	element_list &elvec = l.back();

	// For each descendant, create an element
  for (auto l : luaObj->descendants()) {
		const std::string &elName = l.first;
		LuaObj &elObj = l.second;
		elvec.push_back(
			createElement(limit, elName, &elObj, _or)
		);
	}
}
W::UIElement* W::UIView::createElement(const std::string &limit, const std::string &name, LuaObj *_luaObj, W::UIView::orientation_enum _or) {
	using std::string;
	
	Positioner pos;
	LuaObj &luaObj = *_luaObj;
	
	try {
		// Create positioner
		pos = Positioner(_luaObj);
		
		// Get element properties
		LuaObj &typeObj = luaObj["type"];
		if (!typeObj.isString())
			throw Exception("No type specified for element");
		string eltype = typeObj.str_value();
		
		// Create element
		if (eltype == "button")
			return new Button(name, pos, this);
		else
			throw Exception(
				string("Invalid UIElement type '") + eltype + string("'")
			);
	}
	catch (Exception &exc) {
		throw Exception(
			string(_or == O_LANDSCAPE ? "Landscape: " : "Portrait: ") +
			string("Limit '" + limit + "': ") +
			string("Element: '" + name + "': ") +
			string("error: ") + exc.what()
		);
	}
}
