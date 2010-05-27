#include "playerstate.h"
playerstate::playerstate(int time):
_tick(time), _mp(0),_weapon(0),
_state(PSSTATE_INIT), _score(0){
	for (int i=0; i<PLAYERSTATE_MAXABILITY; ++i)
		_ability[i]=0;
	body = bbody(_pos.x(),-_pos.y(),1,0,BB_CIRC);
	coord2d_t dummy;
	front = bbody(this->calcHotSpot(dummy,.6),.1,BB_CIRC);
}

playerstate::playerstate(const playerstate &player){
	_tick = player._tick;
	_id = player._id;
	_hp = player._hp;
	_mp = player._mp;

	for (int i=0; i<PLAYERSTATE_MAXABILITY; ++i)
		_ability[i]=player._ability[i];

	_weapon = player._weapon;
	_pos = player._pos;
	_vel = player._vel;
	_state = player._state;
	_score = player._score;
	body = player.body;
	front = player.front;

}

void playerstate::tick(uint32_t time){
	for(; _tick<time; ++_tick){
		if (_vel.y() > DBL_EPSILON){
			_pos.x() += (-sin(_vel.x()) * _vel.y());
			_pos.y() += (cos(_vel.x()) * _vel.y());
		}
	}
	body = bbody(_pos.x(),-_pos.y(),1,0,BB_CIRC);
	coord2d_t dummy;
	front = bbody(this->calcHotSpot(dummy,.6),.1,BB_CIRC);
}

coord2d_t playerstate::calcHotSpot(coord2d_t hs, double dist){
	hs.x() = _pos.x()-(sin(_vel.x())*dist);
	hs.y() = -_pos.y()-(cos(_vel.x())*dist);
	return hs;
}