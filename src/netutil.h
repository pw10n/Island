#pragma once

#include <vector>

#include "types.h"

using namespace std;

class gsDelta_t{
	uint32_t _tick;

public:

	gsDelta_t():_tick(0){};
	gsDelta_t(const gsDelta_t &o):_tick(o._tick){};

	gsDelta_t operator=(const gsDelta_t &);

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
	uint16_t _serverid;
	vector<client_t> _clients;

};

//packets
#define MAX_PKTSZ 60

//pkt types
#define PKT_ACK 1

#define PKT_SYNC_GAMESTATE 11
#define PKT_SYNC_PLAYERSTATE 12
#define PKT_SYNC_OBJECTSTATE 13
#define PKT_SYNC_WEPFIRESTATE 14
#define PKT_DELTA_GAMESTATE 21
#define PKT_DELTA_PLAYERSTATE 22
#define PKT_DELTA_OBJECTSTATE 23
#define PKT_DELTA_WEPFIRESTATE 24
#define PKT_REQSYNC_GAMESTATE 31
#define PKT_REQSYNC_PLAYERSTATE 32
#define PKT_REQSYNC_OBJECTSTATE 33
#define PKT_REQSYNC_WEPFIRESTATE 34

#define PKT_PLAYER_MOVE 50
#define PKT_PLAYER_ATTACK 51



#define SEQ_INVALID_PKT 0
struct pkt_header{
	uint8_t start; //1 #
	uint8_t type; //1
	uint16_t clientid; //2
	uint16_t serverid; //2
	uint16_t checksum; //2
	uint32_t seq; //4
};

#define NET_ACK 1
#define NET_NACK 0
struct gAck_data{
	uint8_t ack_value;
};

struct gHello_data{
	uint32_t challengeVersion; //ensures that client and server have same version
};

struct gDelta_data{
   uint32_t tick; //4
   uint8_t field; //1
   varies oValue; //4
   varies nValue; //4
};

struct gsSync_data{
      uint32_t _tick;
      uint8_t _state;
      char _map[GS_MAX_MAP_NAME_LEN];
	  // send syncs for all 
	  // player,object,wepfire state
	  // variables independently
};

struct psSync_data{ //33
	uint32_t _tick; //4
	uint16_t _id; //2
	uint8_t _hp; //1
	uint8_t _mp; //1
	uint8_t _ability[PLAYERSTATE_MAXABILITY]; //1*5
	uint8_t _weapon; //1
	double _pos_x; //4
	double _pos_y; //4
	double _vel_x; //4
	double _vel_y; //4
	uint8_t _state; //1
	uint16_t _score; //2
};

struct osSync_data{
	uint16_t _id;
	uint8_t _hp;
	uint8_t _type;
	double _pos_x;
	double _pos_y;
};


struct wsSync_data{
	double _pos_x;
	double _pos_y;
	double _vel_x;
	double _vel_y;
	uint32_t _ttl;
	uint8_t _type;
};

struct playerChangeMove_data{
	uint16_t _id; //2
	double _vel_x;
	double _vel_y;
};

struct playerAttack_data{
	uint16_t _id; //2
	uint8_t _weapon;
	double _pos_x; //4
	double _pos_y; //4
	double _vel_x; //4
	double _vel_y; //4
	uint16_t _ttl;
};
