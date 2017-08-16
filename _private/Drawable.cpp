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

//#define __W_DEBUG
#include "DebugMacro.h"

/*** rect coord fns ***/

#pragma mark Rect coord fns

void generateRectCoords(const W::v2f &pos, const W::v2f &size, float rotation, W::v3f *);


/*** Drawable ***/

#pragma mark - Drawable

W::Drawable::Drawable(View *_v, int _len, int _lay, BlendMode::T _blend) :
	view(_v),
	length(_len),
	layer(_lay),
	blendMode(_blend),
	preceding_free_space(0)
{
	v_array = (v3f*) malloc(sizeof(v3f) * length);
	c_array = (v4f*) malloc(sizeof(v4f) * length);
}
W::Drawable::~Drawable()
{
	free(v_array);
	free(c_array);
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
	view->addDrawable(this);
}
W::DColouredShape::~DColouredShape()
{
	view->removeDrawable(this);
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

W::DTri::DTri(View *_v, const v2f &_p1, const v2f &_p2, const v2f &_p3, const Colour &_col, int _lay, BlendMode::T _blend) :
	DColouredShape(_v, 3, _lay, _blend)
{
	setP123(_p1, _p2, _p3);
	setCol(_col);
}
void W::DTri::setP123(const v2f &p1, const v2f &p2, const v2f &p3) {
	v_array[0] = p1;
	v_array[1] = p2;
	v_array[2] = p3;
	recopyV();
}
void W::DTri::setCol(const Colour &c) {
	c_array[0] = &c;
	c_array[1] = &c;
	c_array[2] = &c;
	recopyC();
}


#pragma mark - DRect

W::DRect::DRect(View *_v, const v2f &_pos, const v2f &_sz, const Colour &_col, float _rot, int _lay, BlendMode::T _blend) :
	DColouredShape(_v, 6, _lay, _blend)
{
	setPosSzRot(_pos, _sz, _rot);
	setCol(_col);
}
void W::DRect::setPosSzRot(const v2f &_pos, const v2f &_sz, float r) {
	generateRectCoords(_pos, _sz, r, v_array);
	recopyV();
}
void W::DRect::setCol(const W::Colour &c) {
	c_array[0] = &c;
	c_array[1] = &c;
	c_array[2] = &c;
	c_array[3] = &c;
	c_array[4] = &c;
	c_array[5] = &c;
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
	t_array = (v2f*) malloc(sizeof(v2f) * length);
	
	view->addDrawable(this);
	tex->incrementUsageCount();
	tex->atlas->addDrawable(this);
}
W::DTexturedShape::~DTexturedShape()
{
	view->removeDrawable(this);
	tex->decrementUsageCount();
	tex->atlas->remDrawable(this);
	
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
void W::DTexturedShape::recopyT() {
	for (int i=0; i < length; ++i)
		tptr[i] = t_array[i];
}
void W::DTexturedShape::recopyAll() {
	recopyV();
	recopyC();
	recopyT();
}


#pragma mark - DSprite

W::DSprite::DSprite(View *_v, Texture *_t, const v2f &_p, const v2f &_sc, float _opac, float _rot, int _lay, BlendMode::T _blend) :
	DTexturedShape(_v, _t, 6, _lay, _blend)
{
	setPosScaleRot(_p, _sc, _rot);
	setOpac(_opac);
	
	regenAndCopyTexCoords();
}
void W::DSprite::setPosScaleRot(const v2f &p, const v2f &sc, float r) {
	const v2i &tSz = tex->getSize();
	generateRectCoords(p + sc * 0.5, v2f(tSz.a * sc.a, tSz.b * sc.b) - sc, r, v_array);
	#ifdef __W_DEBUG
		std::cout << "DSprite::setPosScaleRot():\n";
		for (int i=0; i < length; ++i)
			std::cout << " " << v_array[i].str() << "\n";
	#endif
	recopyV();
}
void W::DSprite::setOpac(float _opac) {
	for (int i=0; i < length; ++i) {
		c_array[i].a = c_array[i].b = c_array[i].c = 1;
		c_array[i].d = _opac;
	}
	recopyC();
}
void W::DSprite::regenAndCopyTexCoords() {
	float
		tA = tex->floatCoordA(),
		tB = tex->floatCoordB(),
		tC = tex->floatCoordC(),
		tD = tex->floatCoordD();
	W::v2f
		&tc1 = t_array[0],
		&tc2 = t_array[1],
		&tc3 = t_array[2],
		&tc4 = t_array[3],
		&tc5 = t_array[4],
		&tc6 = t_array[5];
	tc1.a = tA, tc1.b = tB;
	tc2.a = tA, tc2.b = tD;
	tc3.a = tC, tc3.b = tD;
	tc4.a = tA, tc4.b = tB;
	tc5.a = tC, tc5.b = tB;
	tc6.a = tC, tc6.b = tD;
	
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
	const W::v2f &pos, const W::v2f &sz, float rotation, W::v3f *v_array
) {
	W::v3f
		&v1 = v_array[0],
		&v2 = v_array[1],
		&v3 = v_array[2],
		&v4 = v_array[3],
		&v5 = v_array[4],
		&v6 = v_array[5];
	
	v1.c = v2.c = v3.c = v4.c = v5.c = v6.c = 0;
	
	v1.a = 0,       v1.b = 0;
	v2.a = 0,       v2.b = sz.b;
	v3.a = sz.a,    v3.b = sz.b;
	v5.a = sz.a,    v5.b = 0;
//	v1.a = 0.5,            v1.b = 0.5;
//	v2.a = 0.5,            v2.b = sz.b - 0.5;
//	v3.a = sz.a - 0.5,     v3.b = sz.b - 0.5 ;
//	v5.a = sz.a - 0.5,     v5.b = 0.5;
	
	if (rotation != 0.0) {
		float rot = rotation * DEG2RAD;
		float cosR = cos(rot), sinR = sin(rot);
		int centreX = 0.5*sz.a, centreY = 0.5*sz.b;
		
		W::v3f v1r, v2r, v3r, v5r;
		
		v1r.a = (v1.a-centreX)*cosR - (v1.b-centreY)*sinR + centreX;
		v2r.a = (v2.a-centreX)*cosR - (v2.b-centreY)*sinR + centreX;
		v3r.a = (v3.a-centreX)*cosR - (v3.b-centreY)*sinR + centreX;
		v5r.a = (v5.a-centreX)*cosR - (v5.b-centreY)*sinR + centreX;
		
		v1r.b = (v1.a-centreX)*sinR + (v1.b-centreY)*cosR + centreY;
		v2r.b = (v2.a-centreX)*sinR + (v2.b-centreY)*cosR + centreY;
		v3r.b = (v3.a-centreX)*sinR + (v3.b-centreY)*cosR + centreY;
		v5r.b = (v5.a-centreX)*sinR + (v5.b-centreY)*cosR + centreY;
		
		v1.a = v1r.a, v1.b = v1r.b;
		v2.a = v2r.a, v2.b = v2r.b;
		v3.a = v3r.a, v3.b = v3r.b;
		v5.a = v5r.a, v5.b = v5r.b;
	}
	
	v1.a += pos.a, v1.b += pos.b;
	v2.a += pos.a, v2.b += pos.b;
	v3.a += pos.a, v3.b += pos.b;
	v5.a += pos.a, v5.b += pos.b;
	
	v4.a = v1.a, v4.b = v1.b;
	v6.a = v3.a, v6.b = v3.b;
}
