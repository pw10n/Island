// gamestate types
//
#pragma once

#include <vector>
#include "stdint.h"

#include "types.h"
#include "netutil.h"

using namespace std;

class gamestate_t;
class playerstate_t;
class objectstate_t;
class wepfirestate_t;

#define GSSTATE_INACTIVE 0
#define GSSTATE_ACTIVE 1
#define GSSTATE_INVALID 99

#define GS_MAX_MAP_NAME_LEN 30


class gamestate_t: public netobj{
   public:
      uint32_t _tick;
      uint8_t _state;
      char _map[GS_MAX_MAP_NAME_LEN];

      vector<playerstate_t> _players;
      vector<objectstate_t> _objects;
      vector<wepfirestate_t> _wepfire;

      gamestate_t(uint32_t time, char* map);

      void tick(uint32_t time);

      void addPlayer(const playerstate_t &player);
      void removePlayer(uint16_t id);

      void addObject(const objectstate_t &object);
      void removeObject(uint16_t id);

      void addWepfire(const wepfirestate_t &wepfire);

      void updateState(uint8_t state);

	  vector<gDelta_data> _deltas;

      //network functions:
      int serialize_delta(char* buf, int sz);
	  int serialize_sync(char* buf, int sz);
      int sync(char* buf, int sz);
};

#define PSSTATE_INIT 0
#define PSSTATE_ALIVE 1
#define PSSTATE_DEAD 2
#define PSSTATE_WAITSYNC 3

#define PLAYERSTATE_MAXABILITY 5

class playerstate_t: public netobj{
   public:
	  uint32_t _tick; //4
      uint16_t _id; //2
      uint8_t _hp; //1
      uint8_t _mp; //1
      uint8_t _ability[PLAYERSTATE_MAXABILITY]; //1*5
      uint8_t _weapon; //1
      coord2d_t _pos; //8
      coord2d_t _vel; //8
      uint8_t _state; //1
      uint16_t _score; //2

	  vector<gDelta_data> _deltas;

	  playerstate_t(uint32_t time);
	  playerstate_t(const playerstate_t &player);

	  void tick(uint32_t time);

	  void change_velocity(coord2d_t nV);
	  void change_velocity(double nVx, double nVy);

	  //network functions:
      int serialize_delta(char* buf, int sz);
	  int serialize_sync(char* buf, int sz);
      int sync(char* buf, int sz);
};

class objectstate_t: public netobj{
   public:

      uint16_t _id;
      uint8_t _hp;
      uint8_t _type;
      coord2d_t _pos;

	  vector<gDelta_data> _deltas;

	  //network functions:
      int serialize_delta(char* buf, int sz);
	  int serialize_sync(char* buf, int sz);
      int sync(char* buf, int sz);
};

class wepfirestate_t{
   public:

      coord2d_t _pos;
      coord2d_t _vel;
      uint32_t _ttl;
      uint8_t _type;

	  vector<gDelta_data> _deltas;

	  //network functions:
      int serialize_delta(char* buf, int sz);
	  int serialize_sync(char* buf, int sz);
      int sync(char* buf, int sz);
};



   

   


