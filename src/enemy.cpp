#include "enemy.h"
#include <cstdlib>
#include <time.h>

#include "types.h"
#include <math.h>
#include "collision.h"
#include "Bin.h"
#include "mdmodel.h"
#include "particle.h"

using namespace std;

#define MIN(x,y) ((x>y)?y:x)
#define MAX(x,y) ((x>y)?x:y)

#define MELEE_AWARE_DIST 7.0
#define RANGED_AWARE_DIST 8.0
#define RANGED_ATTACK_DIST 6.0
#define RANGED_FLEE_DIST 3.0

#define PSTATE_FLEEING 20
#define PSTATE_NOTFLEEING 19
extern mdmodel *enemyMod;
extern gamestate *gs;

void meleeAI::tick(uint32_t time){
	if(_pos.distanceTo((*gs->player)._pos) > MELEE_AWARE_DIST){ //unaware
		coord2d_t v = vel();
		v.x() += (double)(rand()%10-5)/5.0;
		v.y() = .004;
		cVel(v);
	}
	else { //aware
		coord2d_t v = vel();
		v.x() = atan2(_pos.x()-gs->player->_pos.x(),gs->player->_pos.y()-_pos.y())+ecc;
		if(collide(front,gs->player->body)){
			v.y() = 0;
			//attack (blood particles?)
		}else{
			v.y() = .004;
		}
		cVel(v);
	}
	if(gs->SmaPlCollision(this)&&!collide(front,gs->player->body)){
		coord2d_t v = vel();
		v.y() = -.002;
		cVel(v);
		ecc = (double)(rand()%5)/3.0-.75;
	}
	bool skip = gs->cull2(_pos);
	for(; _tick<time; ++_tick){
		if(skip&&(_tick%6!=0)) continue;
		if (_vel.y() > DBL_EPSILON){
			coord2d_t p = pos();
			p.x() += (-sin(vel().x()) * vel().y());
			p.y() += (cos(vel().x()) * vel().y());
			cPos(p);
		}
		if(outbounds()){
			coord2d_t v = vel();
			v.x() += 3.14;
			cVel(v);
		}
	}
	body = bbody(pos().x(),-pos().y(),.5,0,BB_CIRC);
	coord2d_t dummy;
	front = bbody(this->calcHotSpot(dummy,.6),.1,BB_CIRC);
	gs->updatBinLists(this,UPDAT);
	if(_vel.y()>0) Animate(&enemyMod->md5anim[1],&walAni,WORLD_TIME_RESOLUTION);
	else Animate(&enemyMod->md5anim[0],&idlAni,WORLD_TIME_RESOLUTION);
	//cBBody();
}

void meleeAI::draw(){
	glPushMatrix();
	glTranslatef(pos().x(), .6, -pos().y());
    glRotatef(_vel.x()*180.0/3.1415, 0, 1, 0);
    glRotatef(180,0,1,0);
	//if(_state == PLYRSTATATT)
	//	enemyMod->draw(attAni);
	//else
		enemyMod->draw((_vel.y()>0)?walAni:idlAni);
	glPopMatrix();
}

void rangedAI::tick(uint32_t time){
	double distto = _pos.distanceTo(gs->player->_pos);
	if(distto > RANGED_AWARE_DIST){ //unaware
		_vel.x() += (double)(rand()%10-5)/5.0;
		_vel.y() = .004;
		cooldown = 0;
	}
	else if(distto > RANGED_ATTACK_DIST){ //aware but out of range
		_vel.x() = atan2(_pos.x()-gs->player->_pos.x(),gs->player->_pos.y()-_pos.y())+ecc;
		_vel.y() = .004;
		_state = PSTATE_NOTFLEEING;
		cooldown--;
	}
	else if(distto > RANGED_FLEE_DIST && _state!=PSTATE_FLEEING){
		_vel.x() = atan2(_pos.x()-gs->player->_pos.x(),gs->player->_pos.y()-_pos.y());
		_vel.y() = .000;
		if(cooldown==0){
			if(checkLOA()){
				gs->erapid(*this);
				cooldown = 5;
			}
			else{
				_vel.x() += (ecc>0)?1.57:-1.57;
				_vel.y() = .002;
			}
		}
		else cooldown--;
	}
	else{
		_vel.x() = atan2(_pos.x()-gs->player->_pos.x(),gs->player->_pos.y()-_pos.y())+3.1415;
		_vel.y() = .004;
		_state = PSTATE_FLEEING;
		cooldown--;
	}
	if(gs->SmaPlCollision(this)) {_vel.y() = -.002; ecc = (double)(rand()%5)/3.0-.75;}
	if(_vel.y()>0) Animate(&enemyMod->md5anim[1],&walAni,WORLD_TIME_RESOLUTION);//30.0*(time-_tick));
	else Animate(&enemyMod->md5anim[0],&idlAni,WORLD_TIME_RESOLUTION);
	bool skip = gs->cull2(_pos);
	for(; _tick<time; ++_tick){
		if(skip&&(_tick%6!=0)) continue;
		if (_vel.y() > DBL_EPSILON){
			_pos.x() += (-sin(_vel.x()) * _vel.y());
			_pos.y() += (cos(_vel.x()) * _vel.y());
		}
		if(outbounds()){
			coord2d_t v = vel();
			v.x() += 3.14;
			cVel(v);
		}
	}
	if(cooldown<0) cooldown = 0;
	body = bbody(_pos.x(),-_pos.y(),.5,0,BB_CIRC);
	coord2d_t dummy;
	front = bbody(this->calcHotSpot(dummy,.6),.1,BB_CIRC);
	gs->updatBinLists(this,UPDAT);
	//cBBody();
}

void rangedAI::draw(){
	glPushMatrix();
	glTranslatef(_pos.x(), .6, -_pos.y());
    glRotatef(_vel.x()*180.0/3.1415, 0, 1, 0);
    glRotatef(180,0,1,0);
	//if(_state == PLYRSTATATT)
	//	enemyMod->draw(attAni);
	//else
		enemyMod->draw((_vel.y()>0)?walAni:idlAni);
	glPopMatrix();
}

bool rangedAI::checkLOA(){
	lineOfAtt *loa = new lineOfAtt(_pos,gs->player->_pos,_id);
	int px = _pos.x()+50; int pz = -_pos.y()+50;
	int pxa = MAX(px+6,100); int pxi = MIN(px-6,0);
	int pza = MAX(pz+6,100); int pzi = MIN(pz-6,0);
	gs->LarPaCollision(loa,pxi,pxa,pzi,pza); //already know it can't be more than 6 from this
	return !(loa->active); //if loa->active == true, something's in the way
}

void spwanEnemyHelperRandPos(playerstate* obj,
					  uint8_t hp,
					  uint8_t mp,
					  uint32_t tick){
	coord2d_t pos;
	coord2d_t vel;
	// TODO fix rand code

	vel.x() = rand()%100 - 50;
	vel.y() = rand()%100 - 50;
	pos.x() = rand()%100 - 50;
	pos.y() = rand()%100 - 50;


	spawnEnemyHelper(obj, hp, mp, tick, pos, vel);
	
}

void spawnEnemyHelper(playerstate* obj, 
					  uint8_t hp,
					  uint8_t mp,
					  uint32_t tick,
					  coord2d_t pos, 
					  coord2d_t vel){
	obj->_id = ENEMYID+(eid++);
	obj->_tick = tick;
	obj->_hp = hp;
	obj->_mp = mp;
	obj->_weapon = 0;
	obj->_state = PSTATE_AI_SEARCHING;
	obj->_score = 0;
	obj->_pos = pos;
	obj->_vel = vel;

	
	
}











