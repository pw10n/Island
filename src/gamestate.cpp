
#include "gamestate.h"
#define MIN(x,y) ((x>y)?y:x)
#define MAX(x,y) ((x>y)?x:y)


//hitAll: false = stop after first hit; true = check all objects in Bin
int Bin::checkPaCollision(source *src, bool hitAll){
	if(hitAll) objs.unique(); //don't want duplicates if we hit every element of the list
	if(collide(src->body,box)){
		objs.remove_if(killcoto);
		list<colliobj *>::iterator iter;
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

bool Bin::checkPlCollision(playerstate *pla){
	if(collide(pla->front,box)){
		objs.remove_if(killcoto);
		list<colliobj *>::iterator iter;
		for(iter=objs.begin();iter!=objs.end();iter++){
			if((*iter)->_id==pla->_id) continue;
			if(collide(pla->front,(*iter)->body)){
				return true;
			}
		}
	}
	return false;
}

bool Bin::checkObCollision(colliobj *obj,int op){
	objs.remove_if(killcoto);
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
			list<colliobj *>::iterator iter;
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

bool killcoto(const colliobj* obj){
	//return (obj->hitWhat()==0);
	return (NULL==obj);
}

gamestate::gamestate() :
_tick(0), _lastDrawn(0), _state(GSSTATE_INACTIVE),rfire(0) {
	initBins();
}

gamestate::~gamestate(){
	_state = GSSTATE_INVALID;
	for(vector<playerstate*>::iterator it = _enemies.begin();
		it != _enemies.end(); it++){
			delete (*it);
	}
	for(vector<objectstate*>::iterator it = _objects.begin();
		it != _objects.end(); it++){
			delete (*it);
	}
	for(int i=0;i<100;i++){
		for(int j=0;j<100;j++){
			delete bins[i][j];
		}
	}
}

void gamestate::initBins() {
	for(int i=0;i<100;i++){
		for(int j=0;j<100;j++){
			bins[i][j] = new Bin(bbody(b2p(i),b2p(j),b2p(i+1),b2p(j+1),BB_AABB));
		}
	}
	//for(int i=0;i<crates.size();i++) updatBinLists(crates[i],UPDAT);
	//for(int i=0;i<others.size();i++) updatBinLists(others[i],UPDAT);
	//updatBinLists(player,UPDAT);
}

//Similar to Sma functions above but needs to check a little wider area (up to nine Bins)
void gamestate::updatBinLists(colliobj *obj,int op){
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

void gamestate::start(int time){
	_state = GSSTATE_ACTIVE;
	_tick = time;
}

void gamestate::draw(){
	//cerr << "DEBUG: gs draw method" << endl;
	if(GSSTATE_ACTIVE != _state)
		return;

	// prevent redrawing when nothing has changed
	/*if(_lastDrawn == _tick)
		return;
	_lastDrawn = _tick;*/

	//cerr << "DEBUG: I am here. " << endl;
	for(vector<objectstate*>::iterator it = _objects.begin();
		it != _objects.end(); it++){
			//cerr << "DEBUG: calling object draw method" << endl;
			//if(!cull((*it)->_pos))
				(*it)->draw();
				
	}

	//TODO: do stuff
}

/*returns true if it needs be culled*/
bool gamestate::cull(coord2d_t pos){
	//return false; 
	if((pos.x()>player->_pos.x()+14.0)||(pos.x()<player->_pos.x()-14.0)) return true;
	return ((pos.y()>player->_pos.y()+14.0)||(pos.y()<player->_pos.y()-14.0));
}

void gamestate::tick(uint32_t time){
	if(GSSTATE_ACTIVE != _state)
		return;

	uint32_t delta_time = time - _tick;
	_tick = time;
	
	//TODO: do stuff
	if(SmaPlCollision(player)) player->_vel.y() = 0;

	for(vector<objectstate*>::iterator it = _objects.begin(); it!=_objects.end();){
		if((*it)->_hp == 0){
			updatBinLists((*it), REMOV);
			delete (*it);
			it=_objects.erase(it);
		}
		else
			++it;
	}

}

void gamestate::addEnemy(playerstate* e){
	updatBinLists(e, UPDAT);
	_enemies.push_back(e);
}

void gamestate::addObject(objectstate* o){
	updatBinLists(o, UPDAT);
	_objects.push_back(o);
}

#define MIN(x,y) ((x>y)?y:x)
#define MAX(x,y) ((x>y)?x:y)

double gamestate::b2p(int i){
	return (double)(i-50);
}

//This gives quaternary search, splitting the area provided into quarters
void gamestate::LarPaCollision(source *src, int minx, int maxx, int minz, int maxz){
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

bool gamestate::LarObCollision(colliobj *obj, int minx, int maxx, int minz, int maxz){
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
int gamestate::SmaPaCollision(source *src){
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

bool gamestate::SmaObCollision(colliobj *obj){
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

bool gamestate::SmaPlCollision(playerstate *pla){
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



void gamestate::janitor(int x, int z){
	bins[x][z]->objs.unique();
	bins[x][z]->objs.remove_if(killcoto);
}



objectstate::objectstate(): _type(0){}

objectstate::objectstate(const objectstate &other){
	_id=other._id;
	_hp=other._hp;
	_type=other._type;
	_pos=other._pos;
	body=other.body;
}