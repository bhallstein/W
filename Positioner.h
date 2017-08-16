/*
 * Positioner.h - The second incarnation of JenniferAniston
 *
 */

#ifndef W_Positioner
#define W_Positioner

#include "types.h"
#include "lua.hpp"
#include "LuaHelper/LuaState.hpp"
#include "LuaHelper/LHObj.h"

namespace W {
	
	namespace Corner {
		enum T {
			TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT
		};
	}
	namespace PosType {
		enum T {
			FIXED, PROPORTIONAL
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
		Positioner(LHObj &);
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
