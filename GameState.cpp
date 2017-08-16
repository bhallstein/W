#include "W.h"

/*************************************/
/*** External W state push/pop fns ***/
/*************************************/

W::Returny W::KillerReturny(ReturnyType::KILLER_RETURNY);
W::Returny W::EmptyReturny(ReturnyType::EMPTY_RETURNY);

void W::pushState(GameState *g) {
	GameState::pushState(g);
}
void W::popState(W::Returny &r) {
	GameState::popState(r);
}


/**********************/
/*** GameState impl ***/
/**********************/

bool W::GameState::_pop = false;
W::Returny W::GameState::_returny(ReturnyType::EMPTY_RETURNY);
std::vector<W::GameState*> W::GameState::_gsStack;


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
	_vlist.push_back(v);
	v->_subscribeToMouseEvents();
}
void W::GameState::removeView(View *v) {
	_vlist.remove(v);
	v->_unsubscribeFromMouseEvents();
}

void W::GameState::handleCloseEvent() {
	popState(W::KillerReturny);
}

void W::GameState::pushState(GameState *g) {
	//	if (!states.empty()) states.back()->pause();
	_gsStack.push_back(g);
	Messenger::_setActiveGamestate(g);
}
void W::GameState::popState(W::Returny &r) {
	_pop = true;
	_returny = r;
}
void W::GameState::_popState() {
	do {
		_pop = false;
		GameState *last_gs = _gsStack.back();
		delete _gsStack.back();
		Messenger::_gamestateDestroyed(last_gs);
		_gsStack.pop_back();
		if (_gsStack.size()) {
			GameState *new_gs = _gsStack.back();
			Messenger::_setActiveGamestate(new_gs);
			new_gs->resume(&_returny);
		}
	} while (_pop && _gsStack.size());
}