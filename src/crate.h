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
