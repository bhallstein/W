/*
 * W - a tiny 2D game development library
 *
 * ==================
 *  StorageObj.cpp
 * ==================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "StorageObj.h"
#include "Drawable.h"

//#define __W_DEBUG
#include "DebugMacro.h"
#include "Log.h"

#define DATA_ARRAY_INITIAL_SIZE 32

#pragma mark StorageObj

/******************/
/*** StorageObj ***/
/******************/

W::StorageObj::StorageObj() :
	v_array(DATA_ARRAY_INITIAL_SIZE)
{
	// hi
}
W::StorageObj::~StorageObj()
{
	// bai
}


#pragma mark - StorageObjForColouredShapes

/***********************************/
/*** StorageObjForColouredShapes ***/
/***********************************/

W::StorageObjForColouredShapes::StorageObjForColouredShapes() :
	c_array(DATA_ARRAY_INITIAL_SIZE),
	firstD(NULL),
	lastD(NULL)
{
	w_dout << "  StorageObjForColouredShapes() (" << this << ")\n";
}
W::StorageObjForColouredShapes::~StorageObjForColouredShapes()
{
	w_dout << " ~StorageObjForColouredShapes (" << this << ")\n";
}
void W::StorageObjForColouredShapes::addDrawable(DColouredShape *d) {
	w_dout << "  StorageObjForColouredShapes::addDrawable()\n";

	// Expand arrays if necessary
	while(v_array.used_size + d->length > v_array.capacity) expandArrays();
	
	if (v_array.used_size != c_array.used_size || v_array.capacity != c_array.capacity) {
		std::string msg = "Error: StorageObjForColouredShapes v_array & c_array differ in size\n";
		w_dout << "   " << msg;
		log << msg;
		return;
	}
		
	// Set D's index and ptrs
	int &i = d->index = v_array.used_size;
	d->vptr = v_array.array + i;
	d->cptr = c_array.array + i;
	d->storageObj = this;
	
	w_dout << "   D (len: " << d->length << ") added to storage object, index: " << d->index << ", vptr: " << d->vptr << "\n";
	
	// Increase array used_size by D.length
	v_array.used_size += d->length;
	c_array.used_size += d->length;
	
	// Insert in chain
	if (!firstD)
		firstD = lastD = d;
	else {
		lastD->next = d;
		d->prev = lastD;
		lastD = d;
	}
	
	w_dout << "   used size now: " << v_array.used_size << ", capacity: " << v_array.capacity << "\n";
}
void W::StorageObjForColouredShapes::removeDrawable(DColouredShape *d) {
	w_dout << " StorageObjForColouredShapes::removeDrawable()\n";
	
	// Make vertices degenerate
	w_dout << "  making vertices degenerate: ";
	for (int i=0; i < d->length; ++i) {
		w_dout << i+1 << " ";
		v3f &v = d->vptr[i];
		v.x = v.y = 0;
	}
	w_dout << "\n";
	
	int &l = d->length;
	// If D was last obj, just decrease arrays' used_size by D.length + D.preceding_removed
	if (!d->next) {
		v_array.used_size -= l + d->preceding_free_space;
		c_array.used_size -= l + d->preceding_free_space;
		w_dout << "  D was last obj in chain: decreased used_size to " << v_array.used_size << "\n";
	}
	
	// Otherwise, increment D.next's preceding_free_space
	else
		if (d->next) {
			d->next->preceding_free_space += l + d->preceding_free_space;
			w_dout << "  found a following D': set its preceding_free_space to " << d->next->preceding_free_space << "\n";
		}
	
	// Unlink from chain
	w_dout << "  unlinking D from chain\n";
	if (!d->prev) firstD = d->next;		// If D was first in chain, set obj.firstD to D.next
	else d->prev->next = d->next;		// Otherwise set D.prev's next to D.next
	
	if (!d->next) lastD = d->prev;		// If DObj was last in chain, set obj.lastD to D.prev
	else d->next->prev = d->prev;		// Otherwise, set D.next's prev to D.prev
	
}
void W::StorageObjForColouredShapes::expandArrays() {
	v_array.expand();
	c_array.expand();
	updateDrawablePtrs();
}
void W::StorageObjForColouredShapes::contractArrays() {
	while (v_array.capacity > DATA_ARRAY_INITIAL_SIZE && v_array.used_size < v_array.capacity * COMPACTION_THRESHOLD) {
		v_array.contract();
		c_array.contract();
		updateDrawablePtrs();
	}
}
void W::StorageObjForColouredShapes::compact() {
	w_dout << " StorageObjForColouredShapes::compact()\n";
	w_dout << "  used size: " << v_array.used_size << "\n";
	// Remove degenerate areas
	int runningCopyBackTotal = 0;
	int &n = runningCopyBackTotal;
	for (DColouredShape *d = firstD; d; d = d->next) {
		runningCopyBackTotal += d->preceding_free_space;
		
		w_dout << "  moving D " << d << " back by " << n << "\n";
		
		if (runningCopyBackTotal) {
	        for (int i=0; i < d->length; ++i) {
				v_array.array[d->index + i - n] = v_array.array[d->index + i];
				c_array.array[d->index + i - n] = c_array.array[d->index + i];
	        }
	        d->index -= n;
			d->vptr -= n;
			d->cptr -= n;
			w_dout << "   array_index: " << d->index << ", vptr: " << d->vptr << "\n";
		}
		
		d->preceding_free_space = 0;
	}
	v_array.used_size -= runningCopyBackTotal;
	c_array.used_size -= runningCopyBackTotal;
	w_dout << "  used size now: " << v_array.used_size << "\n";
	
	// Contract arrays if used_size has decreased sufficiently
	contractArrays();
}
void W::StorageObjForColouredShapes::updateDrawablePtrs() {
	w_dout << " StorageObjForColouredShapes::updateDrawablePtrs()\n";
	w_dout << "  (v_array: " << v_array.array << ")\n";
	for (DColouredShape *d = firstD; d; d = d->next) {
		w_dout << "  D " << d << " vptr: " << d->vptr << " -> ";
		d->vptr = v_array.array + d->index;
		d->cptr = c_array.array + d->index;
		w_dout << d->vptr << "\n";
	}
}
void W::StorageObjForColouredShapes::print() {
	w_dout << " StorageObjForColouredShapes::print()\n";
	w_dout << "  storageObj: " << this << "\n";
	w_dout << "  used size: " << v_array.used_size << "\n";
	w_dout << "  vertices: \n";
	for (int i=0; i < v_array.used_size; ++i)
		w_dout << "   " << v_array.array[i].str() << "\n";
	w_dout << "  colours: \n";
	for (int i=0; i < c_array.used_size; ++i)
		w_dout << "   " << c_array.array[i].str() << "\n";
}


#pragma mark - StorageObjForTexturedShapes

/***********************************/
/*** StorageObjForTexturedShapes ***/
/***********************************/

W::StorageObjForTexturedShapes::StorageObjForTexturedShapes() :
	c_array(DATA_ARRAY_INITIAL_SIZE),
	t_array(DATA_ARRAY_INITIAL_SIZE),
	firstD(NULL),
	lastD(NULL)
{
	w_dout << "  StorageObjForTexturedShapes() (" << this << ")\n";
}
W::StorageObjForTexturedShapes::~StorageObjForTexturedShapes()
{
	w_dout << "~StorageObjForTexturedShapes() (" << this << ")\n";
}
void W::StorageObjForTexturedShapes::addDrawable(DTexturedShape *d) {
	w_dout << "  StorageObjForTexturedShapes::addDrawable()\n";
	
	// Expand arrays if necessary
	while(v_array.used_size + d->length > v_array.capacity) expandArrays();
	
	if (v_array.used_size != c_array.used_size || v_array.capacity != c_array.capacity) {
		std::string s("Error: StorageObjForTexturedShapes v_array & c_array differ in size\n");
		w_dout << "   " << s;
		log << s;
		return;
	}
	if (v_array.used_size != t_array.used_size || v_array.capacity != t_array.capacity) {
		std::string s("Error: StorageObjForTexturedShapes v_array & t_array differ in size\n");
		w_dout << "   " << s;
		log << s;
		return;
	}

	// Set D's index and ptrs
	int &i = d->index = v_array.used_size;
	d->vptr = v_array.array + i;
	d->cptr = c_array.array + i;
	d->tptr = t_array.array + i;
	d->storageObj = this;
	
	w_dout << "   D (len: " << d->length << ") added to storage object, index: " << d->index << ", vptr: " << d->vptr << "\n";
	
	// Increase array used_size by D.length
	v_array.used_size += d->length;
	c_array.used_size += d->length;
	t_array.used_size += d->length;
	
	// Insert in chain
	if (!firstD)
		firstD = lastD = d;
	else {
		lastD->next = d;
		d->prev = lastD;
		lastD = d;
	}
	
	w_dout << "   used size now: " << v_array.used_size << ", capacity: " << v_array.capacity << "\n";
}
void W::StorageObjForTexturedShapes::removeDrawable(DTexturedShape *d) {
	w_dout << "StorageObjForTexturedShapes::removeDrawable()\n";
	
	// Make vertices degenerate
	w_dout << "  making vertices degenerate: ";
	for (int i=0; i < d->length; ++i) {
		w_dout << i+1 << " ";
		v3f &v = d->vptr[i];
		v.x = v.y = 0;
	}
	w_dout << "\n";
	
	int &l = d->length;
	// If D was last obj, just decrease arrays' used_size by D.length + D.preceding_removed
	if (!d->next) {
		v_array.used_size -= l + d->preceding_free_space;
		c_array.used_size -= l + d->preceding_free_space;
		t_array.used_size -= l + d->preceding_free_space;
		w_dout << "  D was last obj in chain: decreased used_size to " << v_array.used_size << "\n";
	}
	
	// Otherwise, increment D.next's preceding_free_space
	else
		if (d->next) {
			d->next->preceding_free_space += l + d->preceding_free_space;
			w_dout << "  found a following D': set its preceding_free_space to " << d->next->preceding_free_space << "\n";
		}
	
	// Unlink from chain
	w_dout << "  unlinking D from chain\n";
	if (!d->prev) firstD = d->next;		// If D was first in chain, set obj.firstD to D.next
	else d->prev->next = d->next;		// Otherwise set D.prev's next to D.next
	
	if (!d->next) lastD = d->prev;		// If DObj was last in chain, set obj.lastD to D.prev
	else d->next->prev = d->prev;		// Otherwise, set D.next's prev to D.prev
	
}

void W::StorageObjForTexturedShapes::expandArrays() {
	v_array.expand();
	c_array.expand();
	t_array.expand();
	updateDrawablePtrs();
}
void W::StorageObjForTexturedShapes::contractArrays() {
	while (v_array.capacity > DATA_ARRAY_INITIAL_SIZE && v_array.used_size < v_array.capacity * COMPACTION_THRESHOLD) {
		v_array.contract();
		c_array.contract();
		t_array.contract();
		updateDrawablePtrs();
	}
}
void W::StorageObjForTexturedShapes::compact() {
	w_dout << " StorageObjForTexturedShapes::compact()\n";
	w_dout << "  used size: " << v_array.used_size << "\n";
	
	// Remove degenerate areas
	int runningCopyBackTotal = 0;
	int &n = runningCopyBackTotal;
	for (DTexturedShape *d = firstD; d; d = d->next) {
		runningCopyBackTotal += d->preceding_free_space;
		
		w_dout << "  moving D " << d << " back by " << n << "\n";
		
		if (runningCopyBackTotal) {
	        for (int i=0; i < d->length; ++i) {
				v_array.array[d->index + i - n] = v_array.array[d->index + i];
				c_array.array[d->index + i - n] = c_array.array[d->index + i];
				t_array.array[d->index + i - n] = t_array.array[d->index + i];
	        }
	        d->index -= n;
			d->vptr -= n;
			d->cptr -= n;
			d->tptr -= n;
			w_dout << "   array_index: " << d->index << ", vptr: " << d->vptr << "\n";
		}
		
		d->preceding_free_space = 0;
	}
	v_array.used_size -= runningCopyBackTotal;
	c_array.used_size -= runningCopyBackTotal;
	t_array.used_size -= runningCopyBackTotal;
	w_dout << "  used size now: " << v_array.used_size << "\n";
	
	// Contract arrays if used_size has decreased sufficiently
	contractArrays();
}
void W::StorageObjForTexturedShapes::updateDrawablePtrs() {
	w_dout << " StorageObjForTexturedShapes::updateDrawablePtrs()\n";
	w_dout << "  (v_array: " << v_array.array << ")\n";
	for (DTexturedShape *d = firstD; d; d = d->next) {
		w_dout << "  D " << d << " vptr: " << d->vptr << " -> ";
		d->vptr = v_array.array + d->index;
		d->cptr = c_array.array + d->index;
		d->tptr = t_array.array + d->index;
		w_dout << d->vptr << "\n";
	}
}
void W::StorageObjForTexturedShapes::print() {
	w_dout << " StorageObjForTexturedShapes::print()\n";
	w_dout << "  storageObj: " << this << "\n";
	w_dout << "  used size: " << v_array.used_size << "\n";
	w_dout << "  vertices: \n";
	for (int i=0; i < v_array.used_size; ++i)
		w_dout << "   " << v_array.array[i].str() << "\n";
	w_dout << "  colours: \n";
	for (int i=0; i < c_array.used_size; ++i)
		w_dout << "   " << c_array.array[i].str() << "\n";
	w_dout << "  texcoords: \n";
	for (int i=0; i < t_array.used_size; ++i)
		w_dout << "   " << t_array.array[i].str() << "\n";
}
