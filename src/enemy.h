#include "gamestate.h"
#include "collision.h"


class meleeAI: public playerstate{
public:
	double ecc; //eccentricity

	meleeAI(void): playerstate(0) {ecc = (double)(rand()%5)/4.0-.5;};
	~meleeAI(void) {};

	void tick(uint32_t time);
	void draw();
};

class rangedAI: public playerstate{
public:
	double ecc;
	double cooldown;

	rangedAI(void): playerstate(0) {ecc = (double)(rand()%5)/4.0-.5;};
	~rangedAI(void) {};

	void tick(uint32_t time);
	void draw();
	bool checkLOA();
};