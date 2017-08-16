/*
 * oglDrawHelpers.h
 *
 */

#ifndef __W_GenericRetro
#define __W_GenericRetro

#include <map>
#include "types.h"

namespace W {
	
	extern std::map<char,std::vector<rect>> GenericRetro;
	void initGenericRetro();
	
	struct _grInit;
	extern _grInit *_grInitializer;

}

#endif
