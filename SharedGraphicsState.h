/*
 * SharedGraphicState.h
 *
 * An SGS represents everything to be drawn by W.
 * W maintains an SGS to be shared by update and drawing threads.
 *
 */

#ifndef __W__SharedGraphicState
#define __W__SharedGraphicState

#include <vector>

#include "types.h"
#include "DrawnObj.h"

namespace W {
	
	struct ViewAndDrawables {
		rect vInfo;					// Represents view position and size
		std::vector<DrawnObj> objs;	// List of objs to draw, in order
	};
	
	typedef std::vector<ViewAndDrawables> SGS;
	
}

#endif
