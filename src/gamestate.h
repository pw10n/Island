
#pragma once
#include <list> //lists are faster at removing from the middle
#include <vector>
#include "types.h"
#include "stdint.h"
#include "collision.h"
//#include "Bin.h" don't need anymore
#include "playerstate.h"
#include "particle.h"
#include "attack.h"

using namespace std;

class gamestate;
class objectstate;

#define GSSTATE_INACTIVE 0
#define GSSTATE_ACTIVE 1
#define GSSTATE_INVALID 99

#define OBJECTSTATE_CRATE 0
#define OBJECTSTATE_HUT 6
#define OBJECTSTATE_ROCK 7
#define OBJECTSTATE_ROCK2 8
#define HIT_CRATE 1
#define HIT_PLAYER 2
#define HIT_HUT 3
#define HIT_TREE 4
#define HIT_ROCK 5
#define HIT_ROCK2 6

#define PSTATE_AI_SEARCHING 11
#define PSTATE_AI_TARGETING_1 12
#define PSTATE_AI_TARGETING_2 13
#define PSTATE_AI_TARGETING_3 14
#define PSTATE_AI_ATACKING 15


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
	vector<attack> _attacks; //all available attacks; not just the ones the player chooses

	Bin* bins[100][100];

	uint32_t _tick;
	uint8_t _state;

	vector<fireball_s *> fbsrc;
	int fbtim;
	vector<explosion_s *> exsrc;
	bool explo;
	smite_s * smsrc;
	bool smit;
	vector<rapidfire *> rfpar;
	vector<particle *> _pars;
	int beatim;
	bool bflag;
	beam * besrc;
	int rfire;

	uint32_t _lastDrawn;

	void start(int time);
	void draw();
	void tick(uint32_t time);

	bool cull2(coord2d_t pos);
	bool cull3(vec3d_t pos);

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

	void detonate(source * ws, bool splin){
		explo = true;
		exsrc.push_back(new explosion_s(ws->_pos.x(),ws->_pos.z()));
		for(int i=0;i<100;i++) _pars.push_back(new sand(*(exsrc.end()-1)));
		if(!splin){
			for(int i=0;i<200;i++){
				_pars.push_back(new explosion_p(*(exsrc.end()-1)));
			}
		}
		else{
			if (ws->_type == PARTICLE_FIREBALL){
				for(int i=0;i<100;i++){
					_pars.push_back(new explosion_p(*(exsrc.end()-1)));
				}
				for(int i=0;i<200;i++){
					_pars.push_back(new splinter(*(exsrc.end()-1)));
				}
			}
			else if (ws->_type == PARTICLE_RAPID){
				for(int i=0;i<200;i++){
					_pars.push_back(new splinter(*(exsrc.end()-1)));
				}
			}
		}
	}
	void erapid(playerstate& player){
		if(rfpar.size()<100){
			coord2d_t dummy;
			dummy = player.calcHotSpot(dummy,.6);
			double vx = -sin(player.vel().x())*.6;
			double vz = -cos(player.vel().x())*.6;
			rfpar.push_back(new rapidfire(dummy.x(),dummy.y(),vx,vz,player._id));
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