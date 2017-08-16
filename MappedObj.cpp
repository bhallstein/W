#include "MappedObj.h"

bool W::MappedObj::overlapsWith(int x, int y) {
	for (int i=0; i < plan.size(); i++) {
		W::rect r = plan[i];
		if (x >= pos.x + r.pos.x && x < pos.x + r.pos.x + r.sz.width
			&& y >= pos.y + r.pos.y && y < pos.y + r.pos.y + r.sz.height)
			return true;
	}
	return false;
}
