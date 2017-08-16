/*
 * W - a tiny 2D game development library
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
		DObj(View *_v, const Colour &, int _length);
		virtual ~DObj();
		
		void _setArrayPtrs(v3f *_base_vptr, c4f *_base_cptr, t2f *_base_tptr, int _array_index = -1);
		void _moveBackBy(int);
		
		DObj *prevDObj, *nextDObj;
		int _preceding_removed;
			// Number of DObjs immediately preceding this one which have been removed
			// since the last compact
		
		Colour col;
		void setCol(const Colour &_c) { col = _c; updateColour(); }
		
		int arrayLength() { return array_length; }
		
		std::string str() {
			std::stringstream ss;
			ss << this << " i:" << array_index << "n:" << array_length << " pr:" << _preceding_removed;
			ss << " prev:" << prevDObj;
			ss << " next:" << nextDObj;
			return ss.str();
		}
		
	protected:
		View *view;
		void updateColour() {
			for (int i=0; i < array_length; ++i) {
				col_ptr[i] = col;
			}
		}
		void makeTrianglesDegenerate() {
			for (int i=0; i < array_length; ++i) {
				v3f &v = vert_ptr[i];
				v.x = 0;
				v.y = 0;
			}
		}
		v3f *vert_ptr;
		c4f *col_ptr;
		t2f *texcoord_ptr;
		int array_index;
		int array_length;
		
	private:
		v3f *base_vptr;
		c4f *base_cptr;
		t2f *base_tptr;
	};
	
	
	/***
		DUniRect class does the work for most DO subclasses, which on
		the back end manage a single textured, coloured rect.
	***/
	
	class DUniRect : public DObj {
	public:
		DUniRect(View *, const W::position &, const W::size &, const W::Colour &, W::Texture *, float rotation);
		~DUniRect();
		
		void setPos(const W::position &_p) { pos = _p; updateVertices(); }
		void setSz(const W::size &_sz) { sz = _sz; updateVertices(); }
		void setRot(float _r) { rotation = _r; updateVertices(); }
		
		position pos;
		size sz;
		Texture *tex;
		float rotation;
		int texA, texB, texC, texD;
			// These are the integer coords of the drawn "chunk" of the texture
			// in relation to it itself - initially: {0,0,tw,th}
			// The conversion to float coords of the MegaTexture must be done:
			//  - when editing the values of the texcoord array
			//  - when the MT changes in size
		
	protected:
		void updateVertices();
		void updateTexcoords();
			// Utility methods for subclasses to call in their setters
			// to apply user changes
		
	};
	
	
	/***
		DRect draws a coloured rect
	 ***/
	
	class DRect : public DUniRect {
	public:
		DRect(View *, const W::position &, const W::size &, const W::Colour &, float rotation = 0);
	};
	
	
	/***
		DImage draws an image
	 ***/
	
	class DImg : public DUniRect {
	public:
		DImg(View *, const W::position &, const W::size &, Texture *, float opacity = 1, float rotation = 0);
		void setOpacity(float _o) { col.a = _o; updateColour(); }
		void setChunk(int a, int b, int c, int d) {
			texA = a, texB = b, texC = c, texD = d;
			updateTexcoords();
		}
	};
	
	
	/***
		DLine draws a line
	 ***/
	
	class DLine : public DUniRect {
	public:
		DLine(View *, const W::position &_p1, const W::position &_p2, const W::Colour &, int _width = 1);
		void setP1(const W::position &);
		void setP2(const W::position &);
		void setWidth(int);
		void nudge(const W::position &);
		
		position p1, p2;
		int lineWidth;
		
	private:
		void regenRectProperties();
	};

	
	/***
		DEquiTri draws an equilateral triangle
	 ***/
	
	class DEquiTri : public DObj {
	public:
		DEquiTri(View *, const position &, float _radius, const W::Colour &, float rotation = 0);
		~DEquiTri();
		void setPos(const position &_p) { pos = _p; updateVertices(); }
		void setRadius(float _r) { radius = _r; updateVertices(); }
		void setRot(float _r) { rotation = _r; updateVertices(); }
		
		position pos;
		float radius;
		Colour col;
		float rotation;
		Texture *tex;
		
	protected:
		void updateVertices();
		void updateTexcoords();
	};
//
//	
//	/***
//		DCircle
//	 ***/
//	
//	class DCircle : public DObj {
//	public:
//		DCircle(View *, const W::position &_centrePos, int _radius, const W::Colour &);
//		void setCentrePos(const W::position &_p) { centrePos = _p; setNeedsRecopy(); }
//		void setRadius(int _r) { radius = _r; setNeedsRecopy(); }
//		void setCol(const W::Colour &_c) { col = _c; setNeedsRecopy(); }
//		
//		position centrePos;
//		int radius;
//		Colour col;
//		
//	protected:
//		void _recopy(v3f*, c4f*, t2f*);	// Recopy geom data into supplied arrays
//
//	private:
//		static v3f *circleGeom;
//		struct Init;
//		static Init *initializer;
//	};


	/***
		DText draws... text
	 ***/
	
	class DText : public DObj {
	public:
		DText(View *, const W::position &, const std::string &_txt, const W::Colour &, bool r_align = false);
		~DText();
		
		void setPos(const W::position &);
		void setRAlign(bool);
		void setTxt(const char *);
		
		std::string txt;
		
		position pos;
		W::Colour col;
		bool r_align;
		
	protected:
		void updateVertices();
		void updateTexcoords();
		
	private:
		Texture *tex;
		int texA, texB, texC, texD;

		static int _geomLengthForText(const std::string &);
		static int widthForChar(char c);
		
	};
	
}

#endif
