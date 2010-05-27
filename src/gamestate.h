
#pragma once
#include <list> //lists are faster at removing from the middle
#include <vector>
#include "types.h"
#include "stdint.h"
#include "collision.h"
#include "Bin.h"
#include "playerstate.h"
#include "particle.h"

using namespace std;

class gamestate;
class objectstate;

#define GSSTATE_INACTIVE 0
#define GSSTATE_ACTIVE 1
#define GSSTATE_INVALID 99


#define HIT_CRATE 1
#define HIT_PLAYER 2
#define HIT_HUT 3




//options for checkCollision and updatBinLists
#define COLLI 0 //regular collision detection
#define UPDAT 1 //update Bin objs list
#define REMOV 2 //remove object from Bin objs list


extern void damage(uint8_t*,int);

class Bin
{
public:
	bbody box;
	list<colliobj *> objs;

	Bin(void) {}
	Bin(bbody bod): box(bod) {}
	~Bin(void) {} //deletion of objs' contents will be handled by the gamestate vectors
	int checkPaCollision(source *src, bool hitAll);
	bool checkObCollision(colliobj *obj,int op);
	bool checkPlCollision(playerstate *pla);
};
	//used for list::remove_if to get rid of colliobj*s that aren't players or objects
	//don't know why they show up, but they do. This gets rid of them
	bool killcoto(const colliobj* obj);



class gamestate{
public:
	gamestate();
	~gamestate();

	void initBins();
	
	int GW;
	int GH;
	playerstate* player;
	vector<playerstate*> _enemies;
	vector<objectstate*> _objects;

	Bin* bins[100][100];

	uint32_t _tick;
	uint8_t _state;

	fireball_s * fbsrc;
	vector<fireball_p *> fbpar;
	int fbtim;
	explosion_s * exsrc;
	vector<particle *> expar;
	bool explo;
	smite_s * smsrc;
	vector<smite_p *> smpar;
	bool smit;
	vector<rapidfire *> rfpar;
	int beatim;
	bool bflag;
	beam * besrc;
	int rfire;

	uint32_t _lastDrawn;

	void start(int time);
	void draw();
	void tick(uint32_t time);

	bool cull(coord2d_t pos);

	void addEnemy(playerstate* e);
	void addObject(objectstate* o);

	double b2p(int i);
	void LarPaCollision(source *src,int minx,int maxx,int minz,int maxz);
	bool LarObCollision(colliobj *obj,int minx,int maxx,int minz,int maxz);
	int SmaPaCollision(source *src);
	bool SmaObCollision(colliobj *obj);
	bool SmaPlCollision(playerstate *pla);
	void updatBinLists(colliobj *obj,int op);
	//cleans up strange problems
	void janitor(int x, int z);

	void spawnFireball(){
		if (player->_mp<10) {
			return;
		}
		fbtim = 0;
		double fbx = -sin(player->_vel.x());
		double fbz = -cos(player->_vel.x());
		coord2d_t dummy;
		dummy = player->calcHotSpot(dummy,.6);
		fbsrc = new fireball_s(dummy.x(),dummy.y(),fbx/5.0,fbz/5.0,player->_id);
		for(int i=0;i<200;i++){
			fbpar.push_back(new fireball_p(fbsrc));
		}
		if (player->_mp>=10) {
			player->_mp -= 10;
			
		}
	}
	void smiteEm(int x, int y){
		if (player->_mp<50) {
			return;
		}
		player->_mp -= 50;
		//first calc target position
		double dx = ((double)x-GW/2.0)/1.7;
		double dy = (double)y-GH/2.0 + .5;
		double dz = log((double)y*2.0/GH)/-.159;
		coord2d_t targ;
		targ.x() = player->_pos.x() - dz*dx/dy;
		targ.y() = player->_pos.y() + dz;

		smit = true;
		smsrc = new smite_s(targ.x(),-targ.y(),player->_id);
		for(int i=0;i<400;i++){
			smpar.push_back(new smite_p(smsrc));
		}
	}

	void detonate(source * ws, bool splin){
		explo = true;
		exsrc = new explosion_s(ws->_pos.x(),ws->_pos.z());
		if(!splin){
			for(int i=0;i<400;i++){
				expar.push_back(new explosion_p(exsrc));
			}
		}
		else{
			if (ws->_type == PARTICLE_FIREBALL){
				for(int i=0;i<200;i++){
					expar.push_back(new explosion_p(exsrc));
				}
				for(int i=0;i<200;i++){
					expar.push_back(new splinter(exsrc));
				}
			}
			else if (ws->_type == PARTICLE_RAPID){
				for(int i=0;i<200;i++){
					expar.push_back(new splinter(exsrc));
				}
			}
		}
	}

	void rapid(playerstate& player){
		if (player._mp<5){
			return;
		}
		if(rfpar.size()<100&&rfire==0){
			coord2d_t dummy;
			dummy = player.calcHotSpot(dummy,.6);
			double vx = -sin(player._vel.x())*.6;
			double vz = -cos(player._vel.x())*.6;
			rfpar.push_back(new rapidfire(dummy.x(),dummy.y(),vx,vz,player._id));
			if (player._mp>=5 && player._id == 0) {
				player._mp -= 5;
				
			}
		}

	}

	void spread(playerstate& player){
		if (player._mp<5){
			return;
		}
		if(rfpar.size()<100&&rfire==0){
			coord2d_t dummy;
			dummy = player.calcHotSpot(dummy,.6);
			for(double d=-.5;d<.6;d+=.1){
				double vx = -sin(player._vel.x()+d)*.6;
				double vz = -cos(player._vel.x()+d)*.6;
				rfpar.push_back(new rapidfire(dummy.x(),dummy.y(),vx,vz,player._id));
			}
			if (player._mp>=5 && player._id == 0) {
				player._mp -= 5;
				
			}
		}
	}



};


class objectstate: public colliobj{
public:
	objectstate();
	objectstate(const objectstate& other);

	//uint16_t _id;
	//uint8_t _hp;
	uint8_t _type;
	//coord2d_t _pos;
	//bbody body; //used for collision detection

	virtual void draw() = 0;
};