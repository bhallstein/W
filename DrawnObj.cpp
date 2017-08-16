#include "DrawnObj.h"


W::DrawnRect::DrawnRect(const W::rect &_r, const Colour &_col) :
	DrawnObj(RECT),
	r(_r), col(_col)
{
	// hai recty
}


W::DrawnText::DrawnText(const char *_text, const Colour &_col) :
	DrawnObj(TEXT),
	text(_text), col(_col)
{
	// hai texty
}

