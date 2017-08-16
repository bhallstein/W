#include "MappedObj.h"

bool W::MappedObj::overlapsWith(const position &p) {
	for (std::vector<rect>::iterator it = plan.begin(); it < plan.end(); it++) {
		W::rect &r = *it;
		if (p.x >= pos.x + r.pos.x && p.x < pos.x + r.pos.x + r.sz.width
			&& p.y >= pos.y + r.pos.y && p.y < pos.y + r.pos.y + r.sz.height)
			return true;
	}
	return false;
}
