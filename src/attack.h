#pragma once

class attack
{
public:
	unsigned int _iconNum;
	unsigned int _cost;
	unsigned int _cooldown;
	unsigned int _tock;
	void (*func)(int x, int y);


	attack(void);
	attack(unsigned int iconNum, unsigned int cost, unsigned int cooldown):
	_iconNum(iconNum), _cost(cost), _cooldown(cooldown), _tock(0) {};
	~attack(void);
	void fire(int x, int y);
	void _tick(void) {if(_tock>0) _tock--;}
};

void spawnFireball(int x, int y);//dummy x,y
void smiteEm(int x, int y);
void rapid(int x, int y);//dummy x,y
void spread(int x, int y);//dummy x,y
void genCrate(int x, int y);
void initAttacks(void);
void loadAttacks(void);