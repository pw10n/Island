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
#define DONTCOLLIDE sphere(0,100,100)

particle::particle(void)
{
}

particle::~particle(void)
{
}

fireball_s::fireball_s(double ix,double iz,double ivx,double ivz)
{
	//x = ix; y = .1f; z = iz; vx = ivx; vz = ivz;
	_pos = vec3d_t(ix,.1,iz); _vel = vec3d_t(ivx,0,ivz);
	age = 0; active = true;
	r = 1.0f; g = .5f; b = 0.0f; a = 1.0f;
	body = DONTCOLLIDE; //set to prevent collision when "charging"
	_type = PARTICLE_FIREBALL; //there's no point in having a type argument
		//it already knows _type is PARTICLE_FIREBALL because we called fireball_s
}

void fireball_s::move(void)
{
	if(!active) return;
	if(age>15){
		_pos.x() += _vel.x();
		_pos.z() += _vel.z();
	}
	if(age>17){ //waits to keep it from colliding with the user
		body = sphere(.15,_pos.x(),_pos.z());
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
/*
bool fireball_s::collide(float obx, float obz, float obr)
{
	float rad = (float)MIN(age,15)/100.f;
	float dist = hypot(obx-x,obz-z);
	return (dist<(obr+rad));
}
*/
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

void fireball_p::move(void)
{
	life -= fade;
	_pos = _pos + _vel; _vel = _vel * .9;
	//r = (life>1.0f)?1.0f:life;
	//g = (life>1.0f)?(life-1.0f):0;
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
	//float sca = 5.0f-life*2.0f;
	float sca = life*2.0f;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

explosion_s::explosion_s(double ix, double iz)
{
	_pos = vec3d_t(ix,.1,iz); _vel = vec3d_t();
	life = 4.0f;
	fade = .5f;
	//life = 1.0f;
	//fade = (float)(rand()%7+3)/50.0f;
	r = g = a = 1.0f; b = 0.0f;
	active = true;
	body = sphere(life,_pos.x(),_pos.z());
}

void explosion_s::move(void)
{
	if(!active) return;
	life -= fade;
	body = sphere(life,_pos.x(),_pos.z());
}

void explosion_s::draw(void)
{
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(_pos.x(),.1,_pos.z());
	float sca = 4.0f;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}
/*
bool explosion_s::collide(float obx, float obz, float obr)
{
	float dist = hypot(obx-x,obz-z);
	return (dist<(obr+life));
}
*/
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
	//life = 1.0f;
	//fade = (float)(rand()%7+3)/50.0f;
	r = g = a = 1.0f; b = 0.0f;
	active = true;
}

void explosion_p::move(void)
{
	life -= fade;
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

rapidfire::rapidfire(double ix, double iz, double ivx, double ivz)
{
	_pos = vec3d_t(ix,.1,iz); _vel = vec3d_t(ivx,0,ivz);
	life = 0.0; r = a = 1.0f; g = b = 0.0f;
	active = true; boom = false;
	body = sphere(.2,_pos.x(),_pos.z());
	_type = PARTICLE_RAPID;
}

void rapidfire::move(void)
{
	life += .1;
	if(!boom){
		//_pos.x() += -sin(vtr)*.6;
		//_pos.z() += -cos(vtr)*.6;
		_pos = _pos + _vel;
		//body = (life>.05)?sphere(.05f,x,z):DONTCOLLIDE;
		body = sphere(.01,_pos.x(),_pos.z());
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
/*
bool rapidfire::collide(float obx, float obz, float obr)
{
	float dist = hypot(obx-x,obz-z);
	return (dist<(obr+.2f));
}
*/
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
}

void splinter::move(void)
{
	life -= fade;
	if(_pos.y()>0){
		//_pos.x() += _vel.x(); _pos.x() += _vel.y(); _pos.x() += _vel.z();
		_pos = _pos + _vel;
		_vel.y() -= .1;
		roh += sph;
		rov += spv;
	}
	//if(y<0) life = -0.1f;
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
