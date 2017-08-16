#include "W.h"

W::Returny W::KillerReturny(ReturnyType::KILLER_RETURNY);
W::Returny W::EmptyReturny(ReturnyType::EMPTY_RETURNY);


W::GameState::GameState(GameStateTranslucency _tr) :
	translucent(_tr)
{
	Messenger::_useTemporaryState();
}
W::GameState::~GameState()
{
	// bai
}

void W::GameState::addView(View *v) {
	_wv[v->_getWindow()].push_back(v);
	v->_setGameState(this);
	Messenger::subscribeToMouseEvents(Callback(&View::receiveEvent, v));
}
void W::GameState::removeView(View *v) {
	_removeFromWV(v);
	Messenger::unsubscribeFromMouseEvents(v);
}

void W::GameState::handleCloseEvent() {
	W::popState(W::KillerReturny);
}

void W::GameState::_updateWV(Window *w, View *v) {
	_removeFromWV(v);
	_wv[w].push_back(v);
}
void W::GameState::_removeFromWV(View *v) {
	for (std::map<Window*, std::list<View*> >::iterator it = _wv.begin(); it != _wv.end(); it++)
		it->second.remove(v);
}
