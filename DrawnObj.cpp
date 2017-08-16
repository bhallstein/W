#include "DrawnObj.h"

W::DrawnObj::DrawnObj(View *_view, DOType _type, const position &_pos, const size &_sz) :
	type(_type), rct(_pos, _sz), new_rct(_pos, _sz), view(_view)
{
	// hai objy
}

W::DrawnRect::DrawnRect(View *_view, const position &_p, const size &_s, const Colour &_col, float _rot) :
	DrawnObj(_view, RECT, _p, _s),
	col(_col), new_col(_col), rot(_rot), new_rot(_rot)
{
	// hai recty
}


W::DrawnText::DrawnText(View *_view, const position &_p, const char *_txt, const Colour &_col, bool _r_align) :
	DrawnObj(_view, TEXT, _p, size()),
	txt(_txt), new_txt(_txt), col(_col), new_col(_col), r_align(_r_align), new_r_align(_r_align)
{
	// hai texty
}


W::DrawnImage::DrawnImage(View *_view, const position &_p, const size &_s, Texture *_texture, float _opac, float _rot) :
	DrawnObj(_view, IMAGE, _p, _s),
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
