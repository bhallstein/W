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
		Triangle(View *, v2f p1, v2f p2, v2f p3, Colour, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~Triangle();
		
		void setP1(v2f);
		void setP2(v2f);
		void setP3(v2f);
		void setP123(v2f, v2f, v2f);
		
		void nudge(v2f);
		
		void setCol(Colour);
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f p1, p2, p3;
		Colour col;
		
	private:
		void *dTri;
		
	};
	
	
	class EqTriangle {
	public:
		EqTriangle(View *, v2f, float radius, Colour, float rotInDegrees = 0, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~EqTriangle();
		
		void setPosition(v2f);
		void setRadius(float);
		
		void nudge(v2f);
		
		void setCol(Colour);

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
		IsoTriangle(View *, v2f pos, v2f size, Colour, float rotInDegrees = 0, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~IsoTriangle();
		
		void setPosition(v2f);
		void setSize(v2f);
		void setRotation(float);
		
		void nudge(v2f);
		
		void setCol(Colour);
		
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
		Rectangle(View *, v2f position, v2f size, Colour, float rotInDegrees = 0, int layer = DEFAULT_LAYER, BlendMode::T blendMode = BlendMode::Normal);
		~Rectangle();
		
		void setPos(v2f);
		void setSz(v2f);
		void setRot(float);
		void setAll(v2f pos, v2f sz, float);
		void nudge(v2f);
		
		void setCol(Colour);
		
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
		Line(View *, v2f p1, v2f p2, W::Colour, float width, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~Line();
		
		void setP1(v2f);
		void setP2(v2f);
		void setP1And2(v2f, v2f);
		void nudge(v2f);
		
		void setCol(Colour);
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f p1, p2;
		float lineWidth;
		Colour col;
		
	private:
		void genDelta();
		v2f delta;
			// We add/sub delta*width from p1 & p2 to get the
			// rect's corners
		void *dLine;
	};
	
	
	class Circle {
	public:
		Circle(View *, v2f _center, float _radius, W::Colour, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~Circle();
		
		void setCenter(v2f);
		void setRadius(float);
		void setCol(W::Colour);
		
		v2f center;
		float radius;
		Colour col;
		
	protected:
		void *dCircle;
	};
	
	
	class Sprite {
	public:
		Sprite(View *, Texture *, v2f pos, v2f scale = v2f(1.0,1.0), float opacity = 1, float rotInDegrees = 0, int layer = DEFAULT_LAYER, BlendMode::T = BlendMode::Normal);
		~Sprite();
		
		void setPos(v2f);
		void setScale(v2f);
		void setRot(float);
		void setAll(v2f pos, v2f sz, float);
		void nudge(v2f);
		
		void setOpacity(float);
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		v2f pos, scale;
		float rot;
		
		float opac;
		
	private:
		void *dSprite;
	};


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
