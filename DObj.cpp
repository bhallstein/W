/*
 * W - a tiny 2D game development library
 *
 * ============
 *  DObj.cpp
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "DObj.h"
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

//#define GEOM_DEBUG


/* Rect vertex copying helper fn */
 
void copyRectGeom(W::v3f *, const W::position &, const W::size &, float rotation);


/***************************/
/*** DObj implementation ***/
/***************************/

W::DObj::DObj(View *_v, const Colour &_col, int _length) :
	view(_v),
	col(_col),
	array_length(_length),
	prevDObj(NULL),
	nextDObj(NULL),
	_preceding_removed(0)
{
	view->_addDObj(this);
		// - Expands arrays to accommodate if nec.
		// - Sets DObj's array ptrs
		// - Adds DObj to chain
}
W::DObj::~DObj()
{
	makeTrianglesDegenerate();
	view->_remDObj(this);
		// - Unlinks DObj from chain
		// - Adds length to nextDObj's preceding_removed
}
void W::DObj::_setArrayPtrs(W::v3f *_base_vptr, W::c4f *_base_cptr, W::t2f *_base_tptr, int _array_index) {
	if (_array_index != -1)
		array_index = _array_index;
	
	base_vptr = _base_vptr;
	base_cptr = _base_cptr;
	base_tptr = _base_tptr;
	
	vert_ptr = base_vptr + array_index;
	col_ptr = base_cptr + array_index;
	texcoord_ptr = base_tptr + array_index;
}
void W::DObj::_moveBackBy(int n) {
	if (!n) return;
	for (int i=0; i < array_length; ++i) {
		base_vptr[array_index - n + i] = vert_ptr[i];
		base_cptr[array_index - n + i] = col_ptr[i];
		base_tptr[array_index - n + i] = texcoord_ptr[i];
	}
	array_index -= n;
	vert_ptr -= n, col_ptr -= n, texcoord_ptr -= n;
}


/*******************************/
/*** DUniRect implementation ***/
/*******************************/

W::DUniRect::DUniRect(View *_view, const position &_pos, const size &_sz, const Colour &_col, Texture *_tex, float _rot) :
	DObj(_view, _col, 6),
	pos(_pos),
	sz(_sz),
	tex(_tex),
	rotation(_rot),
	texA(0), texB(0), texC(_tex->sz.width), texD(_tex->sz.height)
{
	tex->incrementUsageCount();
}
W::DUniRect::~DUniRect()
{
	tex->decrementUsageCount();
}
void W::DUniRect::updateVertices() {
	copyRectGeom(vert_ptr, pos, sz, rotation);
}
void W::DUniRect::updateTexcoords() {
	float
		tA = tex->floatCoordA(texA),
		tB = tex->floatCoordB(texB),
		tC = tex->floatCoordC(texC),
		tD = tex->floatCoordD(texD);
	W::t2f
		&tc1 = texcoord_ptr[0],
		&tc2 = texcoord_ptr[1],
		&tc3 = texcoord_ptr[2],
		&tc4 = texcoord_ptr[3],
		&tc5 = texcoord_ptr[4],
		&tc6 = texcoord_ptr[5];
	tc1.x = tA, tc1.y = tB;
	tc2.x = tA, tc2.y = tD;
	tc3.x = tC, tc3.y = tD;
	tc4.x = tA, tc4.y = tB;
	tc5.x = tC, tc5.y = tB;
	tc6.x = tC, tc6.y = tD;
}


/****************************/
/*** DRect implementation ***/
/****************************/

W::DRect::DRect(View *_v, const position &_p, const size &_s, const Colour &_col, float _rot) :
	DUniRect(_v, _p, _s, _col, Texture::_whiteTexture, _rot)
{
	updateVertices();
	updateColour();
	updateTexcoords();
}


/***************************/
/*** DImg implementation ***/
/***************************/

W::DImg::DImg(View *_v, const position &_p, const size &_s, Texture *_tex, float _opac, float _rot) :
	DUniRect(_v, _p, _s, Colour(1,1,1,_opac), _tex, _rot)
{
	updateVertices();
	updateColour();
	updateTexcoords();
}


/****************************/
/*** DLine implementation ***/
/****************************/

W::DLine::DLine(View *_v, const position &_p1, const position &_p2, const W::Colour &_col, int _lineWidth) :
	DUniRect(_v, position(), size(), _col, Texture::_whiteTexture, 0),
	p1(_p1),
	p2(_p2),
	lineWidth(_lineWidth)
{
	regenRectProperties();
	updateColour();
	updateTexcoords();
}
void W::DLine::setP1(const W::position &_p1) {
	p1 = _p1;
	regenRectProperties();
}
void W::DLine::setP2(const W::position &_p2) {
	p2 = _p2;
	regenRectProperties();
}
void W::DLine::setWidth(int _w) {
	lineWidth = _w;
	regenRectProperties();
}
void W::DLine::nudge(const W::position &x) {
	p1 += x, p2 += x, pos += x;
	updateVertices();
}
void W::DLine::regenRectProperties() {
	sz.height = lineWidth;
	
	float lineLength = sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y));
	sz.width = lineLength;
	
	pos.x = p1.x + (p2.x - p1.x)*0.5 - lineLength/2;
	pos.y = p1.y + (p2.y - p1.y)*0.5 - lineWidth/2;
	
	rotation = asin((p2.y-p1.y) / lineLength) * RAD2DEG;
	
	updateVertices();
}


/*******************************/
/*** DEquiTri implementation ***/
/*******************************/

W::DEquiTri::DEquiTri(View *_v, const position &_pos, float _rad, const Colour &_col, float _rot) :
	DObj(_v, _col, 3),
	pos(_pos),
	radius(_rad),
	col(_col),
	rotation(_rot),
	tex(Texture::_whiteTexture)
{
	tex->incrementUsageCount();
	updateVertices();
	updateColour();
	updateTexcoords();
}
W::DEquiTri::~DEquiTri()
{
	tex->decrementUsageCount();
}
void W::DEquiTri::updateVertices() {
	v3f &v1 = vert_ptr[0],
		&v2 = vert_ptr[1],
		&v3 = vert_ptr[2];
	
	v1.z = v2.z = v3.z = 0;
	
	float sideLength = radius * ROOT3;
	float apothem = radius * 0.5;
	
	v1.x = 0,               v1.y = -radius;
	v2.x = sideLength*0.5,  v2.y = apothem;
	v3.x = -sideLength*0.5, v3.y = apothem;
	
	if (rotation != 0.0) {
		float rot = rotation * DEG2RAD;
		float cosR = cos(rot), sinR = sin(rot);
		v3f v1r, v2r, v3r;
		
		v1r.x = v1.x*cosR - v1.y*sinR,
		v2r.x = v2.x*cosR - v2.y*sinR;
		v3r.x = v3.x*cosR - v3.y*sinR;
		
		v1r.y = v1.x*sinR + v1.y*cosR;
		v2r.y = v2.x*sinR + v2.y*cosR;
		v3r.y = v3.x*sinR + v3.y*cosR;
		
		v1.x = v1r.x, v1.y = v1r.y;
		v2.x = v2r.x, v2.y = v2r.y;
		v3.x = v3r.x, v3.y = v3r.y;
	}
	
	v1.x += pos.x, v1.y += pos.y;
	v2.x += pos.x, v2.y += pos.y;
	v3.x += pos.x, v3.y += pos.y;
}
void W::DEquiTri::updateTexcoords() {
	float tA = tex->floatCoordA(0),
		tB = tex->floatCoordB(0),
		tC = tex->floatCoordC(tex->sz.width),
		tD = tex->floatCoordD(tex->sz.height);
	
	texcoord_ptr[0].x = tA, texcoord_ptr[0].y = tB;
	texcoord_ptr[1].x = tA, texcoord_ptr[1].y = tD;
	texcoord_ptr[2].x = tC, texcoord_ptr[2].y = tD;
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

W::DText::DText(View *_v, const position &_p, const std::string &_txt, const Colour &_col, TextAlign::T _al) :
	DObj(_v, _col, _geomLengthForText(downCase(_txt))),
	pos(_p),
	txt(_txt),
	col(_col),
	alignment(_al),
	tex(Texture::_whiteTexture),
	texA(0),texB(0),texC(tex->sz.width),texD(tex->sz.height)
{
	tex->incrementUsageCount();
	
	txt = downCase(txt);
	setPos(pos);
	
	updateTexcoords();
	updateColour();
}
W::DText::~DText()
{
	tex->decrementUsageCount();
}
void W::DText::setPos(const W::position &_pos) {
	pos = _pos;
	// Calculate position given total pixel width
	if (alignment == TextAlign::Right)       pos.x -= widthForStr(txt);
	else if (alignment == TextAlign::Centre) pos.x -= widthForStr(txt) * 0.5;
	updateVertices();
}
void W::DText::setAlignment(TextAlign::T _al) {
	if (_al != alignment) {
		alignment = _al;
		setPos(pos);
		updateVertices();
	}
}
void W::DText::setTxt(const char *_txt) {
	// Remove DObj
	makeTrianglesDegenerate();
	view->_remDObj(this);
	
	// Set pos property back to non-alignment-corrected state
	if (alignment == TextAlign::Right) pos.x += widthForStr(txt);
	else if (alignment == TextAlign::Centre) pos.x += widthForStr(txt) * 0.5;
	
	// Put back in virgin state
	prevDObj = nextDObj = NULL;
	_preceding_removed = 0;
	txt = _txt;
	downCase(txt);
	array_length = _geomLengthForText(txt);
	
	// Re-add
	view->_addDObj(this);
	setPos(pos);
	
	updateVertices();	// The location & length of our array chunk may have
	updateColour();		// changed, so we have to update everything.
	updateTexcoords();	//
}
void W::DText::updateVertices() {
	W::position letterPos = pos;
	for (int i=0, geomOffset=0, c; (c=txt[i]); ++i) {
		const std::vector<rect> &rectVec = GenericRetro[c];
		for (std::vector<rect>::const_iterator it = rectVec.begin(); it < rectVec.end(); ++it) {
			const rect &r = *it;
			copyRectGeom(vert_ptr+geomOffset, letterPos+r.pos, r.sz, 0);
			geomOffset += 6;
		}
		letterPos.x += widthForChar(c);
	}
}
void W::DText::updateTexcoords() {
	float
		tA = tex->floatCoordA(0),
		tB = tex->floatCoordB(0),
		tC = tex->floatCoordC(tex->sz.width),
		tD = tex->floatCoordD(tex->sz.height);
	
	for (int i=0; i < array_length; i += 6) {
		W::t2f
			&tc1 = texcoord_ptr[i],
			&tc2 = texcoord_ptr[i+1],
			&tc3 = texcoord_ptr[i+2],
			&tc4 = texcoord_ptr[i+3],
			&tc5 = texcoord_ptr[i+4],
			&tc6 = texcoord_ptr[i+5];
		
		tc1.x = tA, tc1.y = tB;
		tc2.x = tA, tc2.y = tD;
		tc3.x = tC, tc3.y = tD;
		tc4.x = tA, tc4.y = tB;
		tc5.x = tC, tc5.y = tB;
		tc6.x = tC, tc6.y = tD;
	}
}
int W::DText::_geomLengthForText(const std::string &txt) {
	int x = 0, c;
	for (int i=0; (c=txt[i]); ++i)
		x += GenericRetro[c].size();
	return x * 6;
}
int W::DText::widthForStr(const std::string &s) {
	int tw = 0, c;
	for (int i=0; (c=s[i]); ++i) tw += widthForChar(c);
	return tw;
}
int W::DText::widthForChar(char c) {
	static int defaultCharWidth = 14;
	if (c == 'i' || c == '1' || c == ':' || c == '!' || c == '.' || c == '\'') return defaultCharWidth - 4;
	else if (c == 'l') return defaultCharWidth - 2;
	return defaultCharWidth;
}


/* Rect vertex copying helper implementation */

void copyRectGeom(W::v3f *vert_ptr, const W::position &pos, const W::size &sz, float rotation) {
	W::v3f
		&v1 = vert_ptr[0],
		&v2 = vert_ptr[1],
		&v3 = vert_ptr[2],
		&v4 = vert_ptr[3],
		&v5 = vert_ptr[4],
		&v6 = vert_ptr[5];
	
	v1.z = v2.z = v3.z = v4.z = v5.z = v6.z = 0;
	
	v1.x = 0,        v1.y = 0;
	v2.x = 0,        v2.y = sz.height;
	v3.x = sz.width, v3.y = sz.height;
	v5.x = sz.width, v5.y = 0;
	
	if (rotation != 0.0) {
		float rot = rotation * DEG2RAD;
		float cosR = cos(rot), sinR = sin(rot);
		int centreX = 0.5*sz.width, centreY = 0.5*sz.height;
		
		W::v3f v1r, v2r, v3r, v5r;
		
		v1r.x = (v1.x-centreX)*cosR - (v1.y-centreY)*sinR + centreX;
		v2r.x = (v2.x-centreX)*cosR - (v2.y-centreY)*sinR + centreX;
		v3r.x = (v3.x-centreX)*cosR - (v3.y-centreY)*sinR + centreX;
		v5r.x = (v5.x-centreX)*cosR - (v5.y-centreY)*sinR + centreX;
		
		v1r.y = (v1.x-centreX)*sinR + (v1.y-centreY)*cosR + centreY;
		v2r.y = (v2.x-centreX)*sinR + (v2.y-centreY)*cosR + centreY;
		v3r.y = (v3.x-centreX)*sinR + (v3.y-centreY)*cosR + centreY;
		v5r.y = (v5.x-centreX)*sinR + (v5.y-centreY)*cosR + centreY;
		
		v1.x = v1r.x, v1.y = v1r.y;
		v2.x = v2r.x, v2.y = v2r.y;
		v3.x = v3r.x, v3.y = v3r.y;
		v5.x = v5r.x, v5.y = v5r.y;
	}
	
	v1.x += pos.x, v1.y += pos.y;
	v2.x += pos.x, v2.y += pos.y;
	v3.x += pos.x, v3.y += pos.y;
	v5.x += pos.x, v5.y += pos.y;
	
	v4.x = v1.x, v4.y = v1.y;
	v6.x = v3.x, v6.y = v3.y;
}
