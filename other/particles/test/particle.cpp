/*
 *	The class particle itself is probably never going to see use;
 *	instead each effect has a subclass of particle.
 */
#include "particle.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

//#define PARTLIST 1
#define DSIN(x) sin(x*.01745329)
#define DCOS(x) cos(x*.01745329)

particle::particle(void)
{
}

particle::~particle(void)
{
}

source::source(float ix,float iy,float iz,float ivx,float ivy,float ivz)
{
	x = ix; y = iy; z = iz; vx = ivx; vy = ivy; vz = ivz;
}

void source::move(float dt)
{
	x += vx*dt; y += vy*dt; z += vz*dt;
}

void source::draw(void)
{
	glColor3f(r,g,b);
	glPushMatrix();
	glTranslatef(x,y,z);
	glCallList(PARTLIST);
	glPopMatrix();
}

fireball::fireball(source * sour,bool start)
{
	src = sour;
	float t = (float)(rand()%360);
	float p = (float)(rand()%180) - 90.0f;
	float r = (float)(rand()%9+1)/50.0f;
	x = src->x+r*DSIN(t)*DCOS(p)*2.0f;
	y = src->y+r*DSIN(p)*2.0f;
	z = src->z+r*DCOS(t)*DCOS(p)*2.0f;
	vx = src->vx+r*DSIN(t)*DCOS(p);
	vy = src->vy+r*DSIN(p);
	vz = src->vz+r*DCOS(t)*DCOS(p);
	if(start) life = (float)(rand()%20)/10;
	else life = 2.0f;
	fade = (float)(rand()%7+3)/50.0f;
	r = g = 1.0f; b = 0.0f;
	active = true;
}

void fireball::move(float dt)
{
	life -= fade*dt;
	float po = pow(.9f,dt); //hopefully pow() isn't too costly
	x += vx*dt; vx *= po; y += vy*dt; vy *= po; z += vz*dt; vz *= po;
	//r = (life>1.0f)?1.0f:life;
	//g = (life>1.0f)?(life-1.0f):0;
	if(life>1.5f){
		r = 1.0f; g = (life-1.5f)*2.0f;
	}else{
		r = life/1.5f; g = 0;
	}

}

void fireball::draw(void)
{
	glColor3f(r,g,b);
	glPushMatrix();
	glTranslatef(x,y,z);
	//float sca = 5.0f-life*2.0f;
	float sca = life*2.0f;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

boosta::boosta(source * sour)
{
	src = sour;
	t = (float)(rand()%360);
	p = (float)(rand()%180) - 90.0f;
	ra = 0.0f; //start at source
	ang = (float)(rand()%360);
	x = 0;
	y = 0;
	z = 0;
	life = 4.0f;
	fade = (float)(rand()%9+1)/50.0f;
	r = 0.0f; g = 1.0f; b = (float)(rand()%50)/50.0f;
	active = true;
}

void boosta::move(float dt)
{
	life -= fade*dt;
	ang += dt*10.0f; if(ang>360.0f) ang -= 360.0f;
	if(ra<1.0f) ra+= dt/2.0f; else ra=1.0f;
	x = ra*DCOS(ang);
	z = ra*DSIN(ang);
}

void boosta::draw(void)
{
	glColor3f(r,g,b);
	glPushMatrix();
	glTranslatef(src->x,src->y,src->z);
	glRotatef(t,0,1,0);
	glRotatef(p,1,0,0);
	glTranslatef(x,y,z);
	glCallList(PARTLIST);
	glPopMatrix();
}

boostb::boostb(source * sour)
{
	src = sour;
	t = (float)(rand()%360);
	p = (float)(rand()%180) - 90.0f;
	dir = (rand()%2==0)?true:false;
	x = 0;
	y = 0;
	z = 0;
	life = 4.0f;
	fade = (float)(rand()%9+1)/50.0f;
	r = 0.0f; g = (float)(rand()%50)/50.0f; b = 1.0f;
	active = true;
}

void boostb::move(float dt)
{
	life -= fade*dt;
	t += dt*10.0f; if(t>360.0f) t -= 360.0f;
	if(z<1.0f) z+= dt/2.0f; else z=1.0f;
}

void boostb::draw(void)
{
	glColor3f(r,g,b);
	glPushMatrix();
	glTranslatef(src->x,src->y,src->z);
	glRotatef(t*((dir)?1:-1),0,1,0);
	glRotatef(p,1,0,0);
	glTranslatef(x,y,z);
	glCallList(PARTLIST);
	glPopMatrix();
}

spray::spray(source * sour, float it, bool start)
{
	src = sour;
	t = it;
	p = (float)(rand()%20);
	ang = (float)(rand()%360);
	x = 0;
	y = 0;
	z = 0;
	if(start) life = (float)(rand()%10)/10;
	else life = 1.0f;
	fade = (float)(rand()%7+3)/50.0f;
	r = (float)(rand()%100)/100.0f;
	g = (float)(rand()%50+50)/100.0f;
	b = 1.0f;
	dir = (rand()%2)?true:false;
	active = true;
}

void spray::move(float dt)
{
	life -= fade*dt;
	if(dir) ang += 20.0f*dt;
	else ang -= 20.0f*dt;
	z += dt;
}

void spray::draw(void)
{
	glColor3f(r,g,b);
	glPushMatrix();
	glTranslatef(src->x,src->y,src->z);
	glRotatef(t,0,1,0);
	glRotatef(ang,0,0,1);
	glRotatef(p,1,0,0);
	glTranslatef(0,0,z);
	float sca = 3.0f-life;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

explosion::explosion(source * sour)
{
	src = sour;
	float t = (float)(rand()%360);
	float p = (float)(rand()%180) - 90.0f;
	x = src->x;
	y = src->y;
	z = src->z;
	vx = src->vx+.4f*DSIN(t)*DCOS(p);
	vy = src->vy+.4f*DSIN(p);
	vz = src->vz+.4f*DCOS(t)*DCOS(p);
	life = (float)(rand()%4+1);
	fade = 2.0f/life;
	//life = 1.0f;
	//fade = (float)(rand()%7+3)/50.0f;
	r = g = 1.0f; b = 0.0f;
	active = true;
}

void explosion::move(float dt)
{
	life -= fade*dt;
	x += vx*dt; y += vy*dt; z += vz*dt;
	if(y<-.1f) {y=-.1f; vy *= -1.0f;}
	if(life>3.0f){
		r = g = 1.0f;
	}else if(life>2.0f){
		r = 1.0f; g = life-2.0f;
	}else{
		r = life; g = 0;
	}

}

void explosion::draw(void)
{
	glColor3f(r,g,b);
	glPushMatrix();
	glTranslatef(x,y,z);
	float sca = 4.0f;
	glScalef(sca,sca,sca);
	glCallList(PARTLIST);
	glPopMatrix();
}

