#include "W.h"
#include "DrawnObj.h"
#include "oglDrawHelpers.h"

W::View::View(Positioner *_pos) :
	_positioner(_pos)
{
	if (_positioner)
		_updatePosition(_window->getDimensions());
}
W::View::~View()
{
	if (_positioner) delete _positioner;
	
	// GameState::removeView() must first be called, to safely remove the view (& so all its DrawnObjs)
	// from the in-flight graphics data.
	// Therefore we can simply deallocate all DrawnObjs the view holds references to.
	for (std::map<int, DO_list>::iterator it = scene.begin(); it != scene.end(); it++) {
		DO_list &l = it->second;
		for (DO_list::iterator itl = l.begin(); itl < l.end(); ++itl)
			delete *itl;
	}
	for (std::vector<DOAndLayer>::iterator it = newDOs.begin(); it < newDOs.end(); it++)
		delete it->DO;
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
	return EventPropagation::SHOULD_STOP;
}

void W::View::_subscribeToMouseEvents() {
	Messenger::subscribeToMouseEvents(Callback(&View::receiveEvent, this), &rct);
}
void W::View::_unsubscribeFromMouseEvents() {
	Messenger::unsubscribeFromMouseEvents(this);
}

void W::View::addDO(DrawnObj *_obj, int layer) {
	newDOs.push_back(DOAndLayer(_obj, layer));
}
void W::View::removeDO(DrawnObj *_obj) {
	deletedDOs.push_back(_obj);
}
void W::View::_markDOAsDirty(DrawnObj *_obj) {
	dirtyDOs.push_back(_obj);
}
void W::View::_updateDOs() {
	// Remove deleted D.O.s
	for (DO_list::iterator it = deletedDOs.begin(); it < deletedDOs.end(); it++)
		_removeDO(*it);
	deletedDOs.clear();
	
	// Add new D.O.s
	for (std::vector<DOAndLayer>::iterator it = newDOs.begin(); it < newDOs.end(); it++)
		scene[it->layer].push_back(it->DO);
	newDOs.clear();
	
	// Update dirty D.O.s
	for (DO_list::iterator it = dirtyDOs.begin(); it < dirtyDOs.end(); it++)
		(*it)->_updateValues();
	dirtyDOs.clear();
}

void W::View::_removeDO(DrawnObj *_obj) {
	for (std::map<int, DO_list>::iterator itm = scene.begin(); itm != scene.end(); ) {
		DO_list &vec = itm->second;
		for (DO_list::iterator itv = vec.begin(); itv < vec.end(); )
			if (*itv == _obj) {
				delete *itv;
				itv = vec.erase(itv);
			}
			else ++itv;
		if (!vec.size())
			scene.erase(itm++);
		else
			++itm;
	}
}

void W::View::_draw(const size &winSz) {
	size sz = rct.sz;			// Note: these properties are copied, to avoid artifacts in the event
	position pos = rct.pos;		// that the view's position is updated while its objects are being drawn

	glScissor(pos.x, winSz.height - pos.y - sz.height, sz.width, sz.height);
	
	// Users can write custom OpenGL code to draw in the background
	performOpenGLBackgroundDrawing();
	
	// Draw all DOs
	for (std::map<int, DO_list>::iterator itm = scene.begin(); itm != scene.end(); itm++) {
		DO_list &vec = itm->second;
		for (DO_list::iterator itv = vec.begin(); itv != vec.end(); itv++) {
			DrawnObj *obj = *itv;
			if (obj->type == DrawnObj::RECT) {
				DrawnRect *drect = (DrawnRect*) obj;
				position &objPos = drect->rct.pos;
				size &objSz = drect->rct.sz;
				drawRect(objPos.x + pos.x + _offset.x, objPos.y + pos.y + _offset.y, objSz.width, objSz.height, drect->col, drect->rot);
			}
			else if (obj->type == DrawnObj::TEXT) {
				DrawnText *dtext = (DrawnText*) obj;
				position &objPos = dtext->rct.pos;
				drawText(objPos.x + pos.x + _offset.x, objPos.y + pos.y + _offset.y, dtext->col, dtext->txt.c_str(), dtext->r_align);
			}
			else if (obj->type == DrawnObj::IMAGE) {
				DrawnImage *dimg = (DrawnImage*) obj;
				position &objPos = dimg->rct.pos;
				size &objSz = dimg->rct.sz;
				drawImage(objPos.x + pos.x + _offset.x, objPos.y + pos.y + _offset.y, objSz.width, objSz.height, dimg->getTex(), dimg->opac);
			}
		}
	}
}
