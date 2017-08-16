/*
 * W - a simple, cross-platform 2D game develpoment library
 *
 * ==========
 *  DObj.h
 * ==========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
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
		DObj(View *_v, int _length);
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
	
	protected:
		virtual void _recopy(v3f*, c4f*, t2f*) = 0;
			// Override to copy data into arrays
		
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
		
		void setPos(const W::position &_p) { pos = _p; setNeedsRecopy(); }
		void setSz(const W::size &_sz) { sz = _sz; setNeedsRecopy(); }
		
		position pos;
		size sz;
		Colour col;
		Texture *tex;
		float rotation;
		int texA, texB, texC, texD;
			// These are the integer coords of the drawn "chunk" of the texture
			// in relation to it itself. Initially {0,0,tw,th}.
			// The conversion to float coords of the MegaTexture is done by the
			// GeometryChunk when generating GL array data.
	protected:
		void _recopy(v3f*, c4f*, t2f*);	// Recopy geom data into supplied arrays
		
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
		DLine draws a line
	 ***/
	
	class DLine : public _UniRectDObj {
	public:
		DLine(View *, const W::position &_p1, const W::position &_p2, const W::Colour &, int _width = 1);
		void setPos1(const W::position &);
		void setPos2(const W::position &);
		void setWidth(int _w);
		void setCol(const Colour &_c) { col = _c, setNeedsRecopy(); }
		void nudge(const W::position &);
		
		position p1, p2;
		Colour col;
		int lineWidth;
		
	private:
		void recalculateRectProperties();
	};
	
	
	/***
		DCircle
	 ***/
	
	class DCircle : public DObj {
	public:
		DCircle(View *, const W::position &_centrePos, int _radius, const W::Colour &);
		void setCentrePos(const W::position &_p) { centrePos = _p; setNeedsRecopy(); }
		void setRadius(int _r) { radius = _r; setNeedsRecopy(); }
		void setCol(const W::Colour &_c) { col = _c; setNeedsRecopy(); }
		
		position centrePos;
		int radius;
		Colour col;
		
	protected:
		void _recopy(v3f*, c4f*, t2f*);	// Recopy geom data into supplied arrays

	private:
		static v3f *circleGeom;
		struct init;
		static init *Initializer;
	};
	
	
	/***
		DImage draws an image
	 ***/
	
	class DImg : public _UniRectDObj {
	public:
		DImg(View *, const W::position &, const W::size &, Texture *, float opacity = 1, float rotation = 0);
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
		
		void setPos(const W::position &_pos);
		void setCol(Colour _c) { col = _c; setNeedsRecopy(); }
		void setRAlign(bool _a) { r_align = _a; setNeedsRecopy(); }
//		void setTxt(const char *_txt) { txt = _txt; _setDirty(); }
		
		std::string txt;
		
		position pos;
		W::Colour col;
		bool r_align;
		
	protected:
		void _recopy(v3f*, c4f*, t2f*);
		
	private:
		static int _geomLengthForText(const std::string &);
		Texture *tex;
		int texA, texB, texC, texD;
		static int widthForChar(char c);
	};
	
}

#endif
