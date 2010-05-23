
#include "bvh.h"
#include "types.h"
#include <GL/glut.h>

#define PI 3.14159265
#define MIN(x,y) ((x>y)?y:x)
#define MAX(x,y) ((x>y)?x:y)
/*
void Groupable::upgroupbody(void){
	if(group){
		bbody *dummy = new bbody();
		aabbbody(dummy);
		group->body.VMINX = MIN((group->body.VMINX),(dummy->VMINX));
		group->body.VMINZ = MIN((group->body.VMINZ),(dummy->VMINZ));
		group->body.VMAXX = MAX((group->body.VMAXX),(dummy->VMAXX));
		group->body.VMAXZ = MAX((group->body.VMAXZ),(dummy->VMAXZ));
		group->upgroupbody();
	}
}
*/

Enemy::Enemy(void){
	_pos = coord2d_t((double)(rand()%100)-50.0,(double)(rand()%100)-50.0);
	_vel = coord2d_t(0.0,0.0);
	body = bbody(_pos.x(),-_pos.y(),.5,0,BB_CIRC);
	tock = 0;
	alive = true;
	//group = NULL;
}

Enemy::Enemy(bbody boun){
	double dx = boun.VMAXX-boun.VMINX-1.0;
	double dz = boun.VMAXZ-boun.VMINZ-1.0;
	_pos = coord2d_t();
	_pos.x() = boun.VMINX + .5 + (double)(rand()%100)/100.0*dx;
	_pos.y() = boun.VMINZ + .5 + (double)(rand()%100)/100.0*dz;
	_vel = coord2d_t(0.0,0.0);
	body = bbody(_pos.x(),-_pos.y(),.5,0,BB_CIRC);
	tock = 0;
	//group = NULL;
}

Enemy::~Enemy(void){
}

void Enemy::move(void){
	if(!active) return;
	if(!tock){
		tock = 40;
		double th = (double)(rand()%40)/20.0*PI;
		_vel.x() = cos(th)*.05;
		_vel.y() = sin(th)*.05;
	}else{
		tock--;
	}
	_pos.x() += _vel.x();
	_pos.y() += _vel.y();
	body = bbody(_pos.x(),-_pos.y(),.5,0,BB_CIRC);
	//upgroupbody();
}

void Enemy::move(coord2d_t vel){
	if(!active) return;
	_vel = vel;
	_pos.x() += _vel.x();
	_pos.y() += _vel.y();
	tock = 2;
	body = bbody(_pos.x(),-_pos.y(),.5,0,BB_CIRC);
}

void Enemy::shove(Enemy *en){
	if(en->_pos.distanceTo(_pos)<.0001){
		en->push(0);
		//push(PI);
	}else{
		double th = atan2(-en->_pos.y()+_pos.y(),en->_pos.x()-_pos.x());
		en->push(th);
		//push(th+PI);
	}
}

void Enemy::push(double th){
	_pos.x() += cos(th)*.05;
	_pos.y() += sin(th)*.05;
	body = bbody(_pos.x(),-_pos.y(),.5,0,BB_CIRC);
}

void Enemy::draw(void){
	if(!active) return;
	glPushMatrix();
	glTranslatef(_pos.x(),0,-_pos.y());
	double ang = atan2(_vel.y(),_vel.x());
	glRotatef(-90.0+ang*180.0/PI,0,1,0);
	drawCharacter();
	//glutSolidSphere(.4,10,10);
	glPopMatrix();
}

bbody Enemy::aabbbody(bbody *dummy){
	dummy->VMINX = body.VCENX - body.VRAD;
	dummy->VMINZ = body.VCENZ - body.VRAD;
	dummy->VMAXX = body.VCENX + body.VRAD;
	dummy->VMAXZ = body.VCENZ + body.VRAD;
	dummy->_type = BB_AABB;
	return *dummy;
}

void Enemy::updateBin(){
	if(!active) return;
	int ix = (int)_pos.x() + 49; ix = MIN(ix,99); ix = MAX(ix,0);
	int iz = -(int)_pos.y() + 49; iz = MIN(iz,99); iz = MAX(iz,0);
	bins[ix][iz]->checkCollision(this,(alive)?WITHBIN:ENDEATH);
	list<Bin*>::iterator iter;
	for(iter=bins[ix][iz]->nbrs.begin();iter!=bins[ix][iz]->nbrs.end();iter++){
		(*iter)->checkCollision(this,(alive)?WITHBIN:ENDEATH);
	}
}

bool Enemy::colli(){
	if(!active) return false;
	int ix = (int)_pos.x() + 49; ix = MIN(ix,99); ix = MAX(ix,0);
	int iz = (int)_pos.y() + 49; iz = MIN(iz,99); iz = MAX(iz,0);
	bool b = bins[ix][iz]->checkCollision(this,WITHENE);
	list<Bin*>::iterator iter;
	for(iter=bins[ix][iz]->nbrs.begin();iter!=bins[ix][iz]->nbrs.end();iter++){
		b |= (*iter)->checkCollision(this,WITHENE);
	}
	return b;
}

bool Bin::checkCollision(bbody bod){
	if(collide(bod,box)){
		list<Enemy*>::iterator iter;
		for(iter=ene.begin();iter!=ene.end();iter++){
			if(collide(bod,(*iter)->body)){
				(*iter)->alive = false;
				return true;
			}
		}
	}
	return false;
}

bool Bin::checkCollision(Enemy *en,int op){
	if(collide(en->body,box)){
		if(op==WITHBIN){
			//WITHBIN is for purposes of putting Enemys in sBins
			ene.push_back(en);
			return true;
		}
		if(op==ENDEATH){
			ene.remove(en);
			return true;
		}
		else if(op==WITHENE){
			//WITHENE checks if en collides with other Enemys
			list<Enemy*>::iterator iter;
			bool b = false;
			for(iter=ene.begin();iter!=ene.end();iter++){
				if(en == (*iter)) continue;
				if(collide(en->body,(*iter)->body)){
					en->shove(*iter);
					b = true;
				}
			}
			return b;
		}
	}
	if(op==WITHBIN||op==ENDEATH){
		ene.remove(en);
	}
	return false;
}

//note: ints are indices, not position
bool binCollision(Enemy *en, int minx, int maxx, int minz, int maxz){
	if (minx>maxx) {int t=minx; minx = maxx; maxx = t;}
	if (minz>maxz) {int t=minz; minz = maxz; maxz = t;}
	if(!collide(en->body,bbody(b2p(minx),b2p(minz),b2p(maxx),b2p(maxz),BB_AABB))){
		return false;
	}
	bool b = false;
	if(maxx-minx==1){
		if(maxz-minz==1){
			b |= bins[minx][minz]->checkCollision(en,WITHENE);
		}else{
			int midz = (minz+maxz)/2;
			b |= binCollision(en,minx,maxx,minz,midz);
			b |= binCollision(en,minx,maxx,midz,maxz);
		}
	}else{
		int midx = (minx+maxx)/2;
		if(maxz-minz==1){
			b |= binCollision(en,minx,midx,minz,maxz);
			b |= binCollision(en,midx,maxx,minz,maxz);
		}else{
			int midz = (minz+maxz)/2;
			b |= binCollision(en,minx,midx,minz,midz);
			b |= binCollision(en,minx,midx,midz,maxz);
			b |= binCollision(en,midx,maxx,minz,midz);
			b |= binCollision(en,midx,maxx,midz,maxz);
		}
	}
	return b;
}

bool binPaCollision(source *src, int minx, int maxx, int minz, int maxz){
	if (minx>maxx) {int t=minx; minx = maxx; maxx = t;}
	if (minz>maxz) {int t=minz; minz = maxz; maxz = t;}
	if(!collide(src->body,bbody(b2p(minx),b2p(minz),b2p(maxx),b2p(maxz),BB_AABB))){
		return false;
	}
	bool b = false;
	if(maxx-minx==1){
		if(maxz-minz==1){
			b |= bins[minx][minz]->checkCollision(src->body);
		}else{
			int midz = (minz+maxz)/2;
			b |= binPaCollision(src,minx,maxx,minz,midz);
			b |= binPaCollision(src,minx,maxx,midz,maxz);
		}
	}else{
		int midx = (minx+maxx)/2;
		if(maxz-minz==1){
			b |= binPaCollision(src,minx,midx,minz,maxz);
			b |= binPaCollision(src,midx,maxx,minz,maxz);
		}else{
			int midz = (minz+maxz)/2;
			b |= binPaCollision(src,minx,midx,minz,midz);
			b |= binPaCollision(src,minx,midx,midz,maxz);
			b |= binPaCollision(src,midx,maxx,minz,midz);
			b |= binPaCollision(src,midx,maxx,midz,maxz);
		}
	}
	return b;
}
