#pragma once

#include "collision.h"

#define HIT_CRATE 1
#define HIT_PLAYER 2

#define PSSTATE_INIT 0
#define PSSTATE_ALIVE 1
#define PSSTATE_DEAD 2
#define PSSTATE_WAITSYNC 3

#define PLAYERSTATE_MAXABILITY 5

class colliobj{

   public:
	   bbody body;
	   uint16_t _id;
	   uint8_t _hp;
	   coord2d_t _pos;

	   colliobj(): _id(0), _hp(0) {};
	  ~colliobj() {}

	  virtual int hitWhat(void) const = 0; //returns the HIT_* value; overwritten by subclasses
	
	  bool operator==(const colliobj& obj) {return (_id==obj._id);}
};

class playerstate: public colliobj{
public:
	playerstate(int time);
	playerstate(const playerstate& other);

	uint32_t _tick; //4
	//uint16_t _id; //2
	//uint8_t _hp; //1
	uint8_t _mp; //1
	uint8_t _ability[PLAYERSTATE_MAXABILITY]; //1*5
	uint8_t _weapon; //1
	//coord2d_t _pos; //8
	coord2d_t _vel; //8  Radians
	uint8_t _state; //1
	uint16_t _score; //2
	bbody front; //used for collision

	coord2d_t calcHotSpot(coord2d_t hs, double dist);

	//virtual void draw() = 0; uncomment these when ready to use
	void tick(uint32_t time);
	int hitWhat(void) const {return HIT_PLAYER;};
};