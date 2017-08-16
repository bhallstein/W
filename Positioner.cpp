#include "Positioner.h"
#include <vector>

bool checkDistStr(const std::string &s) {
	const char *c = s.c_str();
	for (int i=0, n = (int)s.size(); i < n; i++)
		if (i == n-1) {
			if (!W::isNum(c[i]) || c[i] != '%' || i == 0)
				return false;
		}
		else if (!W::isNum(c[i]))
			return false;
	return true;
}
bool strToCorner(const std::string &s, W::corner &c) {
	if (s == "top left" || s == "TOP LEFT")              c = W::TOP_LEFT;
	else if (s == "top right" || s == "TOP RIGHT")       c = W::TOP_RIGHT;
	else if (s == "bottom left" || s == "BOTTOM LEFT")   c = W::BOTTOM_LEFT;
	else if (s == "bottom right" || s == "BOTTOM RIGHT") c = W::BOTTOM_RIGHT;
	else return false;
	return true;
}

			  
W::Positioner::Positioner(
	corner _fixed_corner,
	pos_type _pos_method_x, pos_type _pos_method_y,
	pos_type _sizing_method_x, pos_type _sizing_method_y,
	float _corner_x, float _corner_y, float _w, float _h
) :
	fixed_corner(_fixed_corner),
	pos_method_x(_pos_method_x), pos_method_y(_pos_method_y),
	sizing_method_x(_sizing_method_x), sizing_method_y(_sizing_method_y),
	corner_x(_corner_x), corner_y(_corner_y), w(_w), h(_h)
{
	//
}

W::Positioner::Positioner(LHObj &l)
{
	// Construct from recursive lua object
	using std::string;

	bool error = false;
	string errmsg = "Error initializing Positioner: ";
	std::vector<string> sComponents;

	// Get size params
	LHObj &l_size = l["size"];
	if (l_size.type != LHValueType::STRING) {
		error = true;
		errmsg += string("\"size\" property not found\n");
	}
	else {
		string &s = l_size.str_value;
		strSplit(s, sComponents, ',');
		if (sComponents.size() != 2 || !checkDistStr(sComponents[0]) || !checkDistStr(sComponents[1])) {
			error = true;
			errmsg += string(
				"\"size\" property incorrectly formed: must be "
				"\"dist,dist\", where dist has the form \"15\" (for absolute pixel values) "
				"or \"50%\" (for proportional values)"
			);
		}
		else {
			string &c1 = sComponents[0], &c2 = sComponents[1];
			sizing_method_x = (c1[c1.size()-1] == '%' ? PPROPORTIONAL : PFIXED);
			sizing_method_y = (c2[c2.size()-1] == '%' ? PPROPORTIONAL : PFIXED);
			strToT<float>(w, c1);
			strToT<float>(h, c2);
		}
	}
	
	// Get position params
	LHObj &l_pos = l["position"];
	if (l_pos.type != LHValueType::STRING) {
		error = true;
		errmsg += string("\"position\" property not found\n");
	}
	else {
		string &s = l_pos.str_value;
		strSplit(s, sComponents, ',');
		if (sComponents.size() != 2 || !checkDistStr(sComponents[0]) || !checkDistStr(sComponents[1])) {
			error = true;
			errmsg += string(
				"\"position\" property incorrectly formed: must be "
				"\"dist,dist\", where dist has the form \"15\" (for absolute pixel values) "
				"or \"50%\" (for proportional values)"
			);
		}
		else {
			string &c1 = sComponents[0], &c2 = sComponents[1];
			pos_method_x = (c1[c1.size()-1] == '%' ? PPROPORTIONAL : PFIXED);
			pos_method_y = (c2[c2.size()-1] == '%' ? PPROPORTIONAL : PFIXED);
			strToT<float>(corner_x, c1);
			strToT<float>(corner_y, c2);
		}
	}
	
	// Get positioning corner
	LHObj &l_corner = l["positioning_corner"];
	if (l_corner.type == LHValueType::NIL)
		fixed_corner = TOP_LEFT;
	else if (!strToCorner(l_corner.str_value, fixed_corner)) {
		error = true;
		errmsg += string(
			"\"positioning_corner\", if present, must be formatted as per: "
			"\"[top/bottom] [left/right]\" (if not present, will be set to "
			"\"top right\")"
		);
	}
	
	if (error) throw Exception(errmsg);
}

W::rect& W::Positioner::refresh(const size &container_size) {
	const int &Ww = container_size.width;
	const int &Wh = container_size.height;
	
	if (sizing_method_x == PFIXED) _p.sz.width = w;
	else _p.sz.width = w * Ww;
	
	if (sizing_method_y == PFIXED) _p.sz.height = h;
	else _p.sz.height = h * Wh;
	
	// Set x
	if (fixed_corner == TOP_LEFT || fixed_corner == BOTTOM_LEFT)
		_p.pos.x = (pos_method_x == PFIXED ? corner_x : corner_x * Ww);
	else {
		int rval = Ww - (pos_method_x == PFIXED ? corner_x : corner_x * Ww);
		_p.pos.x = rval - _p.sz.width;
	}
	// Set y
	if (fixed_corner == TOP_LEFT || fixed_corner == TOP_RIGHT)
		_p.pos.y = (pos_method_y == PFIXED ? corner_y : corner_y * Wh);
	else {
		int bval = Wh - (pos_method_y == PFIXED ? corner_y : corner_y * Wh);
		_p.pos.y = bval - _p.sz.height;
	}
	
	return _p;
}

void W::Positioner::setCorner(int _x, int _y) {
	corner_x = _x, corner_y = _y;
}
