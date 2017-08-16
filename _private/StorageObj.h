/*
 * W - a tiny 2D game development library
 *
 * ================
 *  StorageObj.h
 * ================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// StorageObjs manage backing arrays for use by Drawables

#ifndef __W__StorageObj
#define __W__StorageObj

#include "types.h"
#include "TemplatedArrayThingy.h"

namespace W {

	class DColouredShape;
	class DTexturedShape;
	
	class StorageObj {
	public:
		StorageObj();
		~StorageObj();
		TemplatedArrayThingy<v3f> v_array;
		
		virtual void print() = 0;
	};
	
	
	class StorageObjForColouredShapes : public StorageObj {
	public:
		StorageObjForColouredShapes();
		~StorageObjForColouredShapes();
		TemplatedArrayThingy<v4f> c_array;
		
		void addDrawable(DColouredShape *);
		void removeDrawable(DColouredShape *);
		
		void compact();
		
		bool isEmpty() { return firstD == NULL; }
		
		void print();
		
	private:
		void expandArrays();		// Double array size & update D ptrs
		void contractArrays();		// Halve array size & ~
		void updateDrawablePtrs();
		DColouredShape *firstD, *lastD;
	};
	
	
	class StorageObjForTexturedShapes : public StorageObj {
	public:
		StorageObjForTexturedShapes();
		~StorageObjForTexturedShapes();
		TemplatedArrayThingy<v4f> c_array;
		TemplatedArrayThingy<v2f> t_array;
		
		void addDrawable(DTexturedShape *);
		void removeDrawable(DTexturedShape *);
		
		void compact();
		
		bool isEmpty() { return firstD == NULL; }
		
		void print();
		
	private:
		void expandArrays();		// Double array size & update D ptrs
		void contractArrays();		// Halve array size & ~
		void updateDrawablePtrs();
		DTexturedShape *firstD, *lastD;
	};

}

#endif
