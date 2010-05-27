#pragma once

#include "gamestate.h"

using namespace std;

class goCrate : public objectstate{
public:
	goCrate(unsigned int texture):objectstate(),_tid(texture) {};
	unsigned int _tid;
	void draw();
	int hitWhat(void) const {return HIT_CRATE;};
};

class Hut : public objectstate{
public:
	Hut(unsigned int texture, struct obj_model_t *mdl):objectstate(),_tid(texture), mdl(mdl) {};
	unsigned int _tid;

	struct obj_model_t *mdl;
	void draw();
	int hitWhat(void) const {return HIT_HUT;};
};

