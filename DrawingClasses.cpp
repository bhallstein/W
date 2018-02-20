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


#pragma mark - Triangle

W::Triangle::Triangle(View *_v, v2f _p1, v2f _p2, v2f _p3, Colour _col, int _lay, BlendMode::T _blend) :
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
void W::Triangle::setP1(v2f x){ ((DTri*) dTri)->setP123(p1 = x, p2, p3); }
void W::Triangle::setP2(v2f x){ ((DTri*) dTri)->setP123(p1, p2 = x, p3); }
void W::Triangle::setP3(v2f x){ ((DTri*) dTri)->setP123(p1, p2, p3 = x); }
void W::Triangle::setP123(v2f _p1, v2f _p2, v2f _p3) {
	((DTri*) dTri)->setP123(p1 = _p1, p2 = _p2, p3 = _p3);
}
void W::Triangle::nudge(v2f delta) {
	p1 += delta, p2 += delta, p3 += delta;
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::Triangle::setCol(Colour c) {
	((DTri*) dTri)->setCol(col = c);
}
void W::Triangle::setLayer(int l) {
	((DTri*) dTri)->setLayer(l);
}
void W::Triangle::setBlendMode(BlendMode::T m) {
	((DTri*) dTri)->setBlendMode(m);
}


#pragma mark - EquiTriangle

W::EqTriangle::EqTriangle(View *_v, v2f _p, float _rad, Colour _col, float _rot, int _lay, BlendMode::T _blend) :
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
void W::EqTriangle::setPosition(v2f _p) {
	pos = _p;
	genTriProperties();
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::EqTriangle::setRadius(float _rad) {
	radius = _rad;
	genTriProperties();
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::EqTriangle::nudge(v2f delta) {
	pos += delta;
	p1 += delta, p2 += delta, p3 += delta;
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::EqTriangle::setCol(Colour _col) {
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

W::IsoTriangle::IsoTriangle(View *_v, v2f _p, v2f _sz, Colour _col, float _rot, int _lay, BlendMode::T _blend) :
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
void W::IsoTriangle::setPosition(v2f _p) {
	pos = _p;
	genTriProperties();
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::IsoTriangle::setSize(v2f _sz) {
	sz = _sz;
	genTriProperties();
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::IsoTriangle::nudge(v2f delta) {
	pos += delta;
	p1 += delta, p2 += delta, p3 += delta;
	((DTri*) dTri)->setP123(p1, p2, p3);
}
void W::IsoTriangle::setCol(Colour _col) {
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
	v2f _p,
	v2f _sz,
	Colour _col,
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
void W::Rectangle::setPos(v2f _p) {
	((DRect*) dRect)->setPosSzRot(pos = _p, size, rot);
}
void W::Rectangle::setSz(v2f _s) {
	((DRect*) dRect)->setPosSzRot(pos, size = _s, rot);
}
void W::Rectangle::setRot(float _r) {
	((DRect*) dRect)->setPosSzRot(pos, size, rot = _r);
}
void W::Rectangle::setAll(v2f _p, v2f _s, float _r) {
	((DRect*) dRect)->setPosSzRot(pos = _p, size = _s, rot = _r);
}
void W::Rectangle::nudge(v2f delta) {
	pos += delta;
	((DRect*) dRect)->setPosSzRot(pos, size, rot);
}
void W::Rectangle::setCol(Colour _c) {
	((DRect*) dRect)->setCol(col = _c);
}
void W::Rectangle::setLayer(int l) {
	((DRect*) dRect)->setLayer(l);
}
void W::Rectangle::setBlendMode(BlendMode::T m) {
	((DRect*) dRect)->setBlendMode(m);
}


#pragma mark - Line

W::Line::Line(View *_v, v2f _p1, v2f _p2, const Colour _col, float _width, int _lay, BlendMode::T _blend) :
	p1(_p1),
	p2(_p2),
	lineWidth(_width),
	col(_col)
{
	genDelta();
	dLine = new DLine(_v, p1, p2, delta, col, _lay, _blend);
}
W::Line::~Line()
{
	delete ((DLine*) dLine);
}
void W::Line::setP1(v2f _p) {
	p1 = _p;
	genDelta();
	((DLine*)dLine)->setP1P2Delta(p1, p2, delta);
}
void W::Line::setP2(v2f _p) {
	p2 = _p;
	genDelta();
	((DLine*)dLine)->setP1P2Delta(p1, p2, delta);
}
void W::Line::setP1And2(v2f _p1, v2f _p2) {
	p1 = _p1;
	p2 = _p2;
	genDelta();
	((DLine*)dLine)->setP1P2Delta(p1, p2, delta);
}
void W::Line::nudge(v2f dist) {
	p1 += dist;
	p2 += dist;
	((DLine*)dLine)->setP1P2Delta(p1, p2, delta);
}
void W::Line::genDelta() {
	// Calculate delta
	
	// First off, set delta to the vector between the 2 points
	float da = p2.a - p1.a;
	float db = p2.b - p1.b;
	
	// Rotate it by -90 degrees
	delta.a = -db;
	delta.b = da;
	
	// Scale it to length of lineWidth/2
	float lineLength = sqrt(da*da + db*db);
	delta *= (lineWidth*0.5) / lineLength;	
}
void W::Line::setCol(Colour _c) {
	((DLine*)dLine)->setCol(col = _c);
}
void W::Line::setLayer(int l) {
	((DLine*)dLine)->setLayer(l);
}
void W::Line::setBlendMode(BlendMode::T m) {
	((DLine*)dLine)->setBlendMode(m);
}


#pragma mark Circle

W::Circle::Circle(View *_v, v2f _center, float _radius, W::Colour _col, int _layer, BlendMode::T _blendMode) :
	center(_center),
	radius(_radius),
	col(_col)
{
	dCircle = new DCircle(_v, _center, _radius, _col, _layer, _blendMode);
}
W::Circle::~Circle()
{
	delete ((DCircle*) dCircle);
}
void W::Circle::setCenter(W::v2f _center) {
	((DCircle*) dCircle)->setPosRadius(center = _center, radius);
}
void W::Circle::setRadius(float _r) {
	((DCircle*) dCircle)->setPosRadius(center, radius = _r);
}
void W::Circle::setCol(W::Colour _col) {
	((DCircle*) dCircle)->setCol(col = _col);
}


#pragma mark - Sprite

W::Sprite::Sprite(
	View *_v,
	Texture *_t,
	v2f _p,
	v2f _sc,
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
void W::Sprite::setPos(v2f _p) {
	((DSprite*) dSprite)->setPosScaleRot(pos = _p, scale, rot);
}
void W::Sprite::setScale(v2f _sc) {
	((DSprite*) dSprite)->setPosScaleRot(pos, scale = _sc, rot);
}
void W::Sprite::setRot(float _r) {
	((DSprite*) dSprite)->setPosScaleRot(pos, scale, rot = _r);
}
void W::Sprite::setAll(v2f _p, v2f _sc, float _r) {
	((DSprite*) dSprite)->setPosScaleRot(pos = _p, scale = _sc, rot = _r);
}
void W::Sprite::nudge(v2f delta) {
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



#pragma mark - DText

W::RetroText::RetroText(View *_v,
                        v2f _p,
                        std::string _txt,
                        Colour _col,
                        TextAlign::T _align,
                        int _lay,
                        BlendMode::T _blend) :
  pos(_p),
  txt(downCase(_txt)),
  alignment(_align),
  col(_col)
{
  dRetroText = new DRetroText(_v, _p, txt, _col, _align, _lay, _blend);
}
W::RetroText::~RetroText()
{
  delete ((DRetroText*) dRetroText);
}
void W::RetroText::set() {
  ((DRetroText*) dRetroText)->setPosTxtAlignmentCol(pos, txt, alignment, col);
}
void W::RetroText::setPos(v2f _p) {
  pos = _p;
  set();
}
void W::RetroText::setText(std::string s) {
  txt = downCase(s);
  set();
}
void W::RetroText::setAlignment(TextAlign::T _align) {
  alignment = _align;
  set();
}
void W::RetroText::nudge(v2f delta) {
  pos += delta;
  set();
}
void W::RetroText::setCol(Colour _c) {
  col = _c;
  set();
}
void W::RetroText::setLayer(int l) {
  ((DRetroText*) dRetroText)->setLayer(l);
}
void W::RetroText::setBlendMode(BlendMode::T m) {
  ((DRetroText*) dRetroText)->setBlendMode(m);
}
