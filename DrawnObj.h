/*
 * DrawnObj.h
 *
 * DrawnObj (D.O.) is the base class for all drawn items in the game.
 * It has various subclasses for different types of drawn object, as below.
 * 
 * To draw to the screen, a user class should initialize an object of one of the below types:
 * 		myRect = new Rectangle(~);
 * Then, the object should be added to the appropriate View:
 * 		myView->addDO(myRect);
 * 
 * To update the D.O., e.g. to change its position, use the class’s property setters,
 * for example, setPosn().
 *
 * To remove the D.O., use:
 *		myview->removeDO(myRect)
 * The object will be deallocated automatically. Never manually deallocate a DrawnObj.
 * 
 * DrawnObjs are drawn on a different thread from user code. For thread safety reasons, changes
 * made to them take effect only at the end of an update cycle.
 *
 */

#ifndef __W__DrawableObj
#define __W__DrawableObj

#include "types.h"
#include "Colour.h"
#include "View.h"
#include "Texture.h"

namespace W {
	
	class DrawnObj {
	public:
		enum DOType {
			RECT,
			TEXT,
			IMAGE
		};
		DrawnObj(View *, DOType _type, const W::position &, const W::size &);
		virtual ~DrawnObj() { }
		DOType type;
		
		void _updateValues() {
			rct = new_rct;
			updateValues();
		}
		virtual void updateValues() = 0;
		
		void setPos(const W::position &_pos) { new_rct.pos = _pos; _setDirty(); }
		void setSz(const W::size &_sz) { new_rct.sz = _sz; _setDirty(); }
		void setRect(const W::rect &_rect) { new_rct = _rect; _setDirty(); }
		
		rect rct, new_rct;
		
	protected:
		void _setDirty() {
			view->_markDOAsDirty(this);
		}
		View *view;
	};
	
	/* Subclasses should implement setters for user-modifiable properties.
	 * A setter for x should set a new_x counterpart of x, not x itself.
	 * The setter should then call _setDirty().
	 * An updateValues() override should then be provided which overwrites x with new_x.
	 */
	
	class DrawnRect : public DrawnObj {
	public:
		DrawnRect(View *, const W::position &, const W::size &, const W::Colour &, float rotation = 0);
		
		void setCol(const Colour &_col) { new_col = _col; _setDirty(); }
		void setRot(float _rotation) { new_rot = _rotation; _setDirty(); }
		
		void updateValues() {
			col = new_col;
			rot = new_rot;
		}
		
		W::Colour col, new_col;
		float rot, new_rot;
	};
	
	
	class DrawnText : public DrawnObj {
	public:
		DrawnText(View *, const W::position &, const char *, const W::Colour &, bool r_align = false);
		void setCol(Colour _col) { new_col = _col; _setDirty(); }
		void setTxt(const char *_txt) { new_txt = _txt; _setDirty(); }
		
		void updateValues() {
			col = new_col;
			txt = new_txt;
			r_align = new_r_align;
		}
		
		std::string txt, new_txt;
		W::Colour col, new_col;
		bool r_align, new_r_align;
	};
	
	
	class DrawnImage : public DrawnObj {
	public:
		DrawnImage(View *, const W::position &, const W::size &, Texture *, float opacity = 1, float rotation = 0);
		~DrawnImage();
		void setRot(float _rotation) { new_rot = _rotation; _setDirty(); }
		void setOpacity(float _opacity) { new_opac = _opacity; _setDirty(); }
		
		void updateValues() {
			rot = new_rot;
			opac = new_opac;
		}
		
		float rot, new_rot;
		float opac, new_opac;
		
		Texture * getTex();
		
	protected:
		Texture *texture;
		bool use_placeholder_texture;
	};
	
}

#endif
