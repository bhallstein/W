#include "DrawnObj.h"

W::DrawnObj::DrawnObj(View *_view, DOType _type, const rect &_posn) :
	type(_type), posn(_posn), new_posn(_posn), view(_view)
{
	// hai objy
}
void W::DrawnObj::_updateValues() {
	posn = new_posn;
	updateValues();
}

W::DrawnRect::DrawnRect(View *_view, const rect &_r, const Colour &_col, float _rot) :
	DrawnObj(_view, RECT, _r),
	col(_col), new_col(_col), rot(_rot), new_rot(_rot)
{
	// hai recty
}


W::DrawnText::DrawnText(View *_view, const rect &_r, const char *_text, const Colour &_col) :
	DrawnObj(_view, TEXT, _r),
	text(_text), new_text(_text), col(_col), new_col(_col)
{
	// hai texty
}


W::DrawnImage::DrawnImage(View *_view, const rect &_r, Texture *_texture, float _opac, float _rot) :
	DrawnObj(_view, IMAGE, _r),
	texture(_texture), rot(_rot), new_rot(_rot), opac(_opac), new_opac(_opac)
{
	// hai spritey
	texture->incrementUsageCount();
	use_placeholder_texture = !texture->_uploaded;
}
W::DrawnImage::~DrawnImage()
{
	texture->decrementUsageCount();
}
W::Texture* W::DrawnImage::getTex() {
	if (use_placeholder_texture) {
		if (texture->_uploaded) {
			use_placeholder_texture = false;
			return texture;
		}
		return Texture::_placeholderTexture;
	}
	return texture;
}
