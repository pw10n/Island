
#include "gamestate.h"

gamestate::gamestate(int time) :
_tick(time), _state(GSSTATE_INACTIVE) {

}

gamestate::~gamestate(){
	_state = GSSTATE_INVALID;
	for(vector<playerstate*>::iterator it = _enemies.begin();
		it != _enemies.end(); it++){
			delete (*it);
	}
	for(vector<objectstate*>::iterator it = _objects.begin();
		it != _objects.end(); it++){
			delete (*it);
	}
}

void gamestate::draw(){
	if(GSSTATE_ACTIVE != _state)
		return;

	//TODO: do stuff
}

void gamestate::tick(uint32_t time){
	if(GSSTATE_ACTIVE != _state)
		return;

	uint32_t delta_time = time - _tick;
	_tick = time;
	
	//TODO: do stuff
}

void gamestate::addEnemy(playerstate* e){
	_enemies.push_back(e);
}

void gamestate::addObject(objectstate* o){
	_objects.push_back(o);
}

playerstate::playerstate(int time):
_tick(time), _hp(0), _mp(0),_weapon(0),
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

coord2d_t playerstate::calcHotSpot(coord2d_t hs, double dist){
	hs.x() = _pos.x()-(sin(_vel.x())*dist);
	hs.y() = -_pos.y()-(cos(_vel.x())*dist);
	return hs;
}

objectstate::objectstate():_id(0), _hp(0), _type(0){}

objectstate::objectstate(const objectstate &other){
	_id=other._id;
	_hp=other._hp;
	_type=other._type;
	_pos=other._pos;
	body=other.body;
}