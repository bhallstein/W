/*
 * DObj.h - user-facing drawn object classes
 *
 */

#ifndef __W__DrawableObj
#define __W__DrawableObj

#include "types.h"
#include "Colour.h"
#include "View.h"
#include "Texture.h"

namespace W {
	
	class DObj {
	public:
		DObj(View *_v, const W::position &, int _length);
		virtual ~DObj();

		void setNeedsRecopy() {
			if (!has_set_recopy) {
				view->_setNeedsRecopy(this);
				has_set_recopy = true;
			}
		}
		virtual void recopy(v3f *v, c4f *c, t2f *t) {
			_recopy(v+array_start_index, c+array_start_index, t+array_start_index);
			has_set_recopy = false;
		}
		virtual void _recopy(v3f*, c4f*, t2f*) = 0;
			// Override to copy data into arrays

		void setPos(const W::position &_p) { pos = _p; setNeedsRecopy(); }
		W::position pos;

	private:
		DObj *prevDObj, *nextDObj;
		View *view;
		
		int array_start_index;
		int array_length;
		
		void moveBackBy(int n) {
			array_start_index -= n;
			setNeedsRecopy();
			if (nextDObj) nextDObj->moveBackBy(n);
		}
		
		bool has_set_recopy;
	};
	
	
	/***
		_UniRectDObj class does the work for most DO subclasses, which on
		the back end manage a single textured, coloured rect.
	***/
	
	class _UniRectDObj : public DObj {
	public:
		_UniRectDObj(View *, const W::position &, const W::size &, const W::Colour &, W::Texture *, float rotation);
		~_UniRectDObj();
		void _recopy(v3f*, c4f*, t2f*);	// Recopy geom data into supplied arrays
		
		void setSz(const W::size &_sz) { sz = _sz; setNeedsRecopy(); }
		
		size sz;
		Colour col;
		Texture *tex;
		float rotation;
		int texA, texB, texC, texD;
			// These are the integer coords of the drawn "chunk" of the texture
			// in relation to it itself. Initially {0,0,tw,th}.
			// The conversion to float coords of the MegaTexture is done by the
			// GeometryChunk when generating GL array data.
	};
	
	
	/***
		DRect draws a single, coloured rect
	 ***/
	
	class DRect : public _UniRectDObj {
	public:
		DRect(View *, const W::position &, const W::size &, const W::Colour &, float rotation = 0);
		void setCol(const Colour &_c) { col = _c; setNeedsRecopy(); }
		void setRot(float _r) { rotation = _r; setNeedsRecopy(); }
	};
	
	
	/***
		DImage draws an image
	 ***/
	
	class DImage : public _UniRectDObj {
	public:
		DImage(View *, const W::position &, const W::size &, Texture *, float opacity = 1, float rotation = 0);
		void setRot(float _r) { rotation = _r; setNeedsRecopy(); }
		void setOpacity(float _o) { col.a = _o; setNeedsRecopy(); }
		void setChunk(int a, int b, int c, int d) {
			texA = a, texB = b, texC = c, texD = d;
			setNeedsRecopy();
		}
	};

	
	/***
		DText draws... text
	 ***/
	
	class DText : public DObj {
	public:
		DText(View *, const W::position &, const std::string &_txt, const W::Colour &, bool r_align = false);
		~DText();
		void _recopy(v3f*, c4f*, t2f*);
		
		void setPos(const W::position &_pos);
		void setCol(Colour _c) { col = _c; setNeedsRecopy(); }
		void setRAlign(bool _a) { r_align = _a; setNeedsRecopy(); }
//		void setTxt(const char *_txt) { txt = _txt; _setDirty(); }
		
		std::string txt;
		
		W::Colour col;
		bool r_align;
	private:
		static int _geomLengthForText(const std::string &);
		Texture *tex;
		int texA, texB, texC, texD;
		static int widthForChar(char c);
	};
	
}

#endif
