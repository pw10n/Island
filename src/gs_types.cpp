#include "gs_types.h"
#include "types.h"

#include <cstring>

#define __STDC_LIMIT_MACROS
#include "stdint.h"

// gamestate_t

gamestate_t::gamestate_t(uint32_t time, char *map)
			: _tick(time), _state(GSSTATE_INACTIVE){
				
	if(strlen(map) > GS_MAX_MAP_NAME_LEN){
		strcpy(_map,"");
		_state = GSSTATE_INVALID;
		throw gException("ERROR: Map name length exceeds max len.");
	}
	strcpy(_map, map);
}

void gamestate_t::tick(uint32_t time){
	// TODO: implement method	
}

void gamestate_t::addPlayer(const playerstate_t &player){
	// TODO: implement method	
}

void gamestate_t::removePlayer(uint16_t id){
	// TODO: implement method	
}

void gamestate_t::addObject(const objectstate_t &object){
	// TODO: implement method	
}

void gamestate_t::removeObject(uint16_t id){
	// TODO: implement method	
}

void gamestate_t::addWepfire(const wepfirestate_t &wepfire){
	// TODO: implement method	
}

void gamestate_t::updateState(uint8_t state){
	// TODO: implement method	
}


// playerstate_t


playerstate_t::playerstate_t(uint32_t time): _tick(time), _hp(0), _mp(0), 
					_weapon(0), _state(PSSTATE_INIT),
					_score(0){
	_id = (uint16_t) ( 
		( (double) rand() / ( (double) (RAND_MAX) + 1.0 ) )
		* UINT16_MAX ) + 1;
	
	for (int i=0; i<PLAYERSTATE_MAXABILITY; ++i)
		_ability[i]=0;
}

playerstate_t::playerstate_t(const playerstate_t &player){
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

	for( vector<gDelta_data>::const_iterator it = player._deltas.begin();
		it != player._deltas.end();
		++it )
		_deltas.push_back((*it));
}

void playerstate_t::tick(uint32_t time){
	for(; _tick<time; ++_tick){
		_pos = _pos * _vel;
	}
}

void playerstate_t::change_velocity(coord2d_t nV){
	_vel = nV;
}



