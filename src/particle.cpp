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

fireball_s::fireball_s(float ix,float iz,float ivx,float ivz)
{
	x = ix; y = .1f; z = iz; vx = ivx; vz = ivz;
	age = 0; active = true;
	r = 1.0f; g = .5f; b = 0.0f; a = 1.0f;
	body = DONTCOLLIDE; //set to prevent collision when "charging"
}

void fireball_s::move(void)
{
	if(!active) return;
	if(age>15){
		x += vx; z += vz;
	}
	if(age>20){ //waits until 20 to keep it from colliding with the user
		body = sphere(.15,x,z);
	}
	age++;
}

void fireball_s::draw(void)
{
	if(!active) return;
	glDisable(GL_LIGHTING);
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(x,y,z);
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
	x = src->x+r*DSIN(t)*DCOS(p)*2.0f;
	y = src->y+r*DSIN(p)*2.0f;
	z = src->z+r*DCOS(t)*DCOS(p)*2.0f;
	if(src->age<15){
		vx = r*DSIN(t)*DCOS(p);
		vy = r*DSIN(p);
		vz = r*DCOS(t)*DCOS(p);
		life = (float)(rand()%20)/10;
	}else{
		vx = src->vx+r*DSIN(t)*DCOS(p);
		vy = r*DSIN(p);
		vz = src->vz+r*DCOS(t)*DCOS(p);
		life = 2.0f;
	}
	fade = (float)(rand()%7+3)/50.0f;
	r = g = a = 1.0f; b = 0.0f;
	active = true;
}

void fireball_p::move(void)
{
	life -= fade;
	x += vx; vx *= .9f; y += vy; vy *= .9f; z += vz; vz *= .9f;
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
	glTranslatef(x,y,z);
	//float sca = 5.0f-life*2.0f;
	float sca = life*2.0f;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

explosion_s::explosion_s(float ix, float iz)
{
	x = ix;
	y = .1f;
	z = iz;
	life = 4.0f;
	fade = .5f;
	//life = 1.0f;
	//fade = (float)(rand()%7+3)/50.0f;
	r = g = a = 1.0f; b = 0.0f;
	active = true;
	body = sphere(life,x,z);
}

void explosion_s::move(void)
{
	if(!active) return;
	life -= fade;
	body = sphere(life,x,z);
}

void explosion_s::draw(void)
{
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(x,y,z);
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
	x = src->x;
	y = src->y;
	z = src->z;
	vx = .4f*DSIN(t)*DCOS(p);
	vy = .4f*DSIN(p);
	vz = .4f*DCOS(t)*DCOS(p);
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
	x += vx; y += vy; z += vz;
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
	glTranslatef(x,y,z);
	float sca = 4.0f;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

rapidfire::rapidfire(float ix, float iz, float itr, float itd)
{
	x = ix; y = .1; z = iz; vtr = itr; vtd = itd;
	life = 0.0; r = a = 1.0f; g = b = 0.0f;
	active = true; boom = false;
	body = sphere(.2f,x,z);
}

void rapidfire::move(void)
{
	life += .1;
	if(!boom){
		x += -sin(vtr)*.5;
		z += -cos(vtr)*.5;
		body = sphere(.2f,x,z);
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
	glTranslatef(x,y,z);
	//glRotatef(vtd,0,1,0);
	if(!boom){
		glScalef(4.0f,4.0f,4.0f);
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
	x = src->x;
	y = src->y;
	z = src->z;
	vx = v*DSIN(t)*DCOS(p);
	vy = v*DSIN(p);
	vz = v*DCOS(t)*DCOS(p);
	life = (float)(rand()%7+1);
	fade = .2f;
	roh = (float)(rand()%360);
	rov = (float)(rand()%360);
	sph = (float)(rand()%30-15);
	spv = (float)(rand()%15);
	r = .5f; g = .25f; b = 0; a = 1.f;
	active = true;
}

void splinter::move(void)
{
	life -= fade;
	if(y>0){
		x += vx; y += vy; z += vz;
		vy -= .1;
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
	glTranslatef(x,y,z);
	glRotatef(roh,0,1,0);
	glRotatef(rov,1,0,0);
	float sca = 4.0f;
	glScalef(1,1,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}