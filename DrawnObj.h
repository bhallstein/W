/*
 * DrawnObj.h
 *
 */

#ifndef __W__DrawableObj
#define __W__DrawableObj

#include "types.h"
#include "Colour.h"

namespace W {
	
	class DrawnObj {
	public:
		enum DOType {
			RECT,
			TEXT
		};
		DrawnObj(DOType _type) : type(_type) { }
		DOType type;
	};

	
	class DrawnRect : public DrawnObj {
	public:
		DrawnRect(const W::rect &, const W::Colour &);
		rect r;
		W::Colour col;
	};

	class DrawnText : public DrawnObj {
	public:
		DrawnText(const char *, const W::Colour &);
		std::string text;
		W::Colour col;
	};
	
}

#endif
