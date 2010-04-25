#pragma once
#define PARTLIST 1

class particle
{
public:
	float x,y,z; //Cartesian position
	//particle *src; //Origin point for the effect (will always be a source)
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
	virtual bool collide(float,float,float) = 0;
};

//fireball source
class fireball_s : public source
{
public:
	float vx,vz; //Cartesian velocity
	int age; //used for time-based changes

	/*maybe give this a vector containing its particles?*/

	fireball_s(float,float,float,float);
	void move(void);
	void draw(void);
	bool collide(float,float,float);
};
//fireball particle
class fireball_p : public particle
{
public:
	float vx,vy,vz; //Cartesian velocity
	fireball_s *src; //Origin point for the effect

	fireball_p(fireball_s *);
	void move(void);
	void draw(void);
};

class explosion_s : public source
{
public:

	explosion_s(float,float);
	void move(void);
	void draw(void);
	bool collide(float,float,float);
};

class explosion_p : public particle
{
public:
	float vx,vy,vz;
	explosion_s *src; //Origin point for the effect

	explosion_p(explosion_s *);
	void move(void);
	void draw(void);
};

class rapidfire : public source
{
public:
	float vtr,vtd; //theta: rad, deg
	bool boom; //has it hit yet?
	
	rapidfire(float,float,float,float);
	void move(void);
	void draw(void);
	bool collide(float,float,float);
};

