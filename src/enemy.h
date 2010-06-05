#pragma once

#include "gamestate.h"
#include "collision.h"
#include "playerstate.h"
#include "stdint.h"

class meleeAI: public playerstate{
public:
	double ecc; //eccentricity

	meleeAI(void): playerstate(0) {ecc = (double)(rand()%5)/4.0-.5;};
	~meleeAI(void) {};

	void tick(uint32_t time);
	void draw();
};

class rangedAI: public playerstate{
public:
	double ecc;
	double cooldown;

	rangedAI(void): playerstate(0) {ecc = (double)(rand()%5)/4.0-.5;};
	~rangedAI(void) {};

	void tick(uint32_t time);
	void draw();
	bool checkLOA();
};

void spwanEnemyHelperRandPos(playerstate* obj,
					  uint8_t hp,
					  uint8_t mp,
					  uint32_t tick);

void spawnEnemyHelper(playerstate* obj, 
					  uint8_t hp,
					  uint8_t mp,
					  uint32_t tick,
					  coord2d_t pos, 
					  coord2d_t vel);