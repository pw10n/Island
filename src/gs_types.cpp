#include "gs_types.h"
#include "types.h"

gamestate_t::gamestate_t(uint32_t time, char *map)
			: _tick(time), _state(GSSTATE_INACTIVE){
				
	if(strlen(map) > GS_MAX_MAP_NAME_LEN){
		_map = "";
		_state = GSSTATE_INVALID;
		throw gException("ERROR: Map name length exceeds max len.");
	}
	strcpy(_map, map);

}

void gamestate_t::tick(uint32_t time){
	// TODO: implement method	
}

void gamestate_t::addPlayer(const playerstate_t &player){
	
}

