#include "netutil.h"


class gameobj: public objectstate_t {

public:

    gameobj(): objectstate_t() {};
	  gameobj(uint16_t id, uint8_t hp, uint8_t type, coord2d_t pos): objectstate_t(id, hp, type, pos) {};
    gameobj(uint16_t id, uint8_t hp, uint8_t type, double x, double y): objectstate_t(id, hp, type, x, y){};
    gameobj(const objectstate_t& other): objectstate_t(other){};

};
