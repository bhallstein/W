#ifndef __W__MappedObj
#define __W__MappedObj

#include "types.h"
#include "EventResponder.h"
#include <vector>

namespace W {
	
	class MappedObj : public EventResponder {
	public:
		position pos;
		std::vector<rect> plan;
		
		bool overlapsWith(int x, int y);
		
		virtual void _cplusplusisbollocks() { }
	};
	
}

#endif
