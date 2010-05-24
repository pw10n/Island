#include "Bin.h"

#define MIN(x,y) ((x>y)?y:x)
#define MAX(x,y) ((x>y)?x:y)

//hitAll: false = stop after first hit; true = check all objects in Bin
int Bin::checkPaCollision(source *src, bool hitAll){
	if(collide(src->body,box)){
		list<netobj *>::iterator iter;
		for(iter=objs.begin();iter!=objs.end();iter++){
			if((*iter)->_id==src->pid) continue;
			if(collide(src->body,(*iter)->body)){
				damage(&((*iter)->_hp),src->_damage);
				if(!hitAll) return (*iter)->hitWhat();
			}
		}
	}
	return 0; //hitAll = true doesn't care what it hit, so return value doesn't matter
}

bool Bin::checkPlCollision(playerstate_t *pla){
	if(collide(pla->front,box)){
		list<netobj *>::iterator iter;
		for(iter=objs.begin();iter!=objs.end();iter++){
			if((*iter)->_id==pla->_id) continue;
			if(collide(pla->front,(*iter)->body)){
				return true;
			}
		}
	}
	return false;
}

bool Bin::checkObCollision(netobj *obj,int op){
	if(op==REMOV) {
		objs.remove(obj); //this is why I used list. Does nothing if obj is not there
		return true;
	}
	if(collide(obj->body,box)){
		if(op==UPDAT) {
			objs.push_back(obj); //obj is here; add it. duplication is not a problem as
			return true;		//remove will handle duplicates
		}
		if(op==COLLI){
			list<netobj *>::iterator iter;
			for(iter=objs.begin();iter!=objs.end();iter++){
				if(obj->_id==(*iter)->_id) continue; //don't collide with yourself
				if(collide(obj->body,(*iter)->body)){
					return true;
				}
			}
		}
	}
	else if(op==UPDAT) {
		objs.remove(obj); //obj is not here; remove it
	}
	return false;
}

double b2p(int i){
	return (double)(i-50);
}

//This gives quaternary search, splitting the area provided into quarters
void LarPaCollision(source *src, int minx, int maxx, int minz, int maxz){
	if (minx>maxx) {int t=minx; minx = maxx; maxx = t;}
	if (minz>maxz) {int t=minz; minz = maxz; maxz = t;}
	if(!collide(src->body,bbody(b2p(minx),b2p(minz),b2p(maxx),b2p(maxz),BB_AABB))){
		return;
	}
	//int h = 0;
	if(maxx-minx==1){
		if(maxz-minz==1){
			bins[minx][minz]->checkPaCollision(src,true);
		}else{
			int midz = (minz+maxz)/2;
			LarPaCollision(src,minx,maxx,minz,midz);
			//if(h) return h; //if it hit something, return; otherwise continue checking
			LarPaCollision(src,minx,maxx,midz,maxz);
		}
	}else{
		int midx = (minx+maxx)/2;
		if(maxz-minz==1){
			LarPaCollision(src,minx,midx,minz,maxz);
			LarPaCollision(src,midx,maxx,minz,maxz);
		}else{
			int midz = (minz+maxz)/2;
			LarPaCollision(src,minx,midx,minz,midz);
			LarPaCollision(src,minx,midx,midz,maxz);
			LarPaCollision(src,midx,maxx,minz,midz);
			LarPaCollision(src,midx,maxx,midz,maxz);
		}
	}
}

bool LarObCollision(netobj *obj, int minx, int maxx, int minz, int maxz){
	if (minx>maxx) {int t=minx; minx = maxx; maxx = t;}
	if (minz>maxz) {int t=minz; minz = maxz; maxz = t;}
	if(!collide(obj->body,bbody(b2p(minx),b2p(minz),b2p(maxx),b2p(maxz),BB_AABB))){
		return false;
	}
	bool b = false;
	if(maxx-minx==1){
		if(maxz-minz==1){
			b |= bins[minx][minz]->checkObCollision(obj,COLLI);
		}else{
			int midz = (minz+maxz)/2;
			b |= LarObCollision(obj,minx,maxx,minz,midz);
			b |= LarObCollision(obj,minx,maxx,midz,maxz);
		}
	}else{
		int midx = (minx+maxx)/2;
		if(maxz-minz==1){
			b |= LarObCollision(obj,minx,midx,minz,maxz);
			b |= LarObCollision(obj,midx,maxx,minz,maxz);
		}else{
			int midz = (minz+maxz)/2;
			b |= LarObCollision(obj,minx,midx,minz,midz);
			b |= LarObCollision(obj,minx,midx,midz,maxz);
			b |= LarObCollision(obj,midx,maxx,minz,midz);
			b |= LarObCollision(obj,midx,maxx,midz,maxz);
		}
	}
	return b;
}

//checks a small area (at most four Bins); don't need to narrow down
int SmaPaCollision(source *src){
	int ix = int(src->_pos.x()) + 49; ix = MAX(ix,0); ix = MIN(ix,99);
	int iz = int(src->_pos.z()) + 49; iz = MAX(iz,0); iz = MIN(iz,99);
	int h = bins[ix][iz]->checkPaCollision(src,false);
	if(h) return h;
	if(iz<99){
		h = bins[ix][iz+1]->checkPaCollision(src,false);
		if(h) return h;
	}
	if(ix<99){
		h = bins[ix+1][iz]->checkPaCollision(src,false);
		if(h) return h;
		if(iz<99){
			h = bins[ix+1][iz+1]->checkPaCollision(src,false);
			if(h) return h;
		}
	}
	return 0;
}

bool SmaObCollision(netobj *obj){
	int ix = int(obj->_pos.x()) + 49; ix = MAX(ix,0); ix = MIN(ix,99);
	int iz = int(obj->_pos.y()) + 49; iz = MAX(iz,0); iz = MIN(iz,99);
	bool b = false;
	b |= bins[ix][iz]->checkObCollision(obj,COLLI);
	if(iz<99) b |= bins[ix][iz+1]->checkObCollision(obj,COLLI);
	if(ix<99){
		b |= bins[ix+1][iz]->checkObCollision(obj,COLLI);
		if(iz<99) b |= bins[ix+1][iz+1]->checkObCollision(obj,COLLI);
	}
	return b;
}

bool SmaPlCollision(playerstate_t *pla){
	int ix = int(pla->front.VCENX) + 49; ix = MAX(ix,0); ix = MIN(ix,99);
	int iz = int(pla->front.VCENZ) + 49; iz = MAX(iz,0); iz = MIN(iz,99);
	bool b = false;
	b |= bins[ix][iz]->checkPlCollision(pla);
	if(iz<99) b |= bins[ix][iz+1]->checkPlCollision(pla);
	if(ix<99){
		b |= bins[ix+1][iz]->checkPlCollision(pla);
		if(iz<99) b |= bins[ix+1][iz+1]->checkPlCollision(pla);
	}
	return b;
}

//Similar to Sma functions above but needs to check a little wider area (up to nine Bins)
void updatBinLists(netobj *obj,int op){
	int ix = int(obj->_pos.x()) + 49; ix = MAX(ix,0); ix = MIN(ix,99);
	int iz = -int(obj->_pos.y()) + 49; iz = MAX(iz,0); iz = MIN(iz,99);
	bins[ix][iz]->checkObCollision(obj,op);
	if(iz>0) bins[ix][iz-1]->checkObCollision(obj,op);
	if(iz<99) bins[ix][iz+1]->checkObCollision(obj,op);
	if(ix>0){
		bins[ix-1][iz]->checkObCollision(obj,op);
		if(iz>0) bins[ix-1][iz-1]->checkObCollision(obj,op);
		if(iz<99) bins[ix-1][iz+1]->checkObCollision(obj,op);
	}
	if(ix<99){
		bins[ix+1][iz]->checkObCollision(obj,op);
		if(iz>0) bins[ix+1][iz-1]->checkObCollision(obj,op);
		if(iz<99) bins[ix+1][iz+1]->checkObCollision(obj,op);
	}
}

void janitor(int x, int z){
	bins[x][z]->objs.unique();
	bins[x][z]->objs.remove_if(killneto);
}

bool killneto(const netobj* obj){
	return (obj->hitWhat()==0);
}