/* The second incarnation of JenniferAniston */

#ifndef W_Positioner
#define W_Positioner

#include "types.h"

namespace W {

	enum corner { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };
	enum pos_type { PFIXED, PPROPORTIONAL };
	
	class Positioner {
	public:
		Positioner(
			corner _fixed_corner, pos_type _positioning_method, pos_type _sizing_method,
			float _corner_x, float _corner_y, float _w, float _h
		) :
			fixed_corner(_fixed_corner), positioning_method(_positioning_method), sizing_method(_sizing_method),
			corner_x(_corner_x), corner_y(_corner_y), w(_w), h(_h)
		{
			
		}
		
		rect* refresh(size *winsize) {
			int Ww = winsize->width;
			int Wh = winsize->height;
			
			if (sizing_method == PFIXED)
				_p.sz.width = w, _p.sz.height = h;
			else
				_p.sz.width = w * Ww, _p.sz.height = h * Wh;
			
			// Set x
			if (fixed_corner == TOP_LEFT || fixed_corner == BOTTOM_LEFT)
				_p.pos.x = (positioning_method == PFIXED ? corner_x : corner_x * Ww);
			else {
				int rval = Ww - (positioning_method == PFIXED ? corner_x : corner_x * Ww);
				_p.pos.x = rval - _p.sz.width;
			}
			// Set
			if (fixed_corner == TOP_LEFT || fixed_corner == TOP_RIGHT)
				_p.pos.y = (positioning_method == PFIXED ? corner_y : corner_y * Wh);
			else {
				int bval = Wh - (positioning_method == PFIXED ? corner_y : corner_y * Wh);
				_p.pos.y = bval - _p.sz.height;
			}

			return &_p;
		}
		
		void setCorner(int _x, int _y) {
			corner_x = _x, corner_y = _y;
		}

	private:
		corner fixed_corner;
		pos_type positioning_method;
		pos_type sizing_method;
		float corner_x, corner_y, w, h;

		rect _p;
	};
	
}

#endif
