/*
 * W - a tiny 2D game development library
 *
 * ==============
 *  Drawable.h
 * ==============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// Classes for use by user-facing, Cheshire Cat drawing classes

#ifndef __W__Drawable
#define __W__Drawable

#include "types.h"

namespace W {
	
	class View;
	class Texture;
	class StorageObjForColouredShapes;
	class StorageObjForTexturedShapes;
	
	class Drawable {
	public:
		Drawable(View *, int length, int layer, BlendMode::T);
		int length;
		int index;
		int preceding_free_space;
		
		int layer;
		BlendMode::T blendMode;
		
		View *view;
		
		v3f *vptr, *v_array;
		c4f *cptr, *c_array;
			// xptr: ptr to chunk of managed storage
			// x_array: local copy (modify this then recopy)
		
		void recopyV();
		void recopyC();
	};
	
	
	class DColouredShape : public Drawable {
	public:
		DColouredShape(View *, int _len, int _lay, BlendMode::T _bMode);
		~DColouredShape();
			// NB: the constructor only adds D to the view, setting D's
			// properties. It doesn't copy anything in - this is the subclass's job.
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		void recopyBoth();
		
		StorageObjForColouredShapes *storageObj;
		
		DColouredShape *prev, *next;
	};
	
	
	class DTri : public DColouredShape {
	public:
		DTri(View *, const position &, const position &, const position &, const Colour &, int layer, BlendMode::T);
		
		void setP123(const position &, const position &, const position &);
		void setCol(const Colour &);
	};
	
	
	class DRect : public DColouredShape {
	public:
		DRect(View *, const position &, const size &, const Colour &, float rot, int layer, BlendMode::T);
		
		void setPosSzRot(const position &, const size &, float rot);
		void setCol(const Colour &);
	};
	
	
	class DTexturedShape : public Drawable {
	public:
		DTexturedShape(View *, Texture *, int _len, int _lay, BlendMode::T _bMode);
		~DTexturedShape();
			// NB: the constructor only adds D to the view, setting D's
			// properties. It doesn't copy anything in - this is the subclass's job.
			// (Except for texcoords, which the subclass won't generally touch.)
		
		void setLayer(int);
		void setBlendMode(BlendMode::T);
		
		StorageObjForTexturedShapes *storageObj;
		Texture *tex;
		
		t2f *tptr, *t_array;
		
		void recopyT();
		void recopyAll();
		
		DTexturedShape *prev, *next;
	};
	
	
	class DSprite : public DTexturedShape {
	public:
		DSprite(View *, Texture *, const position &, const size &, float opacity, float rot, int layer, BlendMode::T);
		
		void setPosSzRot(const position &, const size &, float);
		void setOpac(float);
		
		void regenAndCopyTexCoords();
			// Generate texcoords from tex in local array & call recopyT()
	};
}

#endif
