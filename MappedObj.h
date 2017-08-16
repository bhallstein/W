#ifndef __W__MappedObj
#define __W__MappedObj

#include "types.h"

namespace W {
	
	class MappedObj {
	public:
		rect rct;
		void setPos(const W::position &_pos) { rct.pos = _pos; }
		void setSz(const W::size &_sz) { rct.sz = _sz; }
		bool overlapsWith(const W::position &p) {
			float x = rct.pos.x + rct.pos.a;
			float y = rct.pos.y + rct.pos.b;
			return (p.x >= x && p.x < x + rct.sz.width && p.y >= y && p.y < y + rct.sz.height);
		}
	};
	
}

#endif
