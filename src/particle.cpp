/*
 *	The class particle itself is probably never going to see use;
 *	instead each effect has a subclass of particle.
 */
#include "particle.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <cmath>

using namespace std;

//#define PARTLIST 1
#define DSIN(x) sin(x*.01745329)
#define DCOS(x) cos(x*.01745329)
#define MIN(x,y) ((x>y)?y:x)
#define MAX(x,y) ((x>y)?x:y)

particle::particle(void)
{
}

particle::~particle(void)
{
}

fireball_s::fireball_s(double ix,double iz,double ivx,double ivz,uint16_t id)
{
	pid = id;
	_pos = vec3d_t(ix,.1,iz); _vel = vec3d_t(ivx,0,ivz);
	age = 0; active = true;
	r = 1.0f; g = .5f; b = 0.0f; a = 1.0f;
	body = DONTCOLLIDE; //set to prevent collision when "charging"
	_type = PARTICLE_FIREBALL; //there's no point in having a type argument
		//it already knows _type is PARTICLE_FIREBALL because we called fireball_s
	_damage = 10;
}

void fireball_s::move(void)
{
	if(!active) return;
	if(age>15){
		_pos.x() += _vel.x();
		_pos.z() += _vel.z();
	}
	if(age>17){ //waits to keep it from colliding with the user
		body = bbody(_pos,.15,BB_CIRC);
	}
	age++;
}

void fireball_s::draw(void)
{
	if(!active) return;
	glDisable(GL_LIGHTING);
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),.1f,_pos.z());
	//glCallList(PARTLIST);
	double sca = (double)MIN(age,15)/100.;
	glutSolidSphere(sca,10,10);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

fireball_p::fireball_p(fireball_s * sour)
{
	src = sour;
	float t = (float)(rand()%360);
	float p = (float)(rand()%180) - 90.0f;
	//float r = (float)(rand()%9+1)/50.0f;
	int lim = MIN(src->age+1,14);
	float r = (float)(rand()%lim+1)/300.0f;
	_pos.x() = src->_pos.x()+r*DSIN(t)*DCOS(p)*2.0f;
	_pos.y() = src->_pos.y()+r*DSIN(p)*2.0f;
	_pos.z() = src->_pos.z()+r*DCOS(t)*DCOS(p)*2.0f;
	if(src->age<15){
		_vel.x() = r*DSIN(t)*DCOS(p);
		_vel.y() = r*DSIN(p);
		_vel.z() = r*DCOS(t)*DCOS(p);
		life = (float)(rand()%20)/10;
	}else{
		_vel.x() = src->_vel.x()+r*DSIN(t)*DCOS(p);
		_vel.y() = r*DSIN(p);
		_vel.z() = src->_vel.z()+r*DCOS(t)*DCOS(p);
		life = 2.0f;
	}
	fade = (float)(rand()%7+3)/50.0f;
	r = g = a = 1.0f; b = 0.0f;
	active = true;
}

void fireball_p::refresh(void)
{
	float t = (float)(rand()%360);
	float p = (float)(rand()%180) - 90.0f;
	int lim = MIN(src->age+1,14);
	float r = (float)(rand()%lim+1)/300.0f;
	_pos.x() = src->_pos.x()+r*DSIN(t)*DCOS(p)*2.0f;
	_pos.y() = src->_pos.y()+r*DSIN(p)*2.0f;
	_pos.z() = src->_pos.z()+r*DCOS(t)*DCOS(p)*2.0f;
	if(src->age<15){
		_vel.x() = r*DSIN(t)*DCOS(p);
		_vel.y() = r*DSIN(p);
		_vel.z() = r*DCOS(t)*DCOS(p);
		life = (float)(rand()%20)/10;
	}else{
		_vel.x() = src->_vel.x()+r*DSIN(t)*DCOS(p);
		_vel.y() = r*DSIN(p);
		_vel.z() = src->_vel.z()+r*DCOS(t)*DCOS(p);
		life = 2.0f;
	}
	fade = (float)(rand()%7+3)/50.0f;
	r = g = a = 1.0f; b = 0.0f;
	active = true;
}

void fireball_p::move(void)
{
	if(src==NULL){
		active = false;
		return;
	}
	life -= fade;
	if(life<0.0){
		refresh();
		return;
	}
	_pos = _pos + _vel; _vel = _vel * .9;
	if(life>1.5f){
		g = (life-1.5f)*2.0f;
	}else{
		a = life/1.5f; g = 0;
	}

}

void fireball_p::draw(void)
{
	glColor4f(1.0f,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),_pos.y(),_pos.z());
	float sca = life*2.0f;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

explosion_s::explosion_s(double ix, double iz)
{
	pid = 65535;//explosions belong to NO ONE! Share the pain!
	_pos = vec3d_t(ix,.1,iz); _vel = vec3d_t();
	life = 4.0f;
	fade = .5f;
	r = g = a = 1.0f; b = 0.0f;
	active = true;
	body = bbody(_pos,(double)life,BB_CIRC);
	_type = PARTICLE_EXPLOSION;
	_damage = 1;
	_damfrac = 3;
	_tock = 0;
	subPar = 0;
}

void explosion_s::move(void)
{
	if(!active) return;
	life -= fade;
	body = bbody(_pos,(double)life,BB_CIRC);
	_tock++;
	_damage = (_tock%_damfrac)?0:1;
}

void explosion_s::draw(void)
{
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),.1,_pos.z());
	glutWireSphere(life,10,10);
	glPopMatrix();
}

explosion_p::explosion_p(explosion_s * sour)
{
	src = sour;
	float t = (float)(rand()%360);
	float p = (float)(rand()%90);
	_pos = vec3d_t(src->_pos);
	_vel.x() = .4f*DSIN(t)*DCOS(p);
	_vel.y() = .4f*DSIN(p);
	_vel.z() = .4f*DCOS(t)*DCOS(p);
	life = (float)(rand()%4+1);
	fade = 2.0f/life;
	r = g = a = 1.0f; b = 0.0f;
	active = true;
	src->subPar += 1;
}

void explosion_p::move(void)
{
	if(src==NULL||!src->active){
		active = false;
		return;
	}
	life -= fade;
	if(life<0.0){
		src->subPar -= 1;
		active = false;
		return;
	}
	_pos = _pos + _vel;
	if(life>3.0f){
		g = 1.0f;
	}else if(life>2.0f){
		g = life-2.0f;
	}else{
		a = life; g = 0;
	}

}

void explosion_p::draw(void)
{
	glColor4f(1.0f,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),_pos.y(),_pos.z());
	float sca = 4.0f;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

rapidfire::rapidfire(double ix, double iz, double ivx, double ivz, uint16_t id)
{
	pid = id;
	_pos = vec3d_t(ix,.1,iz); _vel = vec3d_t(ivx,0,ivz);
	life = 0.0; r = a = 1.0f; g = b = 0.0f;
	active = true; boom = false;
	body = bbody(_pos,.01,BB_CIRC);
	_type = PARTICLE_RAPID;
	_damage = 1;
}

void rapidfire::move(void)
{
	life += .1;
	if(!boom){
		_pos = _pos + _vel;
		body.VCENX = _pos.x();
		body.VCENZ = _pos.z();
		if(life>3.0){
			life = 0.0;
			boom = true;
			g = 1.0f;
			body = DONTCOLLIDE;
		}
	}else{
		g = MAX(0.0f,1.0f-life*2.0f);
		a = MIN(1.0f,2.0f-life*2.0f);
		if(life>1.0){
			active = false;
		}
	}

}

void rapidfire::draw(void)
{
	glColor4f(1.0f,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),_pos.y(),_pos.z());
	if(!boom){
		float vtd = atan2(_vel.x(),_vel.z())*180.0f/3.1415f;
		glRotatef(vtd,0,1,0);
		glScalef(2.0f,2.0f,4.0f);
		glCallList(PARTLIST);
	}
	else{
		glDisable(GL_LIGHTING);
		glutSolidSphere(life,10,10);
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();
}

splinter::splinter(explosion_s * sour)
{
	src = sour;
	float t = (float)(rand()%360);
	float p = (float)(rand()%90);
	float v = (float)(rand()%8+1)/10.0f;
	_pos = vec3d_t(src->_pos);
	_vel.x() = v*DSIN(t)*DCOS(p);
	_vel.y() = v*DSIN(p);
	_vel.z() = v*DCOS(t)*DCOS(p);
	life = (float)(rand()%7+1);
	fade = .2f;
	roh = (float)(rand()%360);
	rov = (float)(rand()%360);
	sph = (float)(rand()%30-15);
	spv = (float)(rand()%15);
	r = .5f; g = .25f; b = 0; a = 1.0f;
	active = true;
	src->subPar += 1;
	unatt = false;
}

splinter::splinter(vec3d_t inpos)
{
	src = NULL; //we'll be ignoring src
	float t = (float)(rand()%360);
	float p = (float)(rand()%90);
	float v = (float)(rand()%8+1)/10.0f;
	_pos = inpos;
	_vel.x() = v*DSIN(t)*DCOS(p);
	_vel.y() = v*DSIN(p);
	_vel.z() = v*DCOS(t)*DCOS(p);
	life = (float)(rand()%7+1);
	fade = .2f;
	roh = (float)(rand()%360);
	rov = (float)(rand()%360);
	sph = (float)(rand()%30-15);
	spv = (float)(rand()%15);
	r = .5f; g = .25f; b = 0; a = 1.0f;
	active = true;
	unatt = true;
}

void splinter::move(void)
{
	if(!unatt&&(src==NULL||!src->active)){
		active = false;
		return;
	}
	life -= fade;
	if(life<0.0){
		if(!unatt) src->subPar -= 1;
		active = false;
		return;
	}
	if(_pos.y()>0){
		_pos = _pos + _vel;
		_vel.y() -= .1;
		roh += sph;
		rov += spv;
	}
	else a *= .9;
}

void splinter::draw(void)
{
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),_pos.y(),_pos.z());
	glRotatef(roh,0,1,0);
	glRotatef(rov,1,0,0);
	float sca = 4.0f;
	glScalef(1,1,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

beam::beam(playerstate *pla)
{
	play = pla;
	pid = play->_id;
	_pos = vec3d_t(play->front.VCENX,.25,play->front.VCENZ);
	ang = play->vel().x();
	_vel = vec3d_t(-sin(ang)*5,0,-cos(ang)*5);
	life = 0.0; g = a = 1.0f; b = 0.2f; r = 0.0f;
	active = true;
	body = bbody(_pos,_pos + _vel,BB_LINE);
	_type = PARTICLE_BEAM; //LASER!!
	_damage = 1;
}

void beam::move(void)
{
	_pos = vec3d_t(play->front.VCENX,.25,play->front.VCENZ);
	ang = play->vel().x();
	_vel = vec3d_t(-sin(ang)*5,0,-cos(ang)*5);
	body = bbody(_pos,_pos + _vel,BB_LINE);
	b += (float)(rand()%3-1)*.1f;
	if(b>1.0f) b=1.0f;
	else if(b<0.0f) b=0.0f;
}

void beam::draw(void)
{
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),_pos.y(),_pos.z());
	glRotatef(180.0f+ang*180.0f/3.1415f,0,1,0);
	glTranslatef(0,0,.1f); glCallList(PARTLIST);
	glTranslatef(0,0,.1f); glCallList(PARTLIST);
	glTranslatef(0,0,.1f); glCallList(PARTLIST);
	glTranslatef(0,0,.1f); glCallList(PARTLIST);
	glTranslatef(0,0,.1f); glCallList(PARTLIST);
	glPopMatrix();
}

smite_s::smite_s(double ix, double iz, uint16_t id)
{
	pid = id;
	_pos = vec3d_t(ix,.1,iz); _vel = vec3d_t();
	life = 2.0f;
	fade = .5f;
	b = a = 1.0f; r = g = 0.5f;
	active = true;
	body = bbody(_pos,.25,BB_CIRC);
	_type = PARTICLE_SMITE;
	_damage = 50;
	subPar = 0;
}

void smite_s::move(void)
{
	if(!active) return;
	life -= fade;
}

void smite_s::draw(void)
{
	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(_pos.x(),.1,_pos.z());
	float sca = 4.0f;
	glScalef(sca,3.0f*sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

smite_p::smite_p(smite_s * sour)
{
	src = sour;
	float t = (float)(rand()%360);
	_pos = vec3d_t(src->_pos);
	_pos.x() += .25*DSIN(t);
	_pos.y() = -10.0 + (double)(rand()%40)/5.0;
	_pos.z() += .25*DCOS(t);
	_vel.y() = 1.0;
	life = 1.0f;
	fade = (float)(rand()%10+1)/20.0f;
	b = a = 1.0f;
	r = g = (float)(rand()%128)/128.0f;
	active = true;
	src->subPar += 1;
}

void smite_p::move(void)
{
	if(src==NULL||!src->active){
		active = false;
		return;
	}
	life -= fade;
	if(life<0.0){
		src->subPar -= 1;
		active = false;
		return;
	}
	_pos = _pos + _vel;
	a = life/2.0f;

}

void smite_p::draw(void)
{
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),_pos.y(),_pos.z());
	float sca = 4.0f;
	glScalef(2,sca,2);
	glCallList(PARTLIST);
	glPopMatrix();
}

lineOfAtt::lineOfAtt(coord2d_t src, coord2d_t tar, uint16_t id)
{
	pid = id;
	body = bbody(src,tar,BB_LLINE);
	_type = PARTICLE_LOA;
	_damage = 0;
	active = false; //active will serve as a marker, because LarPaCollision() returns void
}

blood::blood(vec3d_t inpos)
{
	float t = (float)(rand()%360);
	float p = (float)(rand()%90);
	float v = (float)(rand()%8+1)/20.0f;
	_pos = inpos;
	_vel.x() = v*DSIN(t)*DCOS(p);
	_vel.y() = v*DSIN(p);
	_vel.z() = v*DCOS(t)*DCOS(p);
	life = (float)(rand()%8+1);
	fade = .2f;
	r = .95f; g = b = 0; a = 1.0f;
	active = true;
}

void blood::move(void)
{
	life -= fade;
	if(life<0.0){
		active = false;
		return;
	}
	if(_pos.y()>0){
		_pos = _pos + _vel;
		_vel.y() -= .2;
	}
	else a *= .95;
}

void blood::draw(void)
{
	float pa = MIN((2.0f*a),1.0f);
	glColor4f(r,g,b,pa);
	glPushMatrix();
	glTranslatef(_pos.x(),_pos.y(),_pos.z());
	float sca = 4.0f*(1.2f-a);
	glScalef(sca,sca,sca);
	//glCallList(PARTLIST);
	glDisable(GL_LIGHTING);
	glutSolidSphere(.05,5,5);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}