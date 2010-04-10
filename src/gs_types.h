// gamestate types
//
#pragma once

#include <vector>
#include <stdint.h>

#include "types.h"

using namespace std;

class gamestate_t;
class playerstate_t;
class objectstate_t;
class wepfirestate_t;

class gamestate_t: public netobj{
   public:
      uint32_t _tick;
      uint8_t _state;
      char _map[30];

      vector<playerstate_t> _players;
      vector<objectstate_t> _objects;
      vector<wepfirestate_t> _wepfire;

      gamestate_t(uint32_t time=0, char* map);

      void tick(uint32_t time);

      void addPlayer(playerstate_t player);
      void removePlayer(uint16_t id);

      void addObject(objectstate_t object);
      void removeObject(uint16_t id);

      void addWepfire(wepfirestate_t wepfire);

      void updateState(uint8_t state);


      //network functions:
      int serialize(char* buf, int sz);

   proteted:
};


class playerstate_t{
   public:

      uint16_t _id;
      uint8_t _hp;
      uint8_t _mp;
      uint8_t _ability[5];
      uint8_t _weapon;
      coord2i_t _pos;
      coord2i_t _vel;
      uint8_t _state;
      uint16_t _score;
};

class objectstate_t{
   public:

      uint16_t _id;
      uint8_t _hp;
      uint8_t _type;
      coord2i_t _pos;
};

class wepfirestate_t{
   public:

      coord2i_t _pos;
      coord2i_t _vel;
      uint32_t _ttl;
      uint8_t _type;
};



   

   


