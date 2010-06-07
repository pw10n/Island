#pragma once
#include "collision.h"
#include "playerstate.h"
#define PARTLIST 1
#define PARTICLE_TEXTURE 1
#define PARTICLE_FIREBALL 0
#define PARTICLE_RAPID 1
#define PARTICLE_BEAM 2
#define PARTICLE_EXPLOSION 3
#define PARTICLE_SMITE 4
#define PARTICLE_LOA 5

#define NUMFBPAR 100

class fireball_p;


class particle
{
public:
	vec3d_t _pos; //Cartesian position
	vec3d_t _vel;
	float r,g,b,a; //Color
	float life,fade;
	bool active;

	particle(void);
	~particle(void);
	virtual void move(void) = 0; //this makes it virtual, and particle abstract, nu?
	virtual void draw(void) = 0;
	coord2d_t * v2c(coord2d_t *dum) {dum->x() = _pos.x();dum->y() = _pos.z(); return dum;};
};
//sources are particles that have collision detection
//and are passed over the network in some form.
//non-sources just look pretty
class source : public particle
{
public:
	bbody body;//use this for testing collision
	uint16_t pid; //id of player who fired
	unsigned int _type;
	unsigned int _damage; //how much damage this attack does
};

//fireball source
class fireball_s : public source
{
public:
	int age; //used for time-based changes
	fireball_p *pars[NUMFBPAR];

	fireball_s(double,double,double,double,uint16_t);
	~fireball_s(void);
	void move(void);
	void draw(void);
};
//fireball particle
class fireball_p : public particle
{
public:
	fireball_s *src; //Origin point for the effect

	fireball_p(fireball_s *);
	void refresh(void);
	void move(void);
	void draw(void);
};

class explosion_s : public source
{
public:
	int _damfrac; //"divides" the damage so explosions aren't insanely powerful
	int _tock;
	int subPar; //number of particles attached to this

	explosion_s(double,double);
	void move(void);
	void draw(void);
};

class explosion_p : public particle
{
public:
	explosion_s *src; //Origin point for the effect

	explosion_p(explosion_s *);
	void move(void);
	void draw(void);
};

class rapidfire : public source
{
public:
	bool boom; //has it hit yet?
	
	rapidfire(double,double,double,double,uint16_t);
	void move(void);
	void draw(void);
};

class splinter : public particle
{
public:
	float roh,rov,sph,spv; //ROtation/SPin; horizontal/vertical
	explosion_s *src;
	bool unatt; //if true, this is not attached to an explosion_s

	splinter(explosion_s *);
	splinter(vec3d_t);
	void move(void);
	void draw(void);
};

class beam : public source
{
public:
	playerstate *play; //player who fired this
	double ang;

	beam(playerstate *);
	void move(void);
	void draw(void);
};

class smite_s : public source
{
public:
	int subPar; //number of particles attached to this

	smite_s(double,double, uint16_t);
	void move(void);
	void draw(void);
};

class smite_p : public particle
{
public:
	smite_s *src; //Origin point for the effect

	smite_p(smite_s *);
	void move(void);
	void draw(void);
};

//not an actual attack. Used by ranged AI to determine if anything is in the way of the shot
class lineOfAtt : public source
{
public:
	lineOfAtt(coord2d_t,coord2d_t,uint16_t);
	void move(void) {printf("Why did you call me?\n");};
	void draw(void) {printf("I'm not supposed to be called.\n");};
};

class blood : public particle
{
public:
	blood(vec3d_t);
	void move(void);
	void draw(void);
};

class sand : public particle
{
public:
	explosion_s *src; //Origin point for the effect

	sand(explosion_s *);
	void move(void);
	void draw(void);
};
