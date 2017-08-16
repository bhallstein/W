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

#define GEOM_LENGTH_FOR_RECT 6
#define RAD2DEG (180.0/M_PI)
#define DEG2RAD (M_PI/180.0)
#define ROOT3 1.732050808

#define CIRCLE_NPOINTS 15
#define GEOM_LENGTH_FOR_CIRCLE 3*CIRCLE_NPOINTS

//#define GEOM_DEBUG

/*** Quad-geom copying helper function ***/

void copyQuadGeomToArrays(
	W::v3f *vert_array, W::c4f *col_array, W::t2f *texcoord_array,
	const W::position &, const W::size &, const W::Colour &, W::Texture *,
	int texA, int texB, int texC, int texD,
	float rotation = 0
);

/***************************/
/*** DObj implementation ***/
/***************************/

W::DObj::DObj(View *_v, int _length) :
	view(_v),
	array_length(_length),
	prevDObj(NULL),
	nextDObj(NULL),
	has_set_recopy(false)
{
	array_start_index = view->_getStorageForDObjOfLength(array_length);
	// If there are no other DObjs in the view, just set view.first & view.last to this
	if (!view->firstDObj)
		view->firstDObj = view->lastDObj = this;
	// Otherwise, insert into DObj chain
	else {
		prevDObj = view->lastDObj;
		prevDObj->nextDObj = this;
		view->lastDObj = this;
	}
	setNeedsRecopy();
}
W::DObj::~DObj()
{
	// Move subsequent DObjs' array start points back by length of this Dobj
	if (nextDObj)
		nextDObj->moveBackBy(array_length);
	
	if (!prevDObj) view->firstDObj = nextDObj;	// If DObj was first in chain, set view.first to this.next
	else prevDObj->nextDObj = nextDObj;			// Otherwise set prev's next to this's next
	
	if (!nextDObj) view->lastDObj = prevDObj;	// If DObj was last in chain, set view.last to this.prev
	else nextDObj->prevDObj = prevDObj;			// Otherwise, set next's prev to this's prev
	
	view->_removeStorageForDObjOfLength(array_length);

	if (has_set_recopy) view->_unsetNeedsRecopy(this);
		// Otherwise, if the user updates the object and then deletes it, will crash
		// when the view calls recopy() on a nonexistent DObj.
}


/***********************************/
/*** _UniRectDObj implementation ***/
/***********************************/

W::_UniRectDObj::_UniRectDObj(View *_view, const position &_pos, const size &_sz, const Colour &_col, Texture *_tex, float _rot) :
	DObj(_view, GEOM_LENGTH_FOR_RECT),
	pos(_pos),
	sz(_sz),
	col(_col),
	tex(_tex),
	rotation(_rot),
	texA(0),texB(0),texC(_tex->sz.width),texD(_tex->sz.height)
{
	tex->incrementUsageCount();
}
W::_UniRectDObj::~_UniRectDObj()
{
	tex->decrementUsageCount();
}
void W::_UniRectDObj::_recopy(v3f *vert_array, c4f *col_array, t2f *texcoord_array) {
	#ifdef GEOM_DEBUG
		std::cout << "\nRecopying rect:\n";
	#endif
	copyQuadGeomToArrays(
		vert_array, col_array, texcoord_array,
		pos, sz, col, tex, texA, texB, texC, texD, rotation
	);
}


/****************************/
/*** DRect implementation ***/
/****************************/

W::DRect::DRect(View *_v, const position &_p, const size &_s, const Colour &_col, float _rot) :
	_UniRectDObj(_v, _p, _s, _col, Texture::_whiteTexture, _rot)
{
	// hai recty
}


/****************************/
/*** DLine implementation ***/
/****************************/

W::DLine::DLine(View *_v, const position &_p1, const position &_p2, const W::Colour &_col, int _lineWidth) :
	_UniRectDObj(_v, position(), size(), _col, Texture::_whiteTexture, 0),
	p1(_p1),
	p2(_p2),
	lineWidth(_lineWidth)
{
	recalculateRectProperties();
}
void W::DLine::setPos1(const W::position &_p1) {
	p1 = _p1;
	recalculateRectProperties();
	setNeedsRecopy();
}
void W::DLine::setPos2(const W::position &_p2) {
	p2 = _p2;
	recalculateRectProperties();
	setNeedsRecopy();
}
void W::DLine::setWidth(int _w) {
	lineWidth = _w;
	recalculateRectProperties();
	setNeedsRecopy();
}
void W::DLine::nudge(const W::position &x) {
	p1 += x;
	p2 += x;
	pos += x;
	setNeedsRecopy();
}
void W::DLine::recalculateRectProperties() {
	sz.height = lineWidth;
	
	float lineLength = sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y));
	sz.width = lineLength;
	
	pos.x = p1.x + (p2.x - p1.x)*0.5 - lineLength/2;
	pos.y = p1.y + (p2.y - p1.y)*0.5 - lineWidth/2;
	
	rotation = asin((p2.y-p1.y) / lineLength) * RAD2DEG;
}


/*******************************/
/*** DEquiTri implementation ***/
/*******************************/

W::DEquiTri::DEquiTri(View *_v, const position &_pos, float _rad, const Colour &_col, float _rot) :
	DObj(_v, 3),
	pos(_pos),
	radius(_rad),
	col(_col),
	rotation(_rot),
	tex(Texture::_whiteTexture)
{
	tex->incrementUsageCount();
}
W::DEquiTri::~DEquiTri()
{
	tex->decrementUsageCount();
}
void W::DEquiTri::_recopy(v3f *vert_array, c4f *col_array, t2f *texcoord_array) {
	#ifdef GEOM_DEBUG
		std::cout << "\nRecopying EquiTri...\n";
	#endif
	
	float texA = tex->floatCoordA(0),
		texB = tex->floatCoordB(0),
		texC = tex->floatCoordC(tex->sz.width),
		texD = tex->floatCoordD(tex->sz.height);
	
	v3f &v1 = vert_array[0],
		&v2 = vert_array[1],
		&v3 = vert_array[2];
	
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
	
	col_array[0] = col;
	col_array[1] = col;
	col_array[2] = col;
	
	texcoord_array[0].x = texA, texcoord_array[0].y = texB;
	texcoord_array[1].x = texA, texcoord_array[1].y = texD;
	texcoord_array[2].x = texC, texcoord_array[2].y = texD;
	
	#ifdef GEOM_DEBUG
		std::cout << " Vertices: " << v1.str() << " " << v2.str() << " " << v3.str() << "\n";
		std::cout << " Colour: " << col_array[0].str() << "\n";
		std::cout << " Tex triangle: " << texcoord_array[0].str() << " " << texcoord_array[1].str() << " " << texcoord_array[2].str() << "\n";	
	#endif
}


/******************************/
/*** DCircle implementation ***/
/******************************/

struct W::DCircle::Init {
	Init() {
		// Generate unit circle geometry
		circleGeom = (v3f*)malloc(sizeof(v3f) * CIRCLE_NPOINTS * 3);
		
		float alpha = 0, dAlpha = 2*M_PI/CIRCLE_NPOINTS;
		v3f origin = {0,0,0},
			initialExternalVertex = {0,1,0};
		
		for (int i=0; i < CIRCLE_NPOINTS; ++i) {
			v3f &v1 = circleGeom[i*3],
				&v2 = circleGeom[i*3+1],
				&v3 = circleGeom[i*3+2];
			
			alpha += dAlpha;
			
			v1 = origin;
			v2 = (i == 0 ? initialExternalVertex : circleGeom[(i-1)*3+2]);
			if (i == CIRCLE_NPOINTS-1)
				v3 = initialExternalVertex;
			else
				v3.x = sin(alpha), v3.y = cos(alpha), v3.z = 0;
		}
	}
};

W::v3f *W::DCircle::circleGeom;
W::DCircle::Init *W::DCircle::initializer = new W::DCircle::Init();

W::DCircle::DCircle(View *_v, const W::position &_centrePos, int _radius, const Colour &_col) :
	DObj(_v, GEOM_LENGTH_FOR_CIRCLE),
	centrePos(_centrePos),
	radius(_radius),
	col(_col)
{
	// Hai circle
}
void W::DCircle::_recopy(v3f *vert_array, c4f *col_array, t2f *texcoord_array) {
	Texture &tex = *Texture::_whiteTexture;
	float texA = tex.floatCoordA(0),
		texB = tex.floatCoordB(0),
		texC = tex.floatCoordC(tex.sz.width),
		texD = tex.floatCoordD(tex.sz.height);
	
	for (int i=0; i < GEOM_LENGTH_FOR_CIRCLE; ++i) {
		v3f &vA = circleGeom[i],
			&vB = vert_array[i];
		vB.x = vA.x * radius + centrePos.x;
		vB.y = vA.y * radius + centrePos.y;
		vB.z = 0;
		
		col_array[i] = col;
		
		t2f &tc = texcoord_array[i];
		if (i%3 == 0)      tc.x = texA, tc.y = texB;
		else if (i%3 == 1) tc.x = texA, tc.y = texD;
		else               tc.x = texC, tc.y = texD;	
	}
}


/***************************/
/*** DImg implementation ***/
/***************************/

W::DImg::DImg(View *_v, const position &_p, const size &_s, Texture *_tex, float _opac, float _rot) :
	_UniRectDObj(_v, _p, _s, Colour(1,1,1,_opac), _tex, _rot)
{
	// hai imagey
}


/****************************/
/*** DText implementation ***/
/****************************/

W::DText::DText(View *_v, const position &_p, const std::string &_txt, const Colour &_col, bool _r_align) :
	DObj(_v, _geomLengthForText(_txt)),
	pos(_p),
	txt(_txt),
	col(_col),
	r_align(_r_align),
	tex(Texture::_whiteTexture),
	texA(0),texB(0),texC(tex->sz.width),texD(tex->sz.height)
{
	downCase(txt);
	setPos(pos);
	tex->incrementUsageCount();
}
W::DText::~DText()
{
	tex->decrementUsageCount();
}
void W::DText::setPos(const W::position &_pos) {
	pos = _pos;
	// work out total width & hence position
	int tw = 0, c;
	for (int i=0; (c=txt[i]); ++i) tw += widthForChar(c);
	if (r_align) pos.x -= tw;
}
void W::DText::_recopy(v3f *vert_array, c4f *col_array, t2f *texcoord_array) {
	#ifdef GEOM_DEBUG
		std::cout << "\nRecopying text '" << txt << "':\n";
	#endif
	W::position letterPos = pos;
	for (int i=0, geomOffset=0, c; (c=txt[i]); ++i) {
		std::vector<rect> &rectVec = GenericRetro[c];
		for (std::vector<rect>::iterator it = rectVec.begin(); it < rectVec.end(); ++it) {
			rect &r = *it;
			copyQuadGeomToArrays(
				vert_array+geomOffset, col_array+geomOffset, texcoord_array+geomOffset,
				letterPos + r.pos, r.sz, col, tex, texA, texB, texC, texD
			);
			geomOffset += GEOM_LENGTH_FOR_RECT;
		}
		letterPos.x += widthForChar(c);
	}
}
int W::DText::_geomLengthForText(const std::string &txt) {
	int x = 0, c;
	for (int i=0; (c=txt[i]); ++i)
		x += GenericRetro[c].size();
	return x * GEOM_LENGTH_FOR_RECT;
}
int W::DText::widthForChar(char c) {
	static int defaultCharWidth = 14;
	if (c == 'i' || c == '1' || c == ':' || c == '!' || c == '.' || c == '\'') return defaultCharWidth - 4;
	else if (c == 'l') return defaultCharWidth - 2;
	return defaultCharWidth;
}


/*********************************************/
/*** Rect copying helper fn implementation ***/
/*********************************************/

#include <cmath>

void copyQuadGeomToArrays(
	W::v3f *vert_array, W::c4f *col_array, W::t2f *texcoord_array,
	const W::position &pos, const W::size &sz, const W::Colour &col, W::Texture *tex,
	int texA, int texB, int texC, int texD,
	float rotation
) {
	// Vertices
	W::v3f &v1 = vert_array[0],
		&v2 = vert_array[1],
		&v3 = vert_array[2],
		&v4 = vert_array[3],
		&v5 = vert_array[4],
		&v6 = vert_array[5];
		
	v1.z = v2.z = v3.z = v4.z = v5.z = v6.z = 0;
	
	v1.x = 0,        v1.y = 0;
	v2.x = 0,        v2.y = sz.height;
	v3.x = sz.width, v3.y = sz.height;
	v5.x = sz.width, v5.y = 0;
	
	if (rotation != 0.0) {
		rotation *= DEG2RAD;
		float cosR = cos(rotation), sinR = sin(rotation);
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
	
	// Colour
	for (int i=0; i < GEOM_LENGTH_FOR_RECT; ++i)
		col_array[i] = col;
	
	// Tex coords
	float
		tA = tex->floatCoordA(texA),
		tB = tex->floatCoordB(texB),
		tC = tex->floatCoordC(texC),
		tD = tex->floatCoordD(texD);
	W::t2f
		&tc1 = texcoord_array[0],
		&tc2 = texcoord_array[1],
		&tc3 = texcoord_array[2],
		&tc4 = texcoord_array[3],
		&tc5 = texcoord_array[4],
		&tc6 = texcoord_array[5];
	tc1.x = tA, tc1.y = tB;
	tc2.x = tA, tc2.y = tD;
	tc3.x = tC, tc3.y = tD;
	tc4.x = tA, tc4.y = tB;
	tc5.x = tC, tc5.y = tB;
	tc6.x = tC, tc6.y = tD;
	
	#ifdef GEOM_DEBUG
		std::cout << " Triangle 1: " << v1.str() << " " << v2.str() << " " << v3.str() << "\n";
		std::cout << " Triangle 2: " << v4.str() << " " << v5.str() << " " << v6.str() << "\n";
		std::cout << " Col: " << col_array[0].str() << "\n";
		std::cout << " Tex triangle 1: " << tc1.str() << ' ' << tc2.str() << ' ' << tc3.str() << "\n";
		std::cout << " Tex triangle 2: " << tc4.str() << ' ' << tc5.str() << ' ' << tc6.str() << "\n";
	#endif
}
