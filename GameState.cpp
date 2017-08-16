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
	if (!W::_locked_gfx_mutex_for_gs_popping) W::_lock_mutex(&graphics_mutex);

	_vlist.push_back(v);
	v->_subscribeToMouseEvents();
	
	if (!W::_locked_gfx_mutex_for_gs_popping) W::_unlock_mutex(&graphics_mutex);
}
void W::GameState::removeView(View *v) {
	if (!W::_locked_gfx_mutex_for_gs_popping) W::_lock_mutex(&graphics_mutex);

	_vlist.remove(v);
	v->_unsubscribeFromMouseEvents();
	
	if (!W::_locked_gfx_mutex_for_gs_popping) W::_unlock_mutex(&graphics_mutex);
}

void W::GameState::handleCloseEvent() {
	W::popState(W::KillerReturny);
}
