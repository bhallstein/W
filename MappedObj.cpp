#include "MappedObj.h"

bool W::MappedObj::overlapsWith(const position &p) {
	for (int i=0; i < plan.size(); i++) {
		W::rect r = plan[i];
		if (p.x >= pos.x + r.pos.x && p.x < pos.x + r.pos.x + r.sz.width
			&& p.y >= pos.y + r.pos.y && p.y < pos.y + r.pos.y + r.sz.height)
			return true;
	}
	return false;
}
