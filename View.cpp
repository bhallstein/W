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
#include "DObj.h"
#include "Controller.h"

#include "oglInclude.h"

#define GL_ARRAY_INITIAL_SIZE 256
#define SECONDS_PER_COMPACT 5
#define COMPACTION_THRESHOLD 0.3	// Compact if (used_size <= CT * size)


/***************************/
/*** View implementation ***/
/***************************/

W::View::View(Positioner *_pos) :
	_positioner(_pos),
	firstDObj(NULL), lastDObj(NULL),
	vertArray(NULL),
	colArray(NULL),
	texcoordArray(NULL),
	array_size(0),
	array_used_size(0),
	frameCount(0)
{
	if (_positioner) _updatePosition();
	
	array_size = GL_ARRAY_INITIAL_SIZE * 0.5;
	increaseArraySize();
}
W::View::~View()
{
	if (_positioner) delete _positioner;
	if (vertArray) {
		delete vertArray;
		delete colArray;
		delete texcoordArray;
	}
}

void W::View::_updatePosition() {
	_updatePosition(_controller.window->getSize());
}
void W::View::_updatePosition(const size &winsize) {
	if (_positioner)
		rct = _positioner->refresh(winsize);
	updatePosition(winsize);
}

W::EventPropagation::T W::View::mouseEvent(Event *ev) {
	ev->pos -= rct.pos;
	processMouseEvent(ev);
	return EventPropagation::ShouldStop;
}

void W::View::_subscribeToMouseEvents() {
	Messenger::subscribeToMouseEvents(Callback(&View::mouseEvent, this), &rct);
}
void W::View::_unsubscribeFromMouseEvents() {
	Messenger::unsubscribeFromMouseEvents(this);
}

void W::View::_addDObj(W::DObj *d) {
	int l = d->arrayLength();
	
	// Expand glData to accomm D if necessary
	while(array_used_size + l > array_size) increaseArraySize();
	int i = array_used_size;
	array_used_size += l;
	
	// Give D glData array pointers & index
	d->_setArrayPtrs(vertArray, colArray, texcoordArray, i);
	
	// Add DObj to chain
	if (!firstDObj)
		firstDObj = lastDObj = d;
	else {
		lastDObj->nextDObj = d;
		d->prevDObj = lastDObj;
		lastDObj = d;
	}
}
void W::View::_remDObj(W::DObj *d) {
	int l = d->arrayLength();
	
	// If D is last obj, just decrease array_used_size by D.length + D.preceding_removed
	if (!d->nextDObj)
		array_used_size -= l + d->_preceding_removed;

	// Otherwise, increment D.next's preceding_removed
	else
		if (d->nextDObj) d->nextDObj->_preceding_removed += l + d->_preceding_removed;
	
	// Unlink D from chain
	if (!d->prevDObj) firstDObj = d->nextDObj;	// If DObj was first in chain, set view.first to D.next
	else d->prevDObj->nextDObj = d->nextDObj;	// Otherwise set D.prev's next to D.next
	
	if (!d->nextDObj) lastDObj = d->prevDObj;	// If DObj was last in chain, set view.last to this.prev
	else d->nextDObj->prevDObj = d->prevDObj;	// Otherwise, set D.next's prev to D.prev
}

void W::View::_draw(const size &winSz) {
	size sz = rct.sz;			// Note: these properties are copied, to avoid artifacts in the event
	position pos = rct.pos;		// that the view's position is updated while its objects are being drawn
	
	// Set up OGL: scissor to view bounds, translate to view pos w/ modelview matrix
	glScissor(pos.x, winSz.height - pos.y - sz.height, sz.width, sz.height);
	glLoadIdentity();
	glTranslatef(pos.x + _offset.x, pos.y + _offset.y, 0);
	
	// Users can write custom OpenGL code
	customOpenGLDrawing();
	
	// Submit data arrays to opengl
	glVertexPointer(3, GL_FLOAT, 0, vertArray);
	glColorPointer(4, GL_FLOAT, 0, colArray);
	glTexCoordPointer(2, GL_FLOAT, 0, texcoordArray);
	glDrawArrays(GL_TRIANGLES, 0, array_used_size);
	
	// Call compact() every so often
	if (frameCount++ == 60*SECONDS_PER_COMPACT)
		frameCount = 0, compact();
}

void W::View::increaseArraySize() {
	v3f *new_vertArray     = (v3f*) malloc(sizeof(v3f) * array_size * 2);
	c4f *new_colArray      = (c4f*) malloc(sizeof(c4f) * array_size * 2);
	t2f *new_texcoordArray = (t2f*) malloc(sizeof(t2f) * array_size * 2);
	if (vertArray) {
		for (int i=0; i < array_used_size; ++i) {
			new_vertArray[i]     = vertArray[i];
			new_colArray[i]      = colArray[i];
			new_texcoordArray[i] = texcoordArray[i];
		}
		free(vertArray);
		free(colArray);
		free(texcoordArray);
	}
	vertArray     = new_vertArray;
	colArray      = new_colArray;
	texcoordArray = new_texcoordArray;
	array_size *= 2;
	
	updateDObjPtrs();
}

void W::View::compact() {
	// Remove padded areas
	int runningMoveBackAmount = 0;
	for (DObj *d = firstDObj; d; d = d->nextDObj) {
		runningMoveBackAmount += d->_preceding_removed;
		d->_preceding_removed = 0;
		d->_moveBackBy(runningMoveBackAmount);
	}
	array_used_size -= runningMoveBackAmount;

	// Compact the data arrays if their used_size has smallened sufficiently
	if (array_used_size <= array_size * COMPACTION_THRESHOLD) {
		v3f *new_vertArray     = (v3f*) malloc(sizeof(v3f) * array_size / 2);
		c4f *new_colArray      = (c4f*) malloc(sizeof(c4f) * array_size / 2);
		t2f *new_texcoordArray = (t2f*) malloc(sizeof(t2f) * array_size / 2);
		if (vertArray) {
			for (int i=0; i < array_used_size; ++i) {
				new_vertArray[i]     = vertArray[i];
				new_colArray[i]      = colArray[i];
				new_texcoordArray[i] = texcoordArray[i];
			}
			free(vertArray);
			free(colArray);
			free(texcoordArray);
		}
		vertArray     = new_vertArray;
		colArray      = new_colArray;
		texcoordArray = new_texcoordArray;
		array_size *= 0.5;
		
		updateDObjPtrs();
	}
}
void W::View::updateDObjPtrs() {
	for (DObj *d = firstDObj; d; d = d->nextDObj)
		d->_setArrayPtrs(vertArray, colArray, texcoordArray);
}
void W::View::_updateDObjTexcoords() {
	for (DObj *d = firstDObj; d; d = d->nextDObj)
		d->updateTexcoords();
}
void W::View::dumpDObjs() {
	std::cout << "  array_size:" << array_size << " used_size:" << array_used_size << "\n";
	for (DObj *d = firstDObj; d; d = d->nextDObj)
		std::cout << "  " << d->str() << "\n";
}
