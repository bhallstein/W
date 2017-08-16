#include "W.h"

W::Returny W::KillerReturny(ReturnyType::KILLER_RETURNY);
W::Returny W::EmptyReturny(ReturnyType::EMPTY_RETURNY);


void W::GameState::addView(View *v) {
	_wv[v->_getWindow()].push_back(v);
	v->_setGameState(this);
	eh.subscribeToMouseEvents(Callback(&View::receiveEvent, v));
}
void W::GameState::removeView(View *v) {
	_removeFromWV(v);
	eh.unsubscribeFromMouseEvents(v);
}

void W::GameState::handleEvent(Event *ev) {
	if (ev->type == EventType::CLOSED)
		handleCloseEvent();
	else
		eh.dispatchEvent(ev);
}
void W::GameState::handleCloseEvent() {
	W::popState(W::KillerReturny);
}

//void W::GameState::setResolution(int _w, int _h) {
//	
//}

void W::GameState::_updateWV(Window *w, View *v) {
	_removeFromWV(v);
	_wv[w].push_back(v);
}
void W::GameState::_removeFromWV(View *v) {
	for (std::map<Window*, std::list<View*> >::iterator it = _wv.begin(); it != _wv.end(); it++)
		it->second.remove(v);
}
