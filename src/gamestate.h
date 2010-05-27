
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
	
	
	playerstate* player;
	vector<playerstate*> _enemies;
	vector<objectstate*> _objects;

	Bin* bins[100][100];

	uint32_t _tick;
	uint8_t _state;

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