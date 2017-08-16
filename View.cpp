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

	// GameState::removeView() must must first be called, to safely remove the view (& so all its DrawnObjs)
	// from the in-flight graphics data.
	// Therefore we can simply deallocate all DrawnObjs the view holds references to.
	for (std::map<int, DO_list>::iterator it = scene.begin(); it != scene.end(); it++) {
		DO_list &l = it->second;
		for (DO_list::iterator it = l.begin(); it < l.end(); it++)
			delete *it;
	}
	for (std::vector<DOAndLayer>::iterator it = newDOs.begin(); it < newDOs.end(); it++)
		delete it->DO;
}

void W::View::_updatePosition(const size &winsize) {
	if (_positioner)
		rct = _positioner->refresh(winsize);
	updatePosition(winsize);
}

void W::View::receiveEvent(Event *ev) {
	ev->pos.x -= rct.pos.x;
	ev->pos.y -= rct.pos.y;
	processMouseEvent(ev);
}

void W::View::_subscribeToMouseEvents() {
	Messenger::subscribeToMouseEvents(Callback(&View::receiveEvent, this), this);
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
	for (std::map<int, DO_list>::iterator it = scene.begin(); it != scene.end(); ) {
		DO_list *vec = &it->second;
		for (DO_list::iterator itv = vec->begin(); itv < vec->end(); )
			if (*itv == _obj) {
				delete *itv;
				itv = vec->erase(itv);
			}
			else it++;
		if (!vec->size())
			scene.erase(it++);
		else
			++it;
	}
}

void W::View::_draw(const size &winSz) {
	size &sz = rct.sz;
	position &pos = rct.pos;
	glScissor(pos.x, winSz.height - pos.y - sz.height, sz.width, sz.height);
	
	// Draw all DOs
	for (std::map<int, DO_list>::iterator itm = scene.begin(); itm != scene.end(); itm++) {
		DO_list &vec = itm->second;
		for (DO_list::iterator itv = vec.begin(); itv != vec.end(); itv++) {
			DrawnObj *obj = *itv;
			if (obj->type == DrawnObj::RECT) {
				DrawnRect *drect = (DrawnRect*) obj;
				rect objPosn = drect->getPosn();
				drawRect(objPosn.pos.x + pos.x, objPosn.pos.y + pos.y, objPosn.sz.width, objPosn.sz.height, drect->getCol(), drect->getRot());
			}
			else if (obj->type == DrawnObj::TEXT) {
				DrawnText *dtext = (DrawnText*) obj;
				rect &objPosn = dtext->getPosn();
				drawText(objPosn.pos.x + pos.x, objPosn.pos.y + pos.y, dtext->getCol(), dtext->getText().c_str());
			}
			else if (obj->type == DrawnObj::IMAGE) {
				DrawnImage *dimg = (DrawnImage*) obj;
				rect &objPosn = dimg->getPosn();
				drawImage(objPosn.pos.x + pos.x, objPosn.pos.y + pos.y, objPosn.sz.width, objPosn.sz.height, dimg->getTex(), dimg->getOpacity());
			}
		}
	}
}
