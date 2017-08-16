/*
 * W - a tiny 2D game development library
 *
 * ================
 *  Drawable.cpp
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Drawable.h"
#include "View.h"
#include "StorageObj.h"
#include "TextureAtlas.h"


/*** rect coord fns ***/

#pragma mark Rect coord fns

void generateRectCoords(const W::position &, const W::size &, float rotation, W::v3f *);


/*** Drawable ***/

#pragma mark - Drawable

W::Drawable::Drawable(View *_v, int _len, int _lay, BlendMode::T _blend) :
	view(_v),
	length(_len),
	layer(_lay),
	blendMode(_blend),
	preceding_free_space(0)
{
	// hi
}
void W::Drawable::recopyV() { for (int i=0; i < length; ++i) vptr[i] = v_array[i]; }
void W::Drawable::recopyC() { for (int i=0; i < length; ++i) cptr[i] = c_array[i]; }


/*************************************/
/*** DColouredShape implementation ***/
/*************************************/

#pragma mark - DColouredShape

W::DColouredShape::DColouredShape(View *_v, int _len, int _lay, BlendMode::T _blend = BlendMode::Normal) :
	Drawable(_v, _len, _lay, _blend),
	prev(NULL),
	next(NULL)
{
	v_array = (v3f*) malloc(sizeof(v3f) * length);
	c_array = (c4f*) malloc(sizeof(c4f) * length);
	
	view->addDrawable(this);
}
W::DColouredShape::~DColouredShape()
{
	view->removeDrawable(this);
	
	free(v_array);
	free(c_array);
}
void W::DColouredShape::setLayer(int _layer) {
	if (layer == _layer) return;
	
	view->removeDrawable(this);
	layer = _layer;
	view->addDrawable(this);
	
	recopyBoth();
}
void W::DColouredShape::setBlendMode(BlendMode::T _mode) {
	if (blendMode == _mode)  return;

	view->removeDrawable(this);
	blendMode = _mode;
	view->addDrawable(this);
	
	recopyBoth();
}
void W::DColouredShape::recopyBoth() {
	recopyV();
	recopyC();
}


#pragma mark - DTri

W::DTri::DTri(View *_v, const position &_p1, const position &_p2, const position &_p3, const Colour &_col, int _lay, BlendMode::T _blend) :
	DColouredShape(_v, 3, _lay, _blend)
{
	setP123(_p1, _p2, _p3);
	setCol(_col);
}
void W::DTri::setP123(const W::position &p1, const W::position &p2, const W::position &p3) {
	v_array[0] = p1;
	v_array[1] = p2;
	v_array[2] = p3;
	recopyV();
}
void W::DTri::setCol(const W::Colour &c) {
	c_array[0] = c;
	c_array[1] = c;
	c_array[2] = c;
	recopyC();
}


#pragma mark - DRect

W::DRect::DRect(View *_v, const position &_p, const size &_sz, const Colour &_col, float _rot, int _lay, BlendMode::T _blend) :
	DColouredShape(_v, 6, _lay, _blend)
{
	setPosSzRot(_p, _sz, _rot);
	setCol(_col);
}
void W::DRect::setPosSzRot(const position &p, const size &s, float r) {
	generateRectCoords(p, s, r, v_array);
	recopyV();
}
void W::DRect::setCol(const W::Colour &c) {
	c_array[0] = c;
	c_array[1] = c;
	c_array[2] = c;
	c_array[3] = c;
	c_array[4] = c;
	c_array[5] = c;
	recopyC();
}


#pragma mark - DTexturedShape 

/*************************************/
/*** DTexturedShape implementation ***/
/*************************************/

W::DTexturedShape::DTexturedShape(View *_v, Texture *_t, int _len, int _lay, BlendMode::T _blend = BlendMode::Normal) :
	Drawable(_v, _len, _lay, _blend),
	tex(_t),
	prev(NULL),
	next(NULL)
{
	v_array = (v3f*) malloc(sizeof(v3f) * length);
	c_array = (c4f*) malloc(sizeof(c4f) * length);
	t_array = (t2f*) malloc(sizeof(t2f) * length);
	
	view->addDrawable(this);
	tex->incrementUsageCount();
	tex->atlas->addDrawable(this);
}
W::DTexturedShape::~DTexturedShape()
{
	view->removeDrawable(this);
	tex->decrementUsageCount();
	tex->atlas->remDrawable(this);
	
	free(v_array);
	free(c_array);
	free(t_array);
}
void W::DTexturedShape::setLayer(int _layer) {
	if (layer == _layer) return;

	view->removeDrawable(this);
	layer = _layer;
	view->addDrawable(this);
	
	recopyAll();
}
void W::DTexturedShape::setBlendMode(BlendMode::T _mode) {
	if (blendMode == _mode) return;
	
	view->removeDrawable(this);
	blendMode = _mode;
	view->addDrawable(this);
	
	recopyAll();
}
void W::DTexturedShape::recopyT() { for (int i=0; i < length; ++i) tptr[i] = t_array[i]; }
void W::DTexturedShape::recopyAll() {
	recopyV();
	recopyC();
	recopyT();
}


#pragma mark - DSprite

W::DSprite::DSprite(View *_v, Texture *_t, const position &_p, const size &_sz, float _opac, float _rot, int _lay, BlendMode::T _blend) :
	DTexturedShape(_v, _t, 6, _lay, _blend)
{
	setPosSzRot(_p, _sz, _rot);
	setOpac(_opac);
	
	regenAndCopyTexCoords();
}
void W::DSprite::setPosSzRot(const position &p, const size &s, float r) {
	generateRectCoords(p, s, r, v_array);
	recopyV();
}
void W::DSprite::setOpac(float _opac) {
	for (int i=0; i < length; ++i) {
		c_array[i].r = c_array[i].g = c_array[i].b = 1;
		c_array[i].a = _opac;
	}
	recopyC();
}
void W::DSprite::regenAndCopyTexCoords() {
	float
		tA = tex->floatCoordA(),
		tB = tex->floatCoordB(),
		tC = tex->floatCoordC(),
		tD = tex->floatCoordD();
	W::t2f
		&tc1 = t_array[0],
		&tc2 = t_array[1],
		&tc3 = t_array[2],
		&tc4 = t_array[3],
		&tc5 = t_array[4],
		&tc6 = t_array[5];
	tc1.x = tA, tc1.y = tB;
	tc2.x = tA, tc2.y = tD;
	tc3.x = tC, tc3.y = tD;
	tc4.x = tA, tc4.y = tB;
	tc5.x = tC, tc5.y = tB;
	tc6.x = tC, tc6.y = tD;
	
	recopyT();
}


#pragma mark - generateRectCoords

/*** generateRectCoords definition ***/

#include <cmath>
#ifndef M_PI
	#define M_PI 3.14159265358979
#endif
#define RAD2DEG (180.0/M_PI)
#define DEG2RAD (M_PI/180.0)

void generateRectCoords(
	const W::position &pos, const W::size &sz, float rotation, W::v3f *v_array
) {
	W::v3f
		&v1 = v_array[0],
		&v2 = v_array[1],
		&v3 = v_array[2],
		&v4 = v_array[3],
		&v5 = v_array[4],
		&v6 = v_array[5];
	
	v1.z = v2.z = v3.z = v4.z = v5.z = v6.z = 0;
	
	v1.x = 0,        v1.y = 0;
	v2.x = 0,        v2.y = sz.height;
	v3.x = sz.width, v3.y = sz.height;
	v5.x = sz.width, v5.y = 0;
	//	v1.x = 0.5,            v1.y = 0.5;
	//	v2.x = 0.5,            v2.y = sz.height - 0.5;
	//	v3.x = sz.width - 0.5, v3.y = sz.height - 0.5;
	//	v5.x = sz.width - 0.5, v5.y = 0.5;
	
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
