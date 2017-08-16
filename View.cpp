#include "W.h"
#include "DObj.h"
#include "Controller.h"

#include "oglInclude.h"

#define GL_ARRAY_INITIAL_SIZE 256

/*
 NB: there’s one major drawback to this otherwise high-performance drawing
 system: removing a single DObj invokes an O(N) copy, where N is the
 number of DObjs currently in play, since all subsequent DObjs must be
 recopied backwards in the arrays.
 
 This could be improved by having View pad empty array sections with degenerate
 triangles and perform lazy compacting, but perf is high enough for the moment
 not to bother.
*/

/**************************************/
/*** glDataArrays struct definition ***/
/**************************************/

struct W::View::glDataArrays {
	unsigned int array_size, array_used_size;
	v3f *vert_array;
	c4f *col_array;
	t2f *texcoord_array;
	glDataArrays() : array_size(0), array_used_size(0), vert_array(NULL), texcoord_array(NULL), col_array(NULL) {
		array_size = GL_ARRAY_INITIAL_SIZE / 2;
		increaseSize();
	}
	~glDataArrays() {
		if (vert_array) {
			delete vert_array;
			delete col_array;
			delete texcoord_array;
		}
	}
	void increaseSize() {
		v3f *new_vert_array     = (v3f*) malloc(sizeof(v3f) * array_size * 2);
		c4f *new_col_array      = (c4f*) malloc(sizeof(c4f) * array_size * 2);
		t2f *new_texcoord_array = (t2f*) malloc(sizeof(t2f) * array_size * 2);
		if (vert_array) {
			for (int i=0; i < array_size; ++i) {
				new_vert_array[i] = vert_array[i];
				new_col_array[i] = col_array[i];
				new_texcoord_array[i] = texcoord_array[i];
			}
			free(vert_array);
			free(col_array);
			free(texcoord_array);
		}
		vert_array     = new_vert_array;
		col_array      = new_col_array;
		texcoord_array = new_texcoord_array;
		array_size *= 2;
	}
	void compact() {
		return;
		if (array_used_size > array_size / 3) return;
		
		v3f *new_vert_array     = (v3f*) malloc(sizeof(v3f) * array_size / 2);
		c4f *new_col_array      = (c4f*) malloc(sizeof(c4f) * array_size / 2);
		t2f *new_texcoord_array = (t2f*) malloc(sizeof(t2f) * array_size / 2);
		if (vert_array) {
			for (int i=0; i < array_size; ++i) {
				new_vert_array[i] = vert_array[i];
				new_col_array[i] = col_array[i];
				new_texcoord_array[i] = texcoord_array[i];
			}
			free(vert_array);
			free(col_array);
			free(texcoord_array);
		}
		vert_array     = new_vert_array;
		col_array      = new_col_array;
		texcoord_array = new_texcoord_array;
		array_size /= 2; 
	}
};


/***************************/
/*** View implementation ***/
/***************************/

W::View::View(Positioner *_pos) :
	_positioner(_pos),
	glData(new glDataArrays),
	firstDObj(NULL),
	lastDObj(NULL)
{
	if (_positioner) _updatePosition();
}
W::View::~View()
{
	if (_positioner) delete _positioner;
	delete glData;
}

void W::View::_updatePosition() {
	_updatePosition(_controller.window->getSize());
}
void W::View::_updatePosition(const size &winsize) {
	if (_positioner)
		rct = _positioner->refresh(winsize);
	updatePosition(winsize);
}

W::EventPropagation::T W::View::receiveEvent(Event *ev) {
	ev->pos.x -= rct.pos.x;
	ev->pos.y -= rct.pos.y;
	processMouseEvent(ev);
	return EventPropagation::ShouldStop;
}

void W::View::_subscribeToMouseEvents() {
	Messenger::subscribeToMouseEvents(Callback(&View::receiveEvent, this), &rct);
}
void W::View::_unsubscribeFromMouseEvents() {
	Messenger::unsubscribeFromMouseEvents(this);
}

int W::View::_getStorageForDObjOfLength(int length) {
	while (glData->array_used_size + length > glData->array_size)
		glData->increaseSize();
	int i = glData->array_used_size;
	glData->array_used_size += length;
	return i;
}
void W::View::_removeStorageForDObjOfLength(int length) {
	glData->array_used_size -= length;
}

void W::View::_updateDObjs() {
	for (std::vector<DObj*>::iterator it = _DObjs_needing_recopy.begin(); it < _DObjs_needing_recopy.end(); ++it)
		(*it)->recopy(glData->vert_array, glData->col_array, glData->texcoord_array);
	_DObjs_needing_recopy.clear();
	glData->compact();
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
	glVertexPointer(3, GL_FLOAT, 0, glData->vert_array);
	glColorPointer(4, GL_FLOAT, 0, glData->col_array);
	glTexCoordPointer(2, GL_FLOAT, 0, glData->texcoord_array);
	glDrawArrays(GL_TRIANGLES, 0, glData->array_used_size);
	
}
