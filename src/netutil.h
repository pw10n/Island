#pragma once

#include "types.h"
#include "gs_types.h"

using namespace std;

struct gsDelta_data{
   uint32_t tick;
   uint8_t field;
   varies oValue;
   varies nValue;
};
struct gsAddPlayer_t{
   uint32_t tick;
   playerstate_t *ps;
}

class netobj{

   public:
      virtual int serialize(char* buf, int sz) = 0;
      virtual int sync(char* buf, int sz) =0;
};



