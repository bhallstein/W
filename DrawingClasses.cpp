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

#define CIRCLE_NPOINTS 15
#define GEOM_LENGTH_FOR_CIRCLE 3*CIRCLE_NPOINTS


#pragma mark - Triangle

W::Triangle::Triangle(View *_v, const position &_p1, const position &_p2, const position &_p3, const Colour &_col, int _lay, BlendMode::T _blend) :
	p1(_p1),
	p2(_p2),
	p3(_p3),
	col(_col)
{
	dTri = new DTri(_v, _p1, _p2, _p3, _col, _lay, _blend);
}
W::Triangle::~Triangle()
{
	delete dTri;
}
void W::Triangle::setP1(const position &x){ dTri->setP123(p1 = x, p2, p3); }
void W::Triangle::setP2(const position &x){ dTri->setP123(p1, p2 = x, p3); }
void W::Triangle::setP3(const position &x){ dTri->setP123(p1, p2, p3 = x); }
void W::Triangle::nudge(const position &delta) {
	p1 += delta, p2 += delta, p3 += delta;
	dTri->setP123(p1, p2, p3);
}
void W::Triangle::setCol(const Colour &c) {
	dTri->setCol(col = c);
}
void W::Triangle::setLayer(int l) {
	dTri->setLayer(l);
}
void W::Triangle::setBlendMode(BlendMode::T m) {
	dTri->setBlendMode(m);
}


#pragma mark - Rectangle

W::Rectangle::Rectangle(
	View *_v,
	const position &_p,
	const size &_sz,
	const Colour &_col,
	float _rot,
	int _lay,
	BlendMode::T _blend
) :
	pos(_p),
	sz(_sz),
	rot(_rot),
	col(_col)
{
	dRect = new DRect(_v, _p, _sz, _col, _rot, _lay, _blend);
}
W::Rectangle::~Rectangle()
{
	delete dRect;
}
void W::Rectangle::setPos(const position &_p) {
	dRect->setPosSzRot(pos = _p, sz, rot);
}
void W::Rectangle::setSz(const size &_s) {
	dRect->setPosSzRot(pos, sz = _s, rot);
}
void W::Rectangle::setRot(float _r) {
	dRect->setPosSzRot(pos, sz, rot = _r);
}
void W::Rectangle::setAll(const position &_p, const size &_s, float _r) {
	dRect->setPosSzRot(pos = _p, sz = _s, rot = _r);
}
void W::Rectangle::nudge(const position &delta) {
	pos += delta;
	dRect->setPosSzRot(pos, sz, rot);
}
void W::Rectangle::setCol(const Colour &_c) {
	dRect->setCol(col = _c);
}
void W::Rectangle::setLayer(int l) {
	dRect->setLayer(l);
}
void W::Rectangle::setBlendMode(BlendMode::T m) {
	dRect->setBlendMode(m);
}


#pragma mark - Line

W::Line::Line(View *_v, const position &_p1, const position &_p2, const W::Colour _col, float _width, int _lay, BlendMode::T _blend) :
	p1(_p1),
	p2(_p2),
	lineWidth(_width),
	col(_col)
{
	position rPos;
	size rSz;
	float rRot;
	
	genRectProperties(rPos, rSz, rRot);
	rectangle = new Rectangle(_v, rPos, rSz, col, rRot, _lay, _blend);
}
W::Line::~Line()
{
	delete rectangle;
}
void W::Line::setP1(const position &_p) {
	p1 = _p;
	genRectProperties(rectangle->pos, rectangle->sz, rectangle->rot);
	rectangle->setAll(rectangle->pos, rectangle->sz, rectangle->rot);
}
void W::Line::setP2(const position &_p) {
	p2 = _p;
	genRectProperties(rectangle->pos, rectangle->sz, rectangle->rot);
	rectangle->setAll(rectangle->pos, rectangle->sz, rectangle->rot);
}
void W::Line::setP1And2(const position &_p1, const position &_p2) {
	p1 = _p1;
	p2 = _p2;
	genRectProperties(rectangle->pos, rectangle->sz, rectangle->rot);
	rectangle->setAll(rectangle->pos, rectangle->sz, rectangle->rot);
}
void W::Line::nudge(const position &delta) {
	p1 += delta;
	p2 += delta;
	rectangle->nudge(delta);
}
void W::Line::genRectProperties(position &pos, size &sz, float &rot) {
	float lineLength = sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y));
	
	sz.height = lineWidth;
	sz.width = lineLength;
	
	pos.x = p1.x + (p2.x - p1.x)*0.5 - lineLength/2;
	pos.y = p1.y + (p2.y - p1.y)*0.5 - lineWidth/2;
	
	rot = asin((p2.y-p1.y) / lineLength) * RAD2DEG;
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
	const position &_p,
	const size &_sz,
	float _opac,
	float _rot,
	int _lay,
	BlendMode::T _blend
) :
	pos(_p),
	sz(_sz),
	rot(_rot),
	opac(_opac)
{
	dSprite = new DSprite(_v, _t, _p, _sz, _opac, _rot, _lay, _blend);
}
W::Sprite::~Sprite()
{
	delete dSprite;
}
void W::Sprite::setPos(const position &_p) {
	dSprite->setPosSzRot(pos = _p, sz, rot);
}
void W::Sprite::setSz(const size &_s) {
	dSprite->setPosSzRot(pos, sz = _s, rot);
}
void W::Sprite::setRot(float _r) {
	dSprite->setPosSzRot(pos, sz, rot = _r);
}
void W::Sprite::setAll(const position &_p, const size &_s, float _r) {
	dSprite->setPosSzRot(pos = _p, sz = _s, rot = _r);
}
void W::Sprite::nudge(const position &delta) {
	pos += delta;
	dSprite->setPosSzRot(pos, sz, rot);
}
void W::Sprite::setOpacity(float _o) {
	dSprite->setOpac(_o);
}
void W::Sprite::setLayer(int l) {
	dSprite->setLayer(l);
}
void W::Sprite::setBlendMode(BlendMode::T m) {
	dSprite->setBlendMode(m);
}	


/*******************************/
/*** DEquiTri implementation ***/
/*******************************/

#pragma mark - DEquiTri

//W::DEquiTri::DEquiTri(View *_v, const position &_pos, float _rad, const Colour &_col, float _rot) :
//	DObj(_v, _col, 3),
//	pos(_pos),
//	radius(_rad),
//	col(_col),
//	rotation(_rot),
//	tex(Texture::_whiteTexture)
//{
//	tex->incrementUsageCount();
//	updateVertices();
//	updateColour();
//	updateTexcoords();
//}
//W::DEquiTri::~DEquiTri()
//{
//	tex->decrementUsageCount();
//}
//void W::DEquiTri::updateVertices() {
//	v3f &v1 = vert_ptr[0],
//		&v2 = vert_ptr[1],
//		&v3 = vert_ptr[2];
//	
//	v1.z = v2.z = v3.z = 0;
//	
//	float sideLength = radius * ROOT3;
//	float apothem = radius * 0.5;
//	
//	v1.x = 0,               v1.y = -radius;
//	v2.x = sideLength*0.5,  v2.y = apothem;
//	v3.x = -sideLength*0.5, v3.y = apothem;
//	
//	if (rotation != 0.0) {
//		float rot = rotation * DEG2RAD;
//		float cosR = cos(rot), sinR = sin(rot);
//		v3f v1r, v2r, v3r;
//		
//		v1r.x = v1.x*cosR - v1.y*sinR,
//		v2r.x = v2.x*cosR - v2.y*sinR;
//		v3r.x = v3.x*cosR - v3.y*sinR;
//		
//		v1r.y = v1.x*sinR + v1.y*cosR;
//		v2r.y = v2.x*sinR + v2.y*cosR;
//		v3r.y = v3.x*sinR + v3.y*cosR;
//		
//		v1.x = v1r.x, v1.y = v1r.y;
//		v2.x = v2r.x, v2.y = v2r.y;
//		v3.x = v3r.x, v3.y = v3r.y;
//	}
//	
//	v1.x += pos.x, v1.y += pos.y;
//	v2.x += pos.x, v2.y += pos.y;
//	v3.x += pos.x, v3.y += pos.y;
//}
//void W::DEquiTri::updateTexcoords() {
//	float tA = tex->floatCoordA(0),
//		tB = tex->floatCoordB(0),
//		tC = tex->floatCoordC(tex->sz.width),
//		tD = tex->floatCoordD(tex->sz.height);
//	
//	texcoord_ptr[0].x = tA, texcoord_ptr[0].y = tB;
//	texcoord_ptr[1].x = tA, texcoord_ptr[1].y = tD;
//	texcoord_ptr[2].x = tC, texcoord_ptr[2].y = tD;
//}

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
