/*
 * W - a tiny 2D game development library
 *
 * ====================
 *  DrawingClasses.h
 * ====================
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
	
	class Triangle {
	public:
		Triangle(View *, const v2f &p1, const v2f &p2, const v2f &p3, const Colour &, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~Triangle();
		
		void setP1(const v2f &);
		void setP2(const v2f &);
		void setP3(const v2f &);
		void setP123(const v2f &, const v2f &, const v2f &);
		
		void nudge(const v2f &);
		
		void setCol(const Colour &);
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f p1, p2, p3;
		Colour col;
		
	private:
		void *dTri;
		
	};
	
	
	class EqTriangle {
	public:
		EqTriangle(View *, const v2f &, float radius, const Colour &, float rotInDegrees = 0, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~EqTriangle();
		
		void setPosition(const v2f &);
		void setRadius(float);
		
		void nudge(const v2f &);
		
		void setCol(const Colour &);

		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f pos;
		float radius;
		float rotation;
		Colour col;
		
	private:
		v2f p1, p2, p3;
		void genTriProperties();
			// Gen from public properties, save result in p1-3
		void *dTri;
		
	};
	
	
	class IsoTriangle {
	public:
		IsoTriangle(View *, const v2f &pos, const v2f &size, const Colour &, float rotInDegrees = 0, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~IsoTriangle();
		
		void setPosition(const v2f &);
		void setSize(const v2f &);
		void setRotation(float);
		
		void nudge(const v2f &);
		
		void setCol(const Colour &);
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f pos;
		v2f sz;
		float rotation;
		Colour col;
	
	private:
		v2f p1, p2, p3;
		void genTriProperties();
		void *dTri;
		
	};
	
	
	class Rectangle {
	public:
		Rectangle(View *, const v2f &position, const v2f &size, const Colour &, float rotInDegrees = 0, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~Rectangle();
		
		void setPos(const v2f &);
		void setSz(const v2f &);
		void setRot(float);
		void setAll(const v2f &, const v2f &, float);
		void nudge(const v2f &);
		
		void setCol(const Colour &);
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f pos, size;
		float rot;
		Colour col;
		
	protected:
		void *dRect;

	};
	
	
	class Line {
	public:
		Line(View *, const v2f &p1, const v2f &p2, const W::Colour, float width, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~Line();
		
		void setP1(const v2f &);
		void setP2(const v2f &);
		void setP1And2(const v2f &p1, const v2f &p2);
		void nudge(const v2f &);
		
		void setCol(const Colour &);
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f p1, p2;
		float lineWidth;
		Colour col;
		
	private:
		void genRectProperties(v2f &pos, v2f &sz, float &rot);
		Rectangle *rectangle;
	};
	
	
	class Sprite {
	public:
		Sprite(View *, Texture *, const v2f &pos, const v2f &scale, float opacity = 1, float rotInDegrees = 0, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~Sprite();
		
		void setPos(const v2f &);
		void setScale(const v2f &);
		void setRot(float);
		void setAll(const v2f &pos, const v2f &sz, float);
		void nudge(const v2f &);
		
		void setOpacity(float);
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f pos, scale;
		float rot;
		
		float opac;
		
	private:
		void *dSprite;
	};

	
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
	
//	namespace TextAlign {
//		enum T { Left, Right, Centre };
//	}
//	
//	class DText : public DObj {
//	public:
//		DText(View *, const W::position &, const std::string &_txt, const W::Colour &, TextAlign::T _al = TextAlign::Left);
//		~DText();
//		
//		void setPos(const W::position &);
//		void setAlignment(TextAlign::T);
//		void setTxt(const char *);
//		
//		std::string txt;
//		
//		position pos;
//		W::Colour col;
//		TextAlign::T alignment;
//		
//	protected:
//		void updateVertices();
//		void updateTexcoords();
//		
//	private:
//		Texture *tex;
//		int texA, texB, texC, texD;
//
//		static int _geomLengthForText(const std::string &);
//		static int widthForStr(const std::string &);
//		static int widthForChar(char c);
//		
//	};
	
}

#endif
