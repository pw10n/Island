#include "attack.h"
#include "gamestate.h"
#include "particle.h"
#include <vector>
#include "gameobjects.h"
#include "collision.h"
#include "texture.h"
#include <SDL/SDL_mixer.h>
#include <SDL/SDL.h>


using namespace std;

extern gamestate *gs;
extern int cid;
extern unsigned int crateTex;
#define PLA gs->player
#define CRATEID 200

// Sounds
Mix_Chunk * smite;
Mix_Chunk * fireball;
Mix_Chunk * rapidf;
Mix_Chunk * crate;


attack::attack(void)
{
}

attack::~attack(void)
{
}

void attack::fire(int x, int y){
	if(_tock>0||PLA->_mp<_cost) return;
	_tock = _cooldown;
	if(func(x,y))
		gs->player->_mp -= _cost;
}

bool spawnFireball(int x, int y){ //dummy x,y
	Mix_PlayChannel(-1, fireball, 0);

	double fbx = -sin(PLA->vel().x());
	double fbz = -cos(PLA->vel().x());
	coord2d_t dummy;
	dummy = PLA->calcHotSpot(dummy,.6);
	gs->fbsrc.push_back(new fireball_s(dummy.x(),dummy.y(),fbx/5.0,fbz/5.0,PLA->_id));
	return true;
}
bool smiteEm(int x, int y){
	//first calc target position

	Mix_PlayChannel(-1, smite, 0);
	double dx = ((double)x-gs->GW/2.0)/1.7;
	double dy = (double)y-gs->GH/2.0 + .5;
	double dz = log((double)y*2.0/gs->GH)/-.159;
	coord2d_t targ;
	targ.x() = PLA->pos().x() - dz*dx/dy;
	targ.y() = PLA->pos().y() + dz;

	gs->smit = true;
	gs->smsrc = new smite_s(targ.x(),-targ.y(),PLA->_id);
	for(int i=0;i<400;i++){
		gs->_pars.push_back(new smite_p(gs->smsrc));
	}
	return true;
}

bool rapid(int x, int y){ //dummy x,y

	Mix_PlayChannel(-1, rapidf, 0);

	if(gs->rfpar.size()<100){
		coord2d_t dummy;
		dummy = PLA->calcHotSpot(dummy,.6);
		double vx = -sin(PLA->vel().x())*.6;
		double vz = -cos(PLA->vel().x())*.6;
		gs->rfpar.push_back(new rapidfire(dummy.x(),dummy.y(),vx,vz,0));
		return true;
	}
	return false;
}

bool spread(int x, int y){ //dummy x,y
	if(gs->rfpar.size()<100){
		coord2d_t dummy;
		dummy = PLA->calcHotSpot(dummy,.6);
		for(double d=-.3;d<.4;d+=.1){
			double vx = -sin(PLA->vel().x()+d)*.6;
			double vz = -cos(PLA->vel().x()+d)*.6;
			gs->rfpar.push_back(new rapidfire(dummy.x(),dummy.y(),vx,vz,0));
		}
		return true;
	}
	return false;
}

bool genCrate(int x, int y){

	Mix_PlayChannel(-1, crate, 0);
	double dx = ((double)x-gs->GW/2.0)/1.7;
	double dy = (double)y-gs->GH/2.0 + .5;
	double dz = log((double)y*2.0/gs->GH)/-.159;
	coord2d_t targ;
	targ.x() = PLA->pos().x() - dz*dx/dy;
	targ.y() = PLA->pos().y() + dz;
	
	goCrate* crt = new goCrate(crateTex);
	
	/*crt->_pos.x() = PLA->_pos.x() + (-sin(PLA->_vel.x()) * dist);
	crt->_pos.y() = PLA->_pos.y() + (cos(PLA->_vel.x()) * dist);*/
	crt->_pos = targ;
	double px = crt->_pos.x();
	double pz = -(crt->_pos.y());
	crt->body = bbody(px-.5,pz-.5,px+.5,pz+.5,BB_AABB);

	crt->_id = CRATEID + (cid++);
	crt->_hp = 10;

	gs->addObject(crt);
	return true;
}

void initAttacks(void){
	unsigned int icon;
	icon = BindTextureBMP((char *)"textures/fireball.bmp", true); //4
	gs->_attacks.push_back(attack(icon,10,10));
	gs->_attacks[0].func = spawnFireball;

	icon = BindTextureBMP((char *)"textures/rapid1.bmp", true); //5
	gs->_attacks.push_back(attack(icon,3,3));
	gs->_attacks[1].func = rapid;
	
	icon = BindTextureBMP((char *)"textures/crate1.bmp", true); //6
	gs->_attacks.push_back(attack(icon,25,0));
	gs->_attacks[2].func = genCrate;

	icon = BindTextureBMP((char *)"textures/regen.bmp", true); //7
	gs->_attacks.push_back(attack(icon,3,0));
	gs->_attacks[3].func = heal; 

	icon = BindTextureBMP((char *)"textures/smite.bmp", true); //8
	gs->_attacks.push_back(attack(icon,50,25));
	gs->_attacks[4].func = smiteEm;

	icon = BindTextureBMP((char *)"textures/spread.bmp", true); //5
	gs->_attacks.push_back(attack(icon,21,5));
	gs->_attacks[5].func = spread;

	//Also inits sound effects
	smite = Mix_LoadWAV("music/smite.wav");
	if(!smite){
	printf("Mix_LoadMUS(\"smite.wav\"): %s\n", Mix_GetError());
	}

	rapidf = Mix_LoadWAV("music/rapid.wav");
	if(!rapidf){
	printf("Mix_LoadMUS(\"rapid.wav\"): %s\n", Mix_GetError());
	}

	crate = Mix_LoadWAV("music/crate.wav");
	if(!crate){
	printf("Mix_LoadMUS(\"crate.wav\"): %s\n", Mix_GetError());
	}

	fireball = Mix_LoadWAV("music/fireball.wav");
	if(!fireball){
	printf("Mix_LoadMUS(\"fireball.wav\"): %s\n", Mix_GetError());
	}


}

	//just here until we setup attack selection
void loadAttacks(void){
	for(int i=0;i<5;i++)
		PLA->_ability[i] = i;
}

bool heal(int x, int y){ //dummy x,y
	if(PLA->_hp<100&&PLA->_hp>0){
		PLA->_hp += 1;
		gs->_pars.push_back(new regen(PLA));
		return true;
	}
	return false;
}