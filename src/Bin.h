#pragma once
#include "collision.h"
#include "particle.h"
#include "netutil.h" //switch me out when the time comes
//#include "gamestate.h" //switch me in when the time comes
#include <list> //lists are faster at removing from the middle

using namespace std;

//options for checkCollision and updatBinLists
#define COLLI 0 //regular collision detection
#define UPDAT 1 //update Bin objs list
#define REMOV 2 //remove object from Bin objs list

class Bin;

extern Bin *bins[100][100];
extern void damage(uint8_t*,int);

class Bin
{
public:
	bbody box;
	list<netobj *> objs;

	Bin(void) {}
	Bin(bbody bod): box(bod) {}
	~Bin(void) {} //deletion of objs' contents will be handled by the global vectors
	int checkPaCollision(source *src,bool hitAll);
	bool checkObCollision(netobj *obj,int op);
	bool checkPlCollision(playerstate_t *pla);
};

double b2p(int i);
void LarPaCollision(source *src,int minx,int maxx,int minz,int maxz);
bool LarObCollision(netobj *obj,int minx,int maxx,int minz,int maxz);
int SmaPaCollision(source *src);
bool SmaObCollision(netobj *obj);
bool SmaPlCollision(playerstate_t *pla);
void updatBinLists(netobj *obj,int op);

//cleans up strange problems
void janitor(int x, int z);
//used for list::remove_if to get rid of netobj*s that aren't players or objects
//don't know why they show up, but they do. This gets rid of them
bool killneto(const netobj* obj);