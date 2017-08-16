/*
 * W - a tiny 2D game development library
 *
 * ======================
 *  DrawingClasses.cpp
 * ======================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "DrawingClasses.h"
#include "Drawable.h"
#include "GenericRetro.h"

#include <cmath>
#ifndef M_PI
	#define M_PI 3.14159265358979
#endif
#define RAD2DEG (180.0/M_PI)
#define DEG2RAD (M_PI/180.0)
#define ROOT3 1.732050808

#define CIRCLE_NPOINTS 15
#define GEOM_LENGTH_FOR_CIRCLE 3*CIRCLE_NPOINTS


#pragma mark - Triangle

W::Triangle::Triangle(View *_v, const v2f &_p1, const v2f &_p2, const v2f &_p3, const Colour &_col, int _lay, BlendMode::T _blend) :
	p1(_p1),
	p2(_p2),
	p3(_p3),
	col(_col)
{
	dTri = new DTri(_v, _p1, _p2, _p3, _col, _lay, _blend);
}
W::Triangle::~Triangle()
{
	delete ((DTri*) dTri);
}
void W::Triangle::setP1(const v2f &x){ ((DTri*) dTri)->setP123(p1 = x, p2, p3); }
void W::Triangle::setP2(const v2f &x){ ((DTri*) dTri)->setP123(p1, p2 = x, p3); }
void W::Triangle::setP3(const v2f &x){ ((DTri*) dTri)->setP123(p1, p2, p3 = x); }
void W::Triangle::setP123(const v2f &_p1, const v2f &_p2, const v2f &_p3) {
	((DTri*) dTri)->setP123(p1 = _p1, p2 = _p2, p3 = _p3);
}
void W::Triangle::nudge(const v2f &delta) {
	p1 += delta, p2 += delta, p3 += delta;
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::Triangle::setCol(const Colour &c) {
	((DTri*) dTri)->setCol(col = c);
}
void W::Triangle::setLayer(int l) {
	((DTri*) dTri)->setLayer(l);
}
void W::Triangle::setBlendMode(BlendMode::T m) {
	((DTri*) dTri)->setBlendMode(m);
}


#pragma mark - EquiTriangle

W::EqTriangle::EqTriangle(View *_v, const v2f &_p, float _rad, const Colour &_col, float _rot, int _lay, BlendMode::T _blend) :
	pos(_p),
	radius(_rad),
	rotation(_rot),
	col(_col)
{
	genTriProperties();
	dTri = new DTri(_v, p1, p2, p3, _col, _lay, _blend);
}
W::EqTriangle::~EqTriangle()
{
	delete ((DTri*) dTri);
}
void W::EqTriangle::setPosition(const v2f &_p) {
	pos = _p;
	genTriProperties();
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::EqTriangle::setRadius(float _rad) {
	radius = _rad;
	genTriProperties();
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::EqTriangle::nudge(const v2f &delta) {
	pos += delta;
	p1 += delta, p2 += delta, p3 += delta;
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::EqTriangle::setCol(const Colour &_col) {
	((DTri*) dTri)->setCol(col = _col);
}
void W::EqTriangle::setLayer(int l) {
	((DTri*) dTri)->setLayer(l);
}
void W::EqTriangle::setBlendMode(BlendMode::T m) {
	((DTri*) dTri)->setBlendMode(m);
}
void W::EqTriangle::genTriProperties() {
	float sideLength = radius * ROOT3;
	float apothem = radius * 0.5;
	
	p1.a = 0,               p1.b = -radius;
	p2.a = sideLength*0.5,  p2.b = apothem;
	p3.a = -sideLength*0.5, p3.b = apothem;
	
	if (rotation != 0.0) {
		float rot = rotation * DEG2RAD;
		float cosR = cos(rot), sinR = sin(rot);
		v2f p1r, p2r, p3r;
		
		p1r.a = p1.a*cosR - p1.b*sinR,
		p2r.a = p2.a*cosR - p2.b*sinR;
		p3r.a = p3.a*cosR - p3.b*sinR;
		
		p1r.b = p1.a*sinR + p1.b*cosR;
		p2r.b = p2.a*sinR + p2.b*cosR;
		p3r.b = p3.a*sinR + p3.b*cosR;
		
		p1.a = p1r.a, p1.b = p1r.b;
		p2.a = p2r.a, p2.b = p2r.b;
		p3.a = p3r.a, p3.b = p3r.b;
	}
	
	p1.a += pos.a, p1.b += pos.b;
	p2.a += pos.a, p2.b += pos.b;
	p3.a += pos.a, p3.b += pos.b;
}


#pragma mark - IsoTriangle

W::IsoTriangle::IsoTriangle(View *_v, const v2f &_p, const v2f &_sz, const Colour &_col, float _rot, int _lay, BlendMode::T _blend) :
	pos(_p),
	sz(_sz),
	rotation(_rot),
	col(_col)
{
	genTriProperties();
	dTri = new DTri(_v, p1, p2, p3, _col, _lay, _blend);
}
W::IsoTriangle::~IsoTriangle()
{
	delete ((DTri*) dTri);
}
void W::IsoTriangle::setPosition(const v2f &_p) {
	pos = _p;
	genTriProperties();
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::IsoTriangle::setSize(const v2f &_sz) {
	sz = _sz;
	genTriProperties();
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::IsoTriangle::nudge(const v2f &delta) {
	pos += delta;
	p1 += delta, p2 += delta, p3 += delta;
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::IsoTriangle::setCol(const Colour &_col) {
	((DTri*) dTri)->setCol(col = _col);
}
void W::IsoTriangle::setLayer(int l) {
	((DTri*) dTri)->setLayer(l);
}
void W::IsoTriangle::setBlendMode(BlendMode::T m) {
	((DTri*) dTri)->setBlendMode(m);
}
void W::IsoTriangle::genTriProperties() {
	float halfWidth = sz.a * 0.5;
	float halfHeight = sz.b * 0.5;
	
	p1.a = 0,               p1.b = -halfHeight;
	p2.a = -halfWidth,      p2.b = halfHeight;
	p3.a = halfWidth,       p3.b = halfHeight;
	
	if (rotation != 0.0) {
		float rot = rotation * DEG2RAD;
		float cosR = cos(rot), sinR = sin(rot);
		v2f p1r, p2r, p3r;
		
		p1r.a = p1.a*cosR - p1.b*sinR,
		p2r.a = p2.a*cosR - p2.b*sinR;
		p3r.a = p3.a*cosR - p3.b*sinR;
		
		p1r.b = p1.a*sinR + p1.b*cosR;
		p2r.b = p2.a*sinR + p2.b*cosR;
		p3r.b = p3.a*sinR + p3.b*cosR;
		
		p1.a = p1r.a, p1.b = p1r.b;
		p2.a = p2r.a, p2.b = p2r.b;
		p3.a = p3r.a, p3.b = p3r.b;
	}
	
	p1.a += pos.a, p1.b += pos.b;
	p2.a += pos.a, p2.b += pos.b;
	p3.a += pos.a, p3.b += pos.b;
}



#pragma mark - Rectangle

W::Rectangle::Rectangle(
	View *_v,
	const v2f &_p,
	const v2f &_sz,
	const Colour &_col,
	float _rot,
	int _lay,
	BlendMode::T _blend
) :
	pos(_p),
	size(_sz),
	rot(_rot),
	col(_col)
{
	dRect = new DRect(_v, _p, _sz, _col, _rot, _lay, _blend);
}
W::Rectangle::~Rectangle()
{
	delete ((DRect*) dRect);
}
void W::Rectangle::setPos(const v2f &_p) {
	((DRect*) dRect)->setPosSzRot(pos = _p, size, rot);
}
void W::Rectangle::setSz(const v2f &_s) {
	((DRect*) dRect)->setPosSzRot(pos, size = _s, rot);
}
void W::Rectangle::setRot(float _r) {
	((DRect*) dRect)->setPosSzRot(pos, size, rot = _r);
}
void W::Rectangle::setAll(const v2f &_p, const v2f &_s, float _r) {
	((DRect*) dRect)->setPosSzRot(pos = _p, size = _s, rot = _r);
}
void W::Rectangle::nudge(const v2f &delta) {
	pos += delta;
	((DRect*) dRect)->setPosSzRot(pos, size, rot);
}
void W::Rectangle::setCol(const Colour &_c) {
	((DRect*) dRect)->setCol(col = _c);
}
void W::Rectangle::setLayer(int l) {
	((DRect*) dRect)->setLayer(l);
}
void W::Rectangle::setBlendMode(BlendMode::T m) {
	((DRect*) dRect)->setBlendMode(m);
}


#pragma mark - Line

W::Line::Line(View *_v, const v2f &_p1, const v2f &_p2, const W::Colour _col, float _width, int _lay, BlendMode::T _blend) :
	p1(_p1),
	p2(_p2),
	lineWidth(_width),
	col(_col)
{
	v2f rPos, rSz;
	float rRot;
	
	genRectProperties(rPos, rSz, rRot);
	rectangle = new Rectangle(_v, rPos, rSz, col, rRot, _lay, _blend);
}
W::Line::~Line()
{
	delete rectangle;
}
void W::Line::setP1(const v2f &_p) {
	p1 = _p;
	genRectProperties(rectangle->pos, rectangle->size, rectangle->rot);
	rectangle->setAll(rectangle->pos, rectangle->size, rectangle->rot);
}
void W::Line::setP2(const v2f &_p) {
	p2 = _p;
	genRectProperties(rectangle->pos, rectangle->size, rectangle->rot);
	rectangle->setAll(rectangle->pos, rectangle->size, rectangle->rot);
}
void W::Line::setP1And2(const v2f &_p1, const v2f &_p2) {
	p1 = _p1;
	p2 = _p2;
	genRectProperties(rectangle->pos, rectangle->size, rectangle->rot);
	rectangle->setAll(rectangle->pos, rectangle->size, rectangle->rot);
}
void W::Line::nudge(const v2f &delta) {
	p1 += delta;
	p2 += delta;
	rectangle->nudge(delta);
}
void W::Line::genRectProperties(v2f &pos, v2f &sz, float &rot) {
	float lineLength = sqrt((p2.a-p1.a)*(p2.a-p1.a) + (p2.b-p1.b)*(p2.b-p1.b));
	
	sz.b = lineWidth;
	sz.a = lineLength;
	
	pos.a = p1.a + (p2.a - p1.a)*0.5 - lineLength/2;
	pos.b = p1.b + (p2.b - p1.b)*0.5 - lineWidth/2;
	
	rot = asin((p2.b-p1.b) / lineLength) * RAD2DEG;
}
void W::Line::setCol(const Colour &_c) {
	rectangle->setCol(col = _c);
}
void W::Line::setLayer(int l) {
	rectangle->setLayer(l);
}
void W::Line::setBlendMode(BlendMode::T m) {
	rectangle->setBlendMode(m);
}


/*****************************/
/*** Sprite implementation ***/
/*****************************/

#pragma mark - Sprite

W::Sprite::Sprite(
	View *_v,
	Texture *_t,
	const v2f &_p,
	const v2f &_sc,
	float _opac,
	float _rot,
	int _lay,
	BlendMode::T _blend
) :
	pos(_p),
	scale(_sc),
	rot(_rot),
	opac(_opac)
{
	dSprite = new DSprite(_v, _t, _p, _sc, _opac, _rot, _lay, _blend);
}
W::Sprite::~Sprite()
{
	delete ((DSprite*) dSprite);
}
void W::Sprite::setPos(const v2f &_p) {
	((DSprite*) dSprite)->setPosScaleRot(pos = _p, scale, rot);
}
void W::Sprite::setScale(const v2f &_sc) {
	((DSprite*) dSprite)->setPosScaleRot(pos, scale = _sc, rot);
}
void W::Sprite::setRot(float _r) {
	((DSprite*) dSprite)->setPosScaleRot(pos, scale, rot = _r);
}
void W::Sprite::setAll(const v2f &_p, const v2f &_sc, float _r) {
	((DSprite*) dSprite)->setPosScaleRot(pos = _p, scale = _sc, rot = _r);
}
void W::Sprite::nudge(const v2f &delta) {
	pos += delta;
	((DSprite*) dSprite)->setPosScaleRot(pos, scale, rot);
}
void W::Sprite::setOpacity(float _o) {
	((DSprite*) dSprite)->setOpac(_o);
}
void W::Sprite::setLayer(int l) {
	((DSprite*) dSprite)->setLayer(l);
}
void W::Sprite::setBlendMode(BlendMode::T m) {
	((DSprite*) dSprite)->setBlendMode(m);
}

///******************************/
///*** DCircle implementation ***/
///******************************/
//
//struct W::DCircle::Init {
//	Init() {
//		// Generate unit circle geometry
//		circleGeom = (v3f*)malloc(sizeof(v3f) * CIRCLE_NPOINTS * 3);
//		
//		float alpha = 0, dAlpha = 2*M_PI/CIRCLE_NPOINTS;
//		v3f origin = {0,0,0},
//			initialExternalVertex = {0,1,0};
//		
//		for (int i=0; i < CIRCLE_NPOINTS; ++i) {
//			v3f &v1 = circleGeom[i*3],
//				&v2 = circleGeom[i*3+1],
//				&v3 = circleGeom[i*3+2];
//			
//			alpha += dAlpha;
//			
//			v1 = origin;
//			v2 = (i == 0 ? initialExternalVertex : circleGeom[(i-1)*3+2]);
//			if (i == CIRCLE_NPOINTS-1)
//				v3 = initialExternalVertex;
//			else
//				v3.x = sin(alpha), v3.y = cos(alpha), v3.z = 0;
//		}
//	}
//};
//
//W::v3f *W::DCircle::circleGeom;
//W::DCircle::Init *W::DCircle::initializer = new W::DCircle::Init();
//
//W::DCircle::DCircle(View *_v, const W::position &_centrePos, int _radius, const Colour &_col) :
//	DObj(_v, GEOM_LENGTH_FOR_CIRCLE),
//	centrePos(_centrePos),
//	radius(_radius),
//	col(_col)
//{
//	// Hai circle
//}
//void W::DCircle::_recopy(v3f *vert_array, c4f *col_array, t2f *texcoord_array) {
//	Texture &tex = *Texture::_whiteTexture;
//	float texA = tex.floatCoordA(0),
//		texB = tex.floatCoordB(0),
//		texC = tex.floatCoordC(tex.sz.width),
//		texD = tex.floatCoordD(tex.sz.height);
//	
//	for (int i=0; i < GEOM_LENGTH_FOR_CIRCLE; ++i) {
//		v3f &vA = circleGeom[i],
//			&vB = vert_array[i];
//		vB.x = vA.x * radius + centrePos.x;
//		vB.y = vA.y * radius + centrePos.y;
//		vB.z = 0;
//		
//		col_array[i] = col;
//		
//		t2f &tc = texcoord_array[i];
//		if (i%3 == 0)      tc.x = texA, tc.y = texB;
//		else if (i%3 == 1) tc.x = texA, tc.y = texD;
//		else               tc.x = texC, tc.y = texD;	
//	}
//}


/****************************/
/*** DText implementation ***/
/****************************/

#pragma mark - DText

//W::DText::DText(View *_v, const position &_p, const std::string &_txt, const Colour &_col, TextAlign::T _al) :
//	DObj(_v, _col, _geomLengthForText(downCase(_txt))),
//	pos(_p),
//	txt(_txt),
//	col(_col),
//	alignment(_al),
//	tex(Texture::_whiteTexture),
//	texA(0),texB(0),texC(tex->sz.width),texD(tex->sz.height)
//{
//	tex->incrementUsageCount();
//	
//	txt = downCase(txt);
//	setPos(pos);
//	
//	updateTexcoords();
//	updateColour();
//}
//W::DText::~DText()
//{
//	tex->decrementUsageCount();
//}
//void W::DText::setPos(const W::position &_pos) {
//	pos = _pos;
//	// Calculate position given total pixel width
//	if (alignment == TextAlign::Right)       pos.x -= widthForStr(txt);
//	else if (alignment == TextAlign::Centre) pos.x -= widthForStr(txt) * 0.5;
//	updateVertices();
//}
//void W::DText::setAlignment(TextAlign::T _al) {
//	if (_al != alignment) {
//		alignment = _al;
//		setPos(pos);
//		updateVertices();
//	}
//}
//void W::DText::setTxt(const char *_txt) {
//	// Remove DObj
//	makeTrianglesDegenerate();
//	view->_remDObj(this);
//	
//	// Set pos property back to non-alignment-corrected state
//	if (alignment == TextAlign::Right) pos.x += widthForStr(txt);
//	else if (alignment == TextAlign::Centre) pos.x += widthForStr(txt) * 0.5;
//	
//	// Put back in virgin state
//	prevDObj = nextDObj = NULL;
//	_preceding_removed = 0;
//	txt = _txt;
//	downCase(txt);
//	array_length = _geomLengthForText(txt);
//	
//	// Re-add
//	view->_addDObj(this);
//	setPos(pos);
//	
//	updateVertices();	// The location & length of our array chunk may have
//	updateColour();		// changed, so we have to update everything.
//	updateTexcoords();	//
//}
//void W::DText::updateVertices() {
//	W::position letterPos = pos;
//	for (int i=0, geomOffset=0, c; (c=txt[i]); ++i) {
//		const std::vector<rect> &rectVec = GenericRetro[c];
//		for (std::vector<rect>::const_iterator it = rectVec.begin(); it < rectVec.end(); ++it) {
//			const rect &r = *it;
//			copyRectGeom(vert_ptr+geomOffset, letterPos+r.pos, r.sz, 0);
//			geomOffset += 6;
//		}
//		letterPos.x += widthForChar(c);
//	}
//}
//void W::DText::updateTexcoords() {
//	float
//		tA = tex->floatCoordA(0),
//		tB = tex->floatCoordB(0),
//		tC = tex->floatCoordC(tex->sz.width),
//		tD = tex->floatCoordD(tex->sz.height);
//	
//	for (int i=0; i < array_length; i += 6) {
//		W::t2f
//			&tc1 = texcoord_ptr[i],
//			&tc2 = texcoord_ptr[i+1],
//			&tc3 = texcoord_ptr[i+2],
//			&tc4 = texcoord_ptr[i+3],
//			&tc5 = texcoord_ptr[i+4],
//			&tc6 = texcoord_ptr[i+5];
//		
//		tc1.x = tA, tc1.y = tB;
//		tc2.x = tA, tc2.y = tD;
//		tc3.x = tC, tc3.y = tD;
//		tc4.x = tA, tc4.y = tB;
//		tc5.x = tC, tc5.y = tB;
//		tc6.x = tC, tc6.y = tD;
//	}
//}
//int W::DText::_geomLengthForText(const std::string &txt) {
//	int x = 0, c;
//	for (int i=0; (c=txt[i]); ++i)
//		x += GenericRetro[c].size();
//	return x * 6;
//}
//int W::DText::widthForStr(const std::string &s) {
//	int tw = 0, c;
//	for (int i=0; (c=s[i]); ++i) tw += widthForChar(c);
//	return tw;
//}
//int W::DText::widthForChar(char c) {
//	static int defaultCharWidth = 14;
//	if (c == 'i' || c == '1' || c == ':' || c == '!' || c == '.' || c == '\'') return defaultCharWidth - 4;
//	else if (c == 'l') return defaultCharWidth - 2;
//	return defaultCharWidth;
//}
