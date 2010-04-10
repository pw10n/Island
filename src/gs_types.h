// gamestate types
//
#pragma once

#include <vector>

#include "types.h"

using namespace std;

class gamestate_t;
class playerstate_t;
class objectstate_t;
class wepfirestate_t;

class gamestate_t{
   public:

      int _tick;
      vector<playerstate_t> _players;
      vector<objectstate_t> _objects;
      vector<wepfirestate_t> _wepfire;
      char _map[30];
};

class playerstate_t{
   public:

      int _hp;
      int _mp;
      int _ability[5];
      int _weapon;
      coord2i_t _pos;
      coord2i_t _vel;
      int _state;
      int _score;
};

class objectstate_t{
   public:

      int _hp;
      int _type;
      coord2i_t _pos;
};

class wepfirestate_t{
   public:

      coord2i_t _pos;
      coord2i_t _vel;
      int _ttl;
      int _type;
};



   

   


