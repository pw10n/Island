#include "netutil.h"


class crate: public objectstate_t {

public:
	unsigned int texture;

    crate(unsigned int texid): objectstate_t() {
		texture = texid;
	};
	crate(uint16_t id, uint8_t hp, uint8_t type, coord2d_t pos, unsigned int texid): objectstate_t(id, hp, type, pos) {
		texture = texid;
	};
    crate(uint16_t id, uint8_t hp, uint8_t type, double x, double y, unsigned int texid): objectstate_t(id, hp, type, x, y){
		texture = texid;
	};
    crate(const objectstate_t& other, unsigned int texid): objectstate_t(other){
		texture = texid;
	};
	crate(uint16_t id, uint8_t hp, uint8_t type, coord2d_t pos): objectstate_t(id, hp, type, pos) {};

};
