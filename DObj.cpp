/*
 * W - a simple, cross-platform 2D game develpoment library
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
#define RAD2DEG 180/M_PI

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


/******************************/
/*** DCircle implementation ***/
/******************************/

struct W::DCircle::init {
	init() {
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
W::DCircle::init *W::DCircle::Initializer = new W::DCircle::init();

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
#define DEG2RAD (3.1415926536/180.0f)

void copyQuadGeomToArrays(
	W::v3f *vert_array, W::c4f *col_array, W::t2f *texcoord_array,
	const W::position &pos, const W::size &sz, const W::Colour &col, W::Texture *tex,
	int texA, int texB, int texC, int texD,
	float rotation
) {
	// Vertices
	W::v3f v1, v2, v3, /*v4,*/ v5, v6;
	v1.z = v2.z = v3.z = /*v4.z =*/ v5.z = v6.z = 1;
	v1.x = pos.x, v1.y = pos.y;
	v2.x = pos.x, v2.y = pos.y+sz.height;
	v3.x = pos.x+sz.width, v3.y = pos.y+sz.height;
//	there is no v4. (we reuse v1.)
	v5.x = pos.x+sz.width, v5.y = pos.y;
	v6.x = pos.x+sz.width, v6.y = pos.y+sz.height;
	
	if (rotation != 0) {
		rotation *= DEG2RAD;
		float cosR = cos(rotation), sinR = sin(rotation);
		W::position transl(pos.x+sz.width/2, pos.y+sz.height/2);
		
		W::v3f v1r, v2r, v3r, v5r, v6r;
		v1r.z = v2r.z = v3r.z = v5r.z = v6r.z = 0;
		
		v1r.x = (v1.x-transl.x)*cosR - (v1.y-transl.y)*sinR + transl.x;
		v2r.x = (v2.x-transl.x)*cosR - (v2.y-transl.y)*sinR + transl.x;
		v3r.x = (v3.x-transl.x)*cosR - (v3.y-transl.y)*sinR + transl.x;
		v5r.x = (v5.x-transl.x)*cosR - (v5.y-transl.y)*sinR + transl.x;
		v6r.x = (v6.x-transl.x)*cosR - (v6.y-transl.y)*sinR + transl.x;
		
		v1r.y = (v1.x-transl.x)*sinR + (v1.y-transl.y)*cosR + transl.y;
		v2r.y = (v2.x-transl.x)*sinR + (v2.y-transl.y)*cosR + transl.y;
		v3r.y = (v3.x-transl.x)*sinR + (v3.y-transl.y)*cosR + transl.y;
		v5r.y = (v5.x-transl.x)*sinR + (v5.y-transl.y)*cosR + transl.y;
		v6r.y = (v6.x-transl.x)*sinR + (v6.y-transl.y)*cosR + transl.y;
		
		vert_array[0] = v1r;
		vert_array[1] = v2r;
		vert_array[2] = v3r;
		vert_array[3] = v1r;
		vert_array[4] = v5r;
		vert_array[5] = v6r;
	}
	else {
		vert_array[0] = v1;
		vert_array[1] = v2;
		vert_array[2] = v3;
		vert_array[3] = v1;
		vert_array[4] = v5;
		vert_array[5] = v6;
	}
	
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
		std::cout << "Triangle 1: " << t1v1.str() << " " << t1v2.str() << " " << t1v3.str() << "\n";
		std::cout << "Triangle 2: " << t2v1.str() << " " << t2v2.str() << " " << t2v3.str() << "\n";
		std::cout << "Col: " << col_array[0].str() << "\n";
		std::cout << "Tex triangle 1: " << tc1.str() << ' ' << tc2.str() << ' ' << tc3.str() << "\n";
		std::cout << "Tex triangle 2: " << tc4.str() << ' ' << tc5.str() << ' ' << tc6.str() << "\n";
	#endif
}
