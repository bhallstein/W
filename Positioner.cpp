/*
 * W - a tiny 2D game development library
 *
 * ==================
 *  Positioner.cpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Positioner.h"
#include <vector>

#include "LuaObj.h"

namespace W {
	bool checkDistStr(const std::string &s) {
		int n = (int) s.length();
		if (n == 0) return false;
		
		const char *c = s.c_str();
		
		if (c[n-1] == '%') {
			if (n < 2) return false;
			for (int i=0; i < n-1; i++)
				if (!W::isNum(c[i]))
					return false;
			return true;
		}
		
		for (int i=0; i < n; i++)
			if (!W::isNum(c[i]))
				return false;
		return true;
	}
	bool strToCorner(const std::string &s, Corner::T &c) {
		if (s == "top left")          c = Corner::TopLeft;
		else if (s == "top right")    c = Corner::TopRight;
		else if (s == "bottom left")  c = Corner::BottomLeft;
		else if (s == "bottom right") c = Corner::BottomRight;
		else return false;
		return true;
	}
}

			  
W::Positioner::Positioner(
	Corner::T _fixed_corner,
	PosType::T _pos_method_x, PosType::T _pos_method_y,
	PosType::T _sizing_method_x, PosType::T _sizing_method_y,
	float _corner_x, float _corner_y, float _w, float _h
) :
	fixed_corner(_fixed_corner),
	pos_method_x(_pos_method_x), pos_method_y(_pos_method_y),
	sizing_method_x(_sizing_method_x), sizing_method_y(_sizing_method_y),
	corner_x(_corner_x), corner_y(_corner_y), w(_w), h(_h),
	draggable(false)
{
	//
}

W::Positioner::Positioner(LuaObj *_l)
{
	// Construct from recursive lua object
	using std::string;
	
	LuaObj &l = *_l;

	bool error = false;
	std::vector<string> errmsgs;
	errmsgs.push_back("Error initializing positioner:");
	std::vector<string> sComponents;

	// Get size params
	LuaObj &l_size = l["size"];
	if (l_size.type != LuaObj::ValueType::STRING) {
		error = true;
		errmsgs.push_back("'size' property not found");
	}
	else {
		string &s = l_size.str_value;
		strSplit(s, sComponents, ',');
		if (sComponents.size() != 2 || !checkDistStr(sComponents[0]) || !checkDistStr(sComponents[1])) {
			error = true;
			errmsgs.push_back(
				"'size' property incorrectly formed: must be "
				"'dist,dist', where dist has the form '15' (for absolute pixel values) "
				"or '50%' (for proportional values)"
			);
		}
		else {
			string &c1 = sComponents[0], &c2 = sComponents[1];
			sizing_method_x = (c1[c1.size()-1] == '%' ? PosType::Proportional : PosType::Fixed);
			sizing_method_y = (c2[c2.size()-1] == '%' ? PosType::Proportional : PosType::Fixed);
			strToT<float>(w, c1); if (sizing_method_x == PosType::Proportional) w /= 100.0;
			strToT<float>(h, c2); if (sizing_method_y == PosType::Proportional) h /= 100.0;
		}
	}
	
	// Get position params
	LuaObj &l_pos = l["position"];
	if (l_pos.type != LuaObj::ValueType::STRING) {
		error = true;
		errmsgs.push_back("'position' property not found");
	}
	else {
		string &s = l_pos.str_value;
		strSplit(s, sComponents, ',');
		if (sComponents.size() == 2 && checkDistStr(sComponents[0]) && checkDistStr(sComponents[1])) {
			string &c1 = sComponents[0], &c2 = sComponents[1];
			pos_method_x = (c1[c1.size()-1] == '%' ? PosType::Proportional : PosType::Fixed);
			pos_method_y = (c2[c2.size()-1] == '%' ? PosType::Proportional : PosType::Fixed);
			strToT<float>(corner_x, c1); if (pos_method_x == PosType::Proportional) corner_x /= 100.0;
			strToT<float>(corner_y, c2); if (pos_method_y == PosType::Proportional) corner_y /= 100.0;
		}
		else {
			error = true;
			errmsgs.push_back(
				"'position' property incorrectly formed: must be "
				"'dist,dist', where dist has the form '15' (for absolute pixel values) "
				"or '50%' (for proportional values)"
			);
		}
	}
	
	// Get positioning corner
	LuaObj &l_corner = l["positioning_corner"];
	downCase(l_corner.str_value);
	if (l_corner.type == LuaObj::ValueType::NIL)
		fixed_corner = Corner::TopLeft;
	else if (!strToCorner(l_corner.str_value, fixed_corner)) {
		error = true;
		errmsgs.push_back(
			"'positioning_corner', if present, must be formatted as per: "
			"'[top/bottom] [left/right]' (if not present, will be set to "
			"'top left')"
		);
	}
	
	// Get draggability
	LuaObj &l_drag = l["draggable"];
	if (l_drag.type == LuaObj::ValueType::NIL)
		draggable = false;
	else if (l_drag.type == LuaObj::ValueType::BOOL)
		draggable = l_drag.bool_value;
	else {
		error = true;
		errmsgs.push_back(
			"'draggable', if present, must be either true or false"
		);
	}
	
	if (error) {
		std::string errmsg;
		implode(errmsgs, errmsg, "\n  ");
		errmsg = string("  ") + errmsg;
		throw Exception(errmsg);
	}
}

W::iRect& W::Positioner::refresh(const v2f &container_size) {
	const int &Ww = container_size.a;
	const int &Wh = container_size.b;
	
	if (sizing_method_x == PosType::Fixed) _p.size.a = w;
	else _p.size.a = w * Ww;
	
	if (sizing_method_y == PosType::Fixed) _p.size.b = h;
	else _p.size.b = h * Wh;
	
	// Set x
	if (fixed_corner == Corner::TopLeft || fixed_corner == Corner::BottomLeft)
		_p.position.a = (pos_method_x == PosType::Fixed ? corner_x : corner_x * Ww);
	else {
		int rval = Ww - (pos_method_x == PosType::Fixed ? corner_x : corner_x * Ww);
		_p.position.a = rval - _p.size.a;
	}
	// Set y
	if (fixed_corner == Corner::TopLeft || fixed_corner == Corner::TopRight)
		_p.position.b = (pos_method_y == PosType::Fixed ? corner_y : corner_y * Wh);
	else {
		int bval = Wh - (pos_method_y == PosType::Fixed ? corner_y : corner_y * Wh);
		_p.position.b = bval - _p.size.b;
	}
	
	return _p;
}

void W::Positioner::nudge(const v2f &delta) {
	corner_x += (fixed_corner == Corner::TopLeft || fixed_corner == Corner::BottomLeft ? delta.a : -delta.a);
	corner_y += (fixed_corner == Corner::TopLeft || fixed_corner == Corner::TopRight   ? delta.b : -delta.b);
}


/* Initialize helper positioners */

namespace W {
	const Positioner *Positioner::WholeAreaPositioner = new Positioner(
		Corner::TopLeft,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 1, 1
	);
	const Positioner *Positioner::LeftHalfPositioner = new Positioner(
		Corner::TopLeft,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 0.5, 1
	);
	const Positioner *Positioner::RightHalfPositioner = new Positioner(
		Corner::TopRight,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 0.5, 1
	);
	const Positioner *Positioner::TopHalfPositioner = new Positioner(
		Corner::TopLeft,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 1, 0.5
	);
	const Positioner *Positioner::BottomHalfPositioner = new Positioner(
		Corner::BottomLeft,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 1, 0.5
	);
	const Positioner *Positioner::TopLeftQuarterPositioner = new Positioner(
		Corner::TopLeft,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 0.5, 0.5
	);
	const Positioner *Positioner::TopRightQuarterPositioner = new Positioner(
		Corner::TopRight,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 0.5, 0.5
	);
	const Positioner *Positioner::BottomLeftQuarterPositioner = new Positioner(
		Corner::BottomLeft,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 0.5, 0.5
	);
	const Positioner *Positioner::BottomRightQuarterPositioner = new Positioner(
		Corner::BottomRight,
		PosType::Fixed, PosType::Fixed,
		PosType::Proportional, PosType::Proportional,
		0, 0, 0.5, 0.5
	);
	
}
