#pragma once
#define PARTLIST 1
#define PARTICLE_TEXTURE 1
#define PARTICLE_FIREBALL 0
#define PARTICLE_RAPID 1
#include "collision.h"


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
};
//sources are particles that have collision detection
//and are passed over the network in some form.
//non-sources just look pretty
class source : public particle
{
public:
	struct sphere body;//use this for testing collision
	//virtual bool collide(float,float,float) = 0;
	unsigned int _type;
};

//fireball source
class fireball_s : public source
{
public:
	//float vx,vz; //Cartesian velocity
	int age; //used for time-based changes

	/*maybe give this a vector containing its particles?*/

	fireball_s(double,double,double,double);
	void move(void);
	void draw(void);
	//bool collide(float,float,float);
};
//fireball particle
class fireball_p : public particle
{
public:
	//float vx,vy,vz; //Cartesian velocity
	fireball_s *src; //Origin point for the effect

	fireball_p(fireball_s *);
	void move(void);
	void draw(void);
};

class explosion_s : public source
{
public:

	explosion_s(double,double);
	void move(void);
	void draw(void);
	//bool collide(float,float,float);
};

class explosion_p : public particle
{
public:
	//float vx,vy,vz;
	explosion_s *src; //Origin point for the effect

	explosion_p(explosion_s *);
	void move(void);
	void draw(void);
};

class rapidfire : public source
{
public:
	//float vtr,vtd; //theta: rad, deg
	bool boom; //has it hit yet?
	
	rapidfire(double,double,double,double);
	void move(void);
	void draw(void);
	//bool collide(float,float,float);
};

class splinter : public particle
{
public:
	float roh,rov,sph,spv; //ROtation/SPin; horizontal/vertical
	//float vx,vy,vz;
	explosion_s *src;
	//float scalw,scall; //Scale width/length

	splinter(explosion_s *);
	void move(void);
	void draw(void);
};

