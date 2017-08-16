/*
 * W - a tiny 2D game development library
 *
 * ============
 *  View.cpp
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "W.h"
#include "StorageObj.h"
#include "Drawable.h"
#include "W_internal.h"

#include "oglInclude.h"

//#define __W_DEBUG
#include "DebugMacro.h"

#define GL_ARRAY_INITIAL_SIZE 256
#define SECONDS_PER_COMPACT 10
#define COMPACTION_THRESHOLD 0.3	// View will compact if (used_size <= CT * size)


#pragma mark - Layer

/*************/
/*** Layer ***/
/*************/

W::Layer::Layer()
{
	w_dout << " Layer() (" << this << ")\n";
}
W::Layer::~Layer()
{
	w_dout << " ~Layer() (" << this << ")\n";
}
void W::Layer::compact() {
	std::map<BlendMode::T, StorageObjForColouredShapes*>::iterator itCGroups;
	for (itCGroups = cGroups.begin(); itCGroups != cGroups.end(); ++itCGroups) {
		w_dout << "  compacting coloured shapes group for blend mode " << itCGroups->first << "\n";
		itCGroups->second->compact();
	}
	std::map<BlendMode::T, std::map<TextureAtlas*, StorageObjForTexturedShapes*> >::iterator itTGroups;
	for (itTGroups = tGroups.begin(); itTGroups != tGroups.end(); ++itTGroups) {
		std::map<TextureAtlas*, StorageObjForTexturedShapes*>::iterator itSObjs;
		std::map<TextureAtlas*, StorageObjForTexturedShapes*> &sObjsMap = itTGroups->second;
		for (itSObjs = sObjsMap.begin(); itSObjs != sObjsMap.end(); ++itSObjs) {
			w_dout << "  compacting textured shapes group for blend mode " << itTGroups->first << " and tex atlas " << itSObjs->first << "\n";
			itSObjs->second->compact();
		}
	}
}


#pragma mark - View

/***************************/
/*** View implementation ***/
/***************************/

W::View::View(const Positioner *_pos) :
	_positioner(NULL),
	frameCounter(0)
{
	if (_pos) {
		_positioner = new Positioner(*_pos),
		_updatePosition();
	}
}
W::View::~View()
{
	if (_positioner) delete _positioner;

	// Delete all storage objects in all layers
	for (std::map<int, Layer>::iterator it = layers.begin(); it != layers.end(); ++it) {
		Layer &l = it->second;
		
		// Delete all coloured shape storage objects
		std::map<BlendMode::T, StorageObjForColouredShapes*>::iterator it2;
		for (it2 = l.cGroups.begin(); it2 != l.cGroups.end(); ++it2)
			delete it2->second;
		
		// Delete all textured shape storage objects
		std::map<BlendMode::T, std::map<TextureAtlas*, StorageObjForTexturedShapes*>>::iterator itBl;
		for (itBl = l.tGroups.begin(); itBl != l.tGroups.end(); ++itBl) {
			std::map<TextureAtlas*, StorageObjForTexturedShapes*> &atlasToSOMap = itBl->second;
			std::map<TextureAtlas*, StorageObjForTexturedShapes*>::iterator itTA;
			for (itTA = atlasToSOMap.begin(); itTA != atlasToSOMap.end(); ++itTA)
				delete itTA->second;
		}
	}
}

void W::View::_updatePosition() {
	_updatePosition(windowSize());
}
void W::View::_updatePosition(const v2i &winsize) {
	if (_positioner)
		rct = _positioner->refresh(winsize);
	updatePosition(winsize);
}

void W::View::_convertEventCoords(Event *ev) {
	ev->pos -= rct.position;
	ev->pos -= _offset;
	convertEventCoords(ev);
}

void W::View::_draw(const v2i &winSz) {
	w_dout << "View::_draw(const size &winSz)\n";
	w_dout << " winsize: " << winSz.str() << "\n";
	w_dout << " position:" << rct.position.str() << ", size:" << rct.size.str() << ", offset:" << _offset.str() << "\n";
	
	v2i &sz = rct.size;			// Note: these properties are copied, to avoid artifacts in the event
	v2i &pos = rct.position;	// that the view's position is updated while its objects are being drawn
	
	// Set up OGL: scissor to view bounds, translate to view pos w/ modelview matrix
	glScissor(pos.a, winSz.b - pos.b - sz.b, sz.a, sz.b);
	glLoadIdentity();
	glTranslatef(pos.a + _offset.a, pos.b + _offset.b, 0);
	
	// Users can write custom OpenGL code
	customOpenGLDrawing();
	
	// To draw:
	//  - iterate over Storage Objects in layer order
	//  - switch the necessary opengl state on/off
	//  - submit data arrays
	
	// For each layer...
	for (std::map<int, Layer>::iterator it = layers.begin(); it != layers.end(); ++it) {
		Layer &l = it->second;
		
		// For each coloured shape storage object...
		std::map<BlendMode::T, StorageObjForColouredShapes*>::iterator it2;
		for (it2 = l.cGroups.begin(); it2 != l.cGroups.end(); ++it2) {
			BlendMode::T blendMode = it2->first;
			StorageObjForColouredShapes *&storageObj = it2->second;
			if (!storageObj->isEmpty()) {
				// Set up OGL features
				oglState.setBlendMode(blendMode);
				oglState.disableTexturing();
				
				// Submit data arrays
				glVertexPointer(3, GL_FLOAT, 0, storageObj->v_array.array);
				glColorPointer(4, GL_FLOAT, 0, storageObj->c_array.array);
				glDrawArrays(GL_TRIANGLES, 0, storageObj->v_array.used_size);
			}
		}
		
		// For each textured shape storage object...
		std::map<BlendMode::T, std::map<TextureAtlas*, StorageObjForTexturedShapes*>>::iterator itBl;
		for (itBl = l.tGroups.begin(); itBl != l.tGroups.end(); ++itBl) {
			std::map<TextureAtlas*, StorageObjForTexturedShapes*> &atlasToSOMap = itBl->second;
			std::map<TextureAtlas*, StorageObjForTexturedShapes*>::iterator itTA;
			for (itTA = atlasToSOMap.begin(); itTA != atlasToSOMap.end(); ++itTA) {
				BlendMode::T blendMode = itBl->first;
				TextureAtlas *texAtlas = itTA->first;
				StorageObjForTexturedShapes *&storageObj = itTA->second;
				if (!storageObj->isEmpty()) {
					// Set up OGL
					oglState.setBlendMode(blendMode);
					oglState.enableTexturing();
					
					// Bind texture atlas
					oglState.bindAtlas(texAtlas);
					
					// Submit
					glVertexPointer(3, GL_FLOAT, 0, storageObj->v_array.array);
					glColorPointer(4, GL_FLOAT, 0, storageObj->c_array.array);
					glTexCoordPointer(2, GL_FLOAT, 0, storageObj->t_array.array);
					glDrawArrays(GL_TRIANGLES, 0, storageObj->v_array.used_size);
				}
			}
		}
	}
	
	// Call compact() every so often
	if (frameCounter++ == 60*SECONDS_PER_COMPACT) {
		compactAllLayers();
		frameCounter = 0;
	}
	
	w_dout << "\n";
}

void W::View::addDrawable(DColouredShape *d) {
	w_dout << "View::addDrawable(DColouredShape*)\n";
	w_dout << " getting layer " << d->layer << "\n";
	Layer &l = layers[d->layer];
	
	w_dout << " getting storage object for blend mode " << d->blendMode << "\n";
	StorageObjForColouredShapes *&storageForBlendMode = l.cGroups[d->blendMode];
	if (!storageForBlendMode) {
		w_dout << "  not found: creating\n";
		storageForBlendMode = new StorageObjForColouredShapes();
	}
	
	w_dout << " adding coloured shape " << d << " (len: " << d->length << ") to storage object " << storageForBlendMode << " in layer " << d->layer << " with blending mode " << d->blendMode << "...\n";
	storageForBlendMode->addDrawable(d);
	
	w_dout << "\n";
}
void W::View::addDrawable(DTexturedShape *d) {
	w_dout << "View::addDrawable(DTexturedShape*)\n";
	w_dout << " getting layer " << d->layer << "\n";
	Layer &l = layers[d->layer];
	
	w_dout << " getting storage object for blend mode " << d->blendMode << " and atlas " << d->tex->atlas << "\n";
	StorageObjForTexturedShapes *&storageForBlendModeAndAtlas = l.tGroups[d->blendMode][d->tex->atlas];
	if (!storageForBlendModeAndAtlas) {
		w_dout << "  not found: creating\n";
		storageForBlendModeAndAtlas = new StorageObjForTexturedShapes();
	}
	
	w_dout << " adding DTexturedShape " << d << " (len: " << d->length << ") to storage object " << storageForBlendModeAndAtlas << " in layer " << d->layer << " with blending mode " << d->blendMode << "...\n";
	storageForBlendModeAndAtlas->addDrawable(d);
	
	w_dout << "\n";
}

void W::View::removeDrawable(DColouredShape *d) {
	w_dout << "View::removeDrawable(DColouredShape*)\n";

	w_dout << " getting storage object from D property\n";
	StorageObjForColouredShapes *storageForBlendMode = (StorageObjForColouredShapes*) d->storageObj;
	if (!storageForBlendMode) {
		w_dout << "  Error: storage object not found when removing Drawable\n";
		return;
	}
	
	w_dout << " removing drawable " << d << " from storage object " << storageForBlendMode << " in layer " << d->layer << " with blending mode " << d->blendMode << "...\n";
	storageForBlendMode->removeDrawable(d);
	
	w_dout << "\n";
}
void W::View::removeDrawable(DTexturedShape *d) {
	w_dout << "View::removeDrawable(DTexturedShape*)\n";

	w_dout << " getting storage object from D property\n";
	StorageObjForTexturedShapes *storageForBlendModeAndAtlas = (StorageObjForTexturedShapes*) d->storageObj;
	if (!storageForBlendModeAndAtlas) {
		w_dout << "  Error: storage object not found when removing Drawable\n";
		return;
	}
	
	w_dout << " removing DTexturedShape " << d << " from storage object " << storageForBlendModeAndAtlas << " in layer " << d->layer << " with blending mode " << d->blendMode << "...\n";
	storageForBlendModeAndAtlas->removeDrawable(d);
	
	w_dout << "\n";
}

void W::View::compactAllLayers() {
	w_dout << "View::compactAllLayers()\n";
	for (std::map<int, Layer>::iterator it = layers.begin(); it != layers.end(); ++it)
		it->second.compact();
}
