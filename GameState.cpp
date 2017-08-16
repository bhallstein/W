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
	// TODO: consider cleanup here: remove any remaining views?
}

void W::GameState::addView(View *v) {
	_vlist.push_back(v);
	Messenger::subscribeToMouseEvents(Callback(&View::receiveEvent, v));
}
void W::GameState::removeView(View *v) {
	_vlist.remove(v);
	Messenger::unsubscribeFromMouseEvents(v);
}

void W::GameState::handleCloseEvent() {
	W::popState(W::KillerReturny);
}
