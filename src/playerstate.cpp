#include "playerstate.h"
#include "gamestate.h"
#include "mdmodel.h"

extern mdmodel *playerMod;
extern gamestate *gs;

#define PLYRSTATPAS 0
#define PLYRSTATATT 1

void colliobj::cPos(coord2d_t pos){
	_pos = pos;
	cBBody();
}

void colliobj::cBBody(){
	body = bbody(_pos.x(),-_pos.y(),1.0,0,BB_CIRC);
	gs->updatBinLists(this,UPDAT);
}

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
	front = bbody(this->calcHotSpot(dummy,.3),.1,BB_CIRC);
	Animate(&playerMod->md5anim[0],&idlAni,WORLD_TIME_RESOLUTION);
	Animate(&playerMod->md5anim[1],&walAni,WORLD_TIME_RESOLUTION);
	if(_state == PLYRSTATATT){
		Animate(&playerMod->md5anim[2],&attAni,WORLD_TIME_RESOLUTION);
		if(attAni.next_frame==0) _state = PLYRSTATPAS;
	}
	cBBody();
}

coord2d_t playerstate::calcHotSpot(coord2d_t hs, double dist){
	hs.x() = _pos.x()-(sin(_vel.x())*dist);
	hs.y() = -_pos.y()-(cos(_vel.x())*dist);
	return hs;
}

void playerstate::draw(){
	glEnable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(_pos.x(), 0, -_pos.y());
    glRotatef(_vel.x()*180.0/3.1415, 0, 1, 0);
    glRotatef(180,0,1,0);
	if(_state == PLYRSTATATT)
		playerMod->draw(attAni);
	else
		playerMod->draw((_vel.y()>0)?walAni:idlAni);
	glPopMatrix();
	/*glPushMatrix();
	glTranslatef(front.vala,0,front.valb);
	glutWireSphere(front.valc,10,10);
	glPopMatrix();*/
	glDisable(GL_LIGHTING);
}	


void playerstate::cBBody(){
	body = bbody(_pos.x(),-_pos.y(),1.0,0,BB_CIRC);
	gs->updatBinLists(this,UPDAT);
}

void playerstate::cVel(coord2d_t vel){
	_vel = vel;
}
void playerstate::cPos(coord2d_t pos){
	_pos = pos;
	//cBBody();
}

void playerstate::cPos(coord2d_t pos, coord2d_t vel){
	_vel = vel;
	_pos = pos;
	cBBody();
}

bool playerstate::outbounds(){
	if(_pos.x()>50.0||_pos.x()<-50.0) return true;
	if(_pos.y()>50.0||_pos.y()<-50.0) return true;
	return false;
}

