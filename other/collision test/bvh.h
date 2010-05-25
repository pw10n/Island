
#include "collision.h"
#include "particle.h"
#include <list>

using namespace std;

#define MAXTIER 4
#define WITHBIN 0 //Bin::checkCollision "with bins only"
#define WITHENE 1 //Bin::checkCollision "with enemies"
#define ENDEATH 2 // dead enemy; remove

class Bin;

extern Bin *bins[100][100];
extern double b2p(int);
extern void drawCharacter();

/*
class Groupable{
public:
	Groupable *group;
	bbody body;
	uint16_t _id;

	Groupable(void) {}
	~Groupable(void) {}
//	virtual void move(coord2d_t) = 0; //this makes it virtual, and particle abstract, nu?
	virtual void draw(void) = 0;
	void upgroupbody(void);
	virtual bbody aabbbody(bbody *) = 0;
};
*/
class Enemy{//: public Groupable{
public:
	coord2d_t _pos, _vel;
	uint8_t tock;
	bbody body;
	uint16_t _id;
	bool active;
	bool alive;

	Enemy(void);
	Enemy(bbody);
	~Enemy(void);
	void move(void);
	void move(coord2d_t);
	void shove(Enemy *);
	void push(double);
	void draw(void);
	bbody aabbbody(bbody *);
	void updateBin();
	bool colli();
	bool operator==(const Enemy& en) {return (_id==en._id);}
};
/*
class Group: public Groupable{
public:
	vector<Groupable *> sub;*/

class Bin{
public:
	bbody box;
	list<Enemy *> ene;
	list<Bin *> nbrs;

	Bin(void) {}
	Bin(bbody bod): box(bod) {}
	~Bin(void) {}
	bool checkCollision(Enemy*,int);
	bool checkCollision(bbody);
};

bool binCollision(Enemy*,int,int,int,int);
bool binPaCollision(source *src,int minx,int maxx,int minz,int maxz);