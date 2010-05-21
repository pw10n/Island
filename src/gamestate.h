
#pragma once

#include <vector>
#include "types.h"
#include "stdint.h"
#include "collision.h"

using namespace std;

class gamestate;
class playerstate;
class objectstate;

#define GSSTATE_INACTIVE 0
#define GSSTATE_ACTIVE 1
#define GSSTATE_INVALID 99

class gamestate{
public:
	gamestate(int time);
	~gamestate();

	vector<playerstate*> _enemies;
	vector<objectstate*> _objects;

	uint32_t _tick;
	uint8_t _state;


	void draw();
	void tick(uint32_t time);

	void addEnemy(playerstate* e);
	void addObject(objectstate* o);
};

#define PSSTATE_INIT 0
#define PSSTATE_ALIVE 1
#define PSSTATE_DEAD 2
#define PSSTATE_WAITSYNC 3

#define PLAYERSTATE_MAXABILITY 5

class playerstate{
public:
	playerstate(int time);
	playerstate(const playerstate& other);

	uint32_t _tick; //4
	uint16_t _id; //2
	uint8_t _hp; //1
	uint8_t _mp; //1
	uint8_t _ability[PLAYERSTATE_MAXABILITY]; //1*5
	uint8_t _weapon; //1
	coord2d_t _pos; //8
	coord2d_t _vel; //8  Radians
	uint8_t _state; //1
	uint16_t _score; //2
	bbody body, front; //used for collision

	coord2d_t calcHotSpot(coord2d_t hs, double dist);

	virtual void draw() = 0;
	virtual void tick(int time) = 0;
};

class objectstate{
public:
	objectstate();
	objectstate(const objectstate& other);

	uint16_t _id;
	uint8_t _hp;
	uint8_t _type;
	coord2d_t _pos;
	bbody body; //used for collision detection

};