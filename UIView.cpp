#include "W.h"

#include "UIView.h"
#include "Callback.h"
#include "Messenger.h"
#include "Positioner.h"
#include "Window.h"

#include "lua.hpp"
#include "LuaState.hpp"
#include "LHObj.h"

#include <iostream>

W::UIView::UIView(const std::string &viewname) :
	View(NULL),
	dragloop(false),
	cur_positioning_index(-1)
{
	createEvTypeMap();	// Make event type translation map for this uiview
	
	// Initialize from Lua
	using std::string;
	if (!initialize(viewname))
		throw Exception(
			string("Error creating UIView '") + string(viewname) +
			string("' - see log file for further details")
		);
	updatePosition(_window->getDimensions());
	
	rect r = {
		position(),
		rct.sz
	};
	bgDRect = new DrawnRect(this, r, W::Colour::TransparentBlack);
	addDO(bgDRect, 0);
}

W::UIView::~UIView()
{
	removeDO(bgDRect);
}

void W::UIView::processMouseEvent(Event *ev) {
	if (dragloop) {
		if (ev->type == EventType::MOUSEMOVE) {
			cur_positioner->nudge(ev->pos - drag_initial);
			_updatePosition(_window->getDimensions());
		}
		else if (ev->type == EventType::LEFTMOUSEUP) {
			Messenger::relinquishPrivilegedEventResponderStatus(this);
			dragloop = false;
		}
		return;
	}
	
	// Resubmit event, translated for UIView’s members
	Event ev2(evTypeMap[ev->type], ev->pos);
	
	bool wasDispatchedToElement = W::Messenger::dispatchPositionalEvent(&ev2);
	
	// If no element was found & allowDrag is true, perform dragloopery
	if (!wasDispatchedToElement && allowDrag && ev->type == EventType::LEFTMOUSEDOWN && Messenger::requestPrivilegedEventResponderStatus(Callback(&View::receiveEvent, (View*)this)))
		drag_initial = ev->pos, dragloop = true;
}

//void W::UIView::draw() {
//	// Draw background - subclass may override (otherwise, draws nothing)
//	drawCustomBackground();
//	// Draw elements
//	element_list *elvec = &(orientation == O_LANDSCAPE ?
//		landscape_elements :
//		portrait_elements)[cur_positioning_index];
//	for (element_list::iterator it = elvec->begin(); it < elvec->end(); it++) {
//		const position &pos = (*it)->pos;
//		const size &sz = (*it)->plan[0].sz;
//		drawRect(pos.x, pos.y, sz.width, sz.height, Colour::Black);
//	}
//}

void W::UIView::updatePosition(const size &winsize) {
	if (orientation_check)
		orientation = (winsize.width >= winsize.height ? O_LANDSCAPE : O_PORTRAIT);
	
	std::vector<int> *limit_vec;
	positioner_list *psnr_vec;
	std::vector<element_list> *ellist_vec;
	element_list *ellist;
	
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
	int new_positioning_index = -1;
	for (std::vector<int>::iterator it = limit_vec->begin(); it < limit_vec->end(); it++, new_positioning_index++)
		if (*it > winsize.width)
			break;
	
	// If change in index, deactivate current elements, activate new ones
	if (new_positioning_index != cur_positioning_index) {
		if (cur_positioning_index != -1) {
			ellist = &ellist_vec->at(cur_positioning_index);
			for (element_list::iterator it = ellist->begin(); it < ellist->end(); it++)
				(*it)->deactivate();
		}
		ellist = &ellist_vec->at(new_positioning_index);
		for (element_list::iterator it = ellist->begin(); it < ellist->end(); it++)
			(*it)->activate();
	}
	cur_positioning_index = new_positioning_index;
//	std::cout << "uiv: positioning index: " << cur_positioning_index << "\n";
	
	// Update position of self
	cur_positioner = psnr_vec->at(cur_positioning_index);
	rct = cur_positioner->refresh(winsize);
//	std::cout << " - " << rct.pos.x << "," << rct.pos.y << " / " << rct.sz.width << "x" << rct.sz.height << "\n";
	
	allowDrag = cur_positioner->isDraggable();
	
	// Update element positions
	ellist = &ellist_vec->at(cur_positioning_index);
	for (element_list::iterator it = ellist->begin(); it < ellist->end(); it++)
		(*it)->_updatePosition(rct.sz);
}

bool W::UIView::initialize(const std::string &viewname) {
	using std::string;
	
	LuaState ls;
	if (!ls.loadFile(viewname.c_str())) {
		W::log << "UIView: couldn’t open file '" << viewname << "'" << std::endl;
		return false;
	}
	
	// Get portrait & landscape LHObjs
	lua_State *L = ls.state();
	lua_getglobal(L, "portrait");  LHObj portraitObj = lhGet(L);
	lua_getglobal(L, "landscape"); LHObj landscapeObj = lhGet(L);
	if (!portraitObj.isTable() && !landscapeObj.isTable()) {
		W::log
			<< "UIView: in '" << viewname
			<< "', couldn't find 'portrait' or 'landscape' layout object(s)" << std::endl;
		return false;
	}
	
	orientation_check = (portraitObj.isTable() && landscapeObj.isTable());
	
	try {
		if (orientation_check) {
			// Add all landscape descendants as positioners
			LHObj::_descendantmap &d1 = landscapeObj.descendants;
			if (d1.size() == 0) {
				W::log <<
					"UIView: '" << viewname << "': Landscape: error: no layouts found" << std::endl;
				return false;
			}
			for (LHObj::_descendantmap::iterator it = d1.begin(); it != d1.end(); it++) {
				addPositioner(it->first, it->second, O_LANDSCAPE);
				addElements(it->first, it->second["elements"], O_LANDSCAPE);
			}
			// Add all portrait descendants as positioners
			LHObj::_descendantmap &d2 = portraitObj.descendants;
			if (d2.size() == 0) {
				W:log <<
					"UIView: '" << viewname << "': Portrait: error: no layouts found" << std::endl;
				return false;
			}
			for (LHObj::_descendantmap::iterator it = d2.begin(); it != d2.end(); it++) {
				const std::string &descName = it->first;
				LHObj &descendant = it->second;
				addPositioner(descName, descendant, O_PORTRAIT);
				addElements(descName, descendant["elements"], O_PORTRAIT);
			}
		}
		else {
			// Add all descendants from whichever orientation obj exists as positioners
			LHObj *ob;
			if (landscapeObj.isTable()) {
				orientation = O_LANDSCAPE;
				ob = &landscapeObj;
			}
			else {
				orientation = O_PORTRAIT;
				ob = &portraitObj;
			}
			LHObj::_descendantmap &d = ob->descendants;
			if (d.size() == 0) {
				W::log
					<< "UIView: '" << viewname << "': "
					<< (orientation == O_LANDSCAPE ? "Landscape" : "Portrait")
					<< ": error: no layouts found" << std::endl;
				return false;
			}
			for (LHObj::_descendantmap::iterator it = d.begin(); it != d.end(); it++) {
				const std::string &descName = it->first;
				LHObj &descendant = it->second;
				addPositioner(descName, descendant, orientation);
				addElements(descName, descendant["elements"], orientation);
			}
		}
	}
	catch (Exception &exc) {
		// Creation of Positioner will throw if LHObj is improperly formed
		W::log << "UIView: '" << viewname << "': " << exc.what() << std::endl;
		return false;
	}
	
	return true;
}

void W::UIView::addPositioner(const std::string &limit, W::LHObj &lhobj, W::UIView::orientation_enum _or) {
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
	Positioner *pos;
	try {
		pos = new Positioner(lhobj);
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
void W::UIView::addElements(const std::string &limit, W::LHObj &lhobj, W::UIView::orientation_enum _or) {
	// Add new element list to appropriate vector
	std::vector<element_list> &l = (_or == O_LANDSCAPE ? landscape_elements : portrait_elements);
	l.push_back(element_list());
	element_list &elvec = l.back();
	// For each descendant, create an element
	LHObj::_descendantmap &d = lhobj.descendants;
	for (LHObj::_descendantmap::iterator it = d.begin(); it != d.end(); it++) {
		const std::string &elName = it->first;
		LHObj &elObj = it->second;
		elvec.push_back(
			createElement(limit, elName, elObj, _or)
		);
	}
}
W::UIElement* W::UIView::createElement(const std::string &limit, const std::string &name, W::LHObj &lhobj, W::UIView::orientation_enum _or) {
	using std::string;
	Positioner *pos;
	try {
		// Create positioner
		pos = new Positioner(lhobj);
		
		// Get element properties
		LHObj &typeObj = lhobj["type"];
		if (!typeObj.isString())
			throw Exception("No type specified for element");
		string eltype = typeObj.str_value;
		
		// Create element
		if (eltype == "button")
			return new Button(this, name, pos, evTypeMap);
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

void W::UIView::createEvTypeMap() {
	using namespace EventType;
	evTypeMap[MOUSEMOVE]      = Event::registerType();
	evTypeMap[LEFTMOUSEDOWN]  = Event::registerType();
	evTypeMap[LEFTMOUSEUP]    = Event::registerType();
	evTypeMap[RIGHTMOUSEDOWN] = Event::registerType();
	evTypeMap[RIGHTMOUSEUP]   = Event::registerType();
}
