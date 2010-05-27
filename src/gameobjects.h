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

class palmTree : public objectstate{
public:
	palmTree(unsigned int texture, struct obj_model_t *mdl):objectstate(),_tid(texture), mdl(mdl) {};
	unsigned int _tid;

	struct obj_model_t *mdl;
	void draw();
	int hitWhat(void) const {return HIT_TREE;};
};

class rock : public objectstate{
public:
	rock(unsigned int texture, int angle, struct obj_model_t *mdl):objectstate(),_tid(texture), angle(angle), mdl(mdl) {};
	unsigned int _tid;
	int angle;

	struct obj_model_t *mdl;
	void draw();
	int hitWhat(void) const {return HIT_ROCK;};
};

class rock2 : public objectstate{
public:
	rock2(unsigned int texture, int angle, struct obj_model_t *mdl):objectstate(),_tid(texture), angle(angle), mdl(mdl) {};
	unsigned int _tid;
	int angle;

	struct obj_model_t *mdl;
	void draw();
	int hitWhat(void) const {return HIT_ROCK;};
};

