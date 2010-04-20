#pragma once
#define PARTLIST 1

class particle
{
public:
	float x,y,z; //Cartesian position
	//particle *src; //Origin point for the effect (will always be a source)
	float r,g,b; //Color
	float life,fade;
	bool active;

	particle(void);
	~particle(void);
	virtual void move(float) = 0; //this makes it virtual, and particle abstract, nu?
	virtual void draw(void) = 0;
};
/*
enum Type {
	Fireball, BoostA, BoostB, Spray, Explosion
};*/

//just a mobile or immobile 3d point
class source : public particle
{
public:
	float vx,vy,vz; //Cartesian velocity
	//enum Type type; //

	/*maybe give this a vector containing its particles?*/

	source(float,float,float,float,float,float);
	void move(float);
	void draw(void);
};

class fireball : public particle
{
public:
	float vx,vy,vz; //Cartesian velocity
	source *src; //Origin point for the effect

	fireball(source *,bool);
	void move(float);
	void draw(void);
};

class boosta : public particle
{
public:
	float t,p,ra; //Orbit (spherical)
	float ang;
	source *src; //Origin point for the effect

	boosta(source *);
	void move(float);
	void draw(void);
};

class boostb : public particle
{
public:
	float t,p; //Orbit (spherical)
	bool dir; //cw or ccw
	source *src; //Origin point for the effect

	boostb(source *);
	void move(float);
	void draw(void);
};

class spray : public particle
{
public:
	float t,p,ang;
	bool dir; //cw or ccw
	source *src; //Origin point for the effect

	spray(source *,float,bool);
	void move(float);
	void draw(void);
};

class explosion : public particle
{
public:
	float vx,vy,vz;
	source *src; //Origin point for the effect

	explosion(source *);
	void move(float);
	void draw(void);
};