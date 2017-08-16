/* The second incarnation of JenniferAniston */

#ifndef W_Positioner
#define W_Positioner

#include "types.h"
#include "lua.hpp"
#include "LuaState.hpp"
#include "LHObj.h"

namespace W {

	enum corner { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };
	enum pos_type { PFIXED, PPROPORTIONAL };
	
	class Positioner {
	public:
		Positioner(
			corner,
			pos_type _pos_method_x, pos_type _pos_method_y,
			pos_type _sizing_method_x, pos_type _sizing_method_y,
			float _x, float _y, float _w, float _h
		);
		Positioner(LHObj &);
		W::rect& refresh(const size &container_size);

		void nudge(const position &delta);
		bool isDraggable();
		
	private:
		corner fixed_corner;
		pos_type pos_method_x, pos_method_y;
		pos_type sizing_method_x, sizing_method_y;
		float corner_x, corner_y, w, h;
		bool draggable;
		
		rect _p;
		
	};
	
}

#endif
