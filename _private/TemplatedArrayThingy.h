/*
 * W - a tiny 2D game development library
 *
 * ==========================
 *  TemplatedArrayThingy.h
 * ==========================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef W_TemplatedArrayThingy_h
#define W_TemplatedArrayThingy_h

//#define __W_DEBUG
#include <iostream>
#include "DebugMacro.h"

#define COMPACTION_THRESHOLD 0.3	// Compaction occurs if used_size < capacity * threshold
#define COMPACTION_AMOUNT 0.5		// For obvious reasons, compaction_amount must be >= threshold


template<typename T>
class TemplatedArrayThingy {
public:
	TemplatedArrayThingy(int initial_size);
	~TemplatedArrayThingy();
	
	void expand();
	void contract();
	
	int capacity;
	int used_size;
	
	T *array;
};


template<typename T>
TemplatedArrayThingy<T>::TemplatedArrayThingy(int initial_size) : array(NULL), used_size(0)
{
	w_dout << "  TemplatedArrayThingy() (" << this << ", size: " << initial_size << " x " << sizeof(T) << " bytes)\n";
	array = (T*) malloc(sizeof(T) * initial_size);
	capacity = initial_size;
}

template<typename T>
TemplatedArrayThingy<T>::~TemplatedArrayThingy()
{
	w_dout << " ~TemplatedArrayThingy() (" << this << ")\n";
	if (array) free(array);
}

template<typename T>
void TemplatedArrayThingy<T>::expand() {
	w_dout << " TemplatedArrayThingy<T>::expand()\n";
	w_dout << "  capacity: " << capacity << " -> " << capacity*2 << "\n";
	capacity *= 2;
	T *new_array = (T*) malloc(sizeof(T) * capacity);
	if (array) {
		for (int i=0; i < used_size; ++i)
			new_array[i] = array[i];
		free(array);
	}
	array = new_array;
}

template<typename T>
void TemplatedArrayThingy<T>::contract() {
	if (used_size > capacity * COMPACTION_THRESHOLD)  return;
	
	w_dout << " TemplatedArrayThingy<T>::contract()\n";
	w_dout << "  capacity: " << capacity << " -> " << capacity * 0.5 << "\n";
	capacity *= 0.5;
	T *new_array = (T*) malloc(sizeof(T) * capacity);
	if (array) {
		for (int i=0; i < used_size; ++i)
			new_array[i] = array[i];
		free(array);
	}
	array = new_array;
}

#endif
