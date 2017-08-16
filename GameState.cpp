/*
 * W - a tiny 2D game development library
 *
 * =================
 *  GameState.cpp
 * =================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "W.h"

/*************************************/
/*** External W state push/pop fns ***/
/*************************************/

W::Returny W::KillerReturny(ReturnyType::Killer);
W::Returny W::EmptyReturny(ReturnyType::Empty);

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
W::Returny W::GameState::_returny(ReturnyType::Empty);
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
}
void W::GameState::removeView(View *v) {
	_vlist.remove(v);
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
		delete last_gs;
		Messenger::_gamestateDestroyed(last_gs);
		_gsStack.pop_back();
		if (_gsStack.size()) {
			GameState *new_gs = _gsStack.back();
			Messenger::_setActiveGamestate(new_gs);
			new_gs->resume(&_returny);
		}
	} while (_pop && _gsStack.size());
}

void W::GameState::_updateAllDObjTexcoords() {
	for (std::vector<GameState*>::iterator it = _gsStack.begin(); it < _gsStack.end(); ++it)
		(*it)->_updateAllDObjTexcoordsInThisState();
}
void W::GameState::_updateAllDObjTexcoordsInThisState() {
	for (Viewlist::iterator it = _vlist.begin(); it != _vlist.end(); ++it)
		(*it)->_updateDObjTexcoords();
}
