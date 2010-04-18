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

class gsDelta_t{

};



class netobj{

   public:
      virtual int serialize_delta(char* buf, int sz) = 0;
	  virtual int serialize_sync(char* buf, int sz) = 0;
      virtual int sync(char* buf, int sz) =0;
};

class client_t{
	uint16_t _clientid;
	string _name;


};

class gameServer{
	vector<client_t> _clients;

};
