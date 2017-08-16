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


#pragma mark - Gamestate

/**********************/
/*** GameState impl ***/
/**********************/


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
