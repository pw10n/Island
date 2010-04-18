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

      void addPlayer(playerstate_t player);
      void removePlayer(uint16_t id);

      void addObject(objectstate_t object);
      void removeObject(uint16_t id);

      void addWepfire(wepfirestate_t wepfire);

      void updateState(uint8_t state);

	  vector<gsDelta_t> _deltas;

      //network functions:
      int serialize_delta(char* buf, int sz);
	  int serialize_sync(char* buf, int sz);
      int sync(char* buf, int sz);

   proteted:
};


class playerstate_t: public netobj{
   public:
      uint16_t _id;
      uint8_t _hp;
      uint8_t _mp;
      uint8_t _ability[5];
      uint8_t _weapon;
      coord2d_t _pos;
      coord2d_t _vel;
      uint8_t _state;
      uint16_t _score;

	  playerstate_t();
	  playerstate_t(const playerstate_t &player);

	  vector<gsDelta_t> _deltas;

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

	  vector<gsDelta_data> _deltas;

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

	  vector<gsDelta_t> _deltas;

	  //network functions:
      int serialize_delta(char* buf, int sz);
	  int serialize_sync(char* buf, int sz);
      int sync(char* buf, int sz);
};



   

   


