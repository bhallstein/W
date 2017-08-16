/*
 * W - a tiny 2D game development library
 *
 * ================
 *  Positioner.h
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef W_Positioner
#define W_Positioner

#include "types.h"

class LuaObj;

namespace W {
	
	namespace Corner {
		enum T {
			TopLeft, TopRight, BottomLeft, BottomRight
		};
	}
	namespace PosType {
		enum T {
			Fixed, Proportional
		};
	}
	
	class Positioner {
	public:
		Positioner(
			Corner::T,
			PosType::T _pos_method_x, PosType::T _pos_method_y,
			PosType::T _sizing_method_x, PosType::T _sizing_method_y,
			float _x, float _y, float _w, float _h
		);
		Positioner(LuaObj *);
		W::rect& refresh(const size &container_size);

		void nudge(const position &delta);
		bool isDraggable() { return draggable; }
		
	private:
		Corner::T fixed_corner;
		PosType::T pos_method_x, pos_method_y;
		PosType::T sizing_method_x, sizing_method_y;
		float corner_x, corner_y, w, h;
		bool draggable;
		
		rect _p;
		
	};
	
}

#endif
