//#define _CRTDBG_MAPALLOC   //used to find memory leaks
//#include <stdlib.h>
//#include <crtdbg.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <cstring>
#include <map>
#include <vector>
#include "particle.h"
#include "collision.h"
#include "netutil.h"
#include "texture.h"
#include "types.h"

//#include "md5mesh.cpp"
//#include "md5anim.cpp"
#include "md5model.h"
#include "mdmodel.h"

//#include <cstdio>
//#include <cstdlib>
#include <ctime>

#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/glut.h>

#define __STDC_LIMIT_MACROS
#include "stdint.h"

#include "gameobjects.h"
//#include "types.h"
//#include "gs_types.h"

using namespace std;

#define WORLD_TIME_RESOLUTION 30
#define HIT_CRATE 1
#define HIT_PLAYER 2
#define MAP_SIZE 50 //(set as 50)Equivalent to 100x100 this is the number of sand tiles
#define TILE_HEIGHT .0001 //used for how high water tiles above 0 level .0001 for 0 top view, .01 for 1 top view
#define TOP_VIEW 0 //Set to 1 to see birds eye view of island

uint32_t worldtime=0;

int hit_damage = 10;

#define MIN(x,y) (x>y)?y:x
#define MAX(x,y) (x>y)?x:y

typedef struct materialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess[1];
} materialStruct;

materialStruct Black = {
  {0.0, 0.0, 0.0, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {0.0}
};
materialStruct White = {
  {1.0, 1.0, 1.0, 0.0},
  {1.0, 1.0, 1.0, 0.0},
  {1.0, 1.0, 1.0, 0.0},
  {1.0}
};

materialStruct Red = {
  {1.0, 0.0, 0.0, 1.0},
  {1.0, 0.0, 0.0, 1.0},
  {1.0, 0.0, 0.0, 1.0},
  {1.0}
};

materialStruct Blue = {
  {0.0, 0.0, 1.0, 1.0},
  {0.0, 0.0, 1.0, 1.0},
  {0.0, 0.0, 1.0, 1.0},
  {1.0}
};

materialStruct Grey = {
  {0.3, 0.3, 0.3, 1.0},
  {0.3, 0.3, 0.3, 1.0},
  {0.3, 0.3, 0.3, 1.0},
  {0.3}
};

materialStruct ModeMat = {
  {0.6, 0.6, 0.6, 1.0},
  {0.3, 0.3, 0.3, 1.0},
  {0.0, 0.0, 0.0, 0.0},
  {0.0}
};


materialStruct Sand = {
	{0.85, 0.79, 0.71, 1.0},
	{0.85, 0.79, 0.71, 1.0},
	{0.0, 0.0, 0.0, 1.0},
	{0.0}
};
coord2d_t vel;
playerstate_t* player;
vector<playerstate_t> others;
//objectstate_t crates[5];

//sets up a specific material
void materials(materialStruct materials) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, materials.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materials.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, materials.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, materials.shininess);
}

void rapid(playerstate_t&);
int light;
//globals for lighting - use a white light and apply materials
//light position
GLfloat light_pos[4] = {1.0, 5.0, 1.5, 1.0}; //1.0,5.0,1.5,1.0
//light color (ambiant, diffuse and specular)
GLfloat light_amb[4] = {0.6, 0.6, 0.6, 1.0};
GLfloat light_diff[4] = {0.6, 0.6, 0.6, 1.0};
GLfloat light_spec[4] = {0.8, 0.8, 0.8, 1.0};

int mat = 0;
//set up some materials



//other globals
int GW;
int GH;
float eyex, eyey, eyez;
float LAx, LAy, LAz;
float theta;
float angle;
float myX, myY, myZ;
bool flag = false;
int rfire = 0;
GLubyte * alpha;
float fps;

fireball_s * fbsrc;
vector<fireball_p *> fbpar;
int fbtim;
explosion_s * exsrc;
vector<particle *> expar;
bool explo;
vector<rapidfire *> rfpar;
int beatim;
bool bflag;
beam * besrc;

vector<unsigned int> textures;
vector<objectstate_t*> crates;

mdmodel* fred;
mdmodel* enemy;
struct anim_info_t idlAnim, walAnim, eneAnim;


float p2w_x(int x) {
  float x1;
  x1  = (x*(2/(float)GW)) + (((1/(float)GW) - 1));
  x1 = x1 * ((float)GW/(float)GH);

return x1;

}

float p2w_y(int y) {
  
  float y1;
  y1  = (y*(2/(float)GH)) + (((1/(float)GH) - 1));

return y1;
}

void damage(uint8_t *target, int dam){
	if(*target>dam) *target -= dam;
	else *target = 0;
}

void drawCharacter();
bool cull(const coord2d_t);

////// dummy ai functions for 25% /////////
/*

AI LOGIC PLAN:

AI will move in random direction bouncing off the 
walls in a 10.0 x 10.0 map until it encounters a player
in firing range. Once the player is encountered, it will
Wait 3 ticks (30ms x 3) before firing.

Once the player has moved out of range, it continues to move
forward in the last direction it was facing.

*/


#define PSTATE_AI_SEARCHING 11
#define PSTATE_AI_TARGETING_1 12
#define PSTATE_AI_TARGETING_2 13
#define PSTATE_AI_TARGETING_3 14
#define PSTATE_AI_ATACKING 15

void init_ai(){
	for(int i=0; i<10; ++i){
		playerstate_t temp(0);
		temp._id = 1;
		temp._tick = 0;
		temp._hp = 10;
		temp._mp = 200;
		temp._weapon = 0;
		temp._state = PSTATE_AI_SEARCHING;
		temp._score = 0;
		others.push_back(temp);
	}
	others[0]._pos.x() = 35.0;
	others[0]._pos.y() = 3.0;
	others[0]._vel.x() = 45.0;
	others[0]._vel.y() = 0.05;

	others[1]._pos.x() = -43.0;
	others[1]._pos.y() = -13.0;
	others[1]._vel.x() = 32.0;
	others[1]._vel.y() = 0.05;

	others[2]._pos.x() = 20.0;
	others[2]._pos.y() = -13.0;
	others[2]._vel.x() = 15.0;
	others[2]._vel.y() = 0.05;

	others[3]._pos.x() = -23.0;
	others[3]._pos.y() = 43.0;
	others[3]._vel.x() = -45.0;
	others[3]._vel.y() = 0.05;

	others[4]._pos.x() = 5.0;
	others[4]._pos.y() = 0.5;
	others[4]._vel.x() = 10.0;
	others[4]._vel.y() = 0.05;

	others[5]._pos.x() = 15.0;
	others[5]._pos.y() = 3.0;
	others[5]._vel.x() = 25.0;
	others[5]._vel.y() = 0.05;

	others[6]._pos.x() = -3.0;
	others[6]._pos.y() = -13.0;
	others[6]._vel.x() = 82.0;
	others[6]._vel.y() = 0.05;

	others[7]._pos.x() = 10.0;
	others[7]._pos.y() = -13.0;
	others[7]._vel.x() = 55.0;
	others[7]._vel.y() = 0.05;

	others[8]._pos.x() = -13.0;
	others[8]._pos.y() = 31.0;
	others[8]._vel.x() = -25.0;
	others[8]._vel.y() = 0.05;

	others[9]._pos.x() = 4.0;
	others[9]._pos.y() = 41.5;
	others[9]._vel.x() = 30.0;
	others[9]._vel.y() = 0.05;
}

void drawAi(){
	for(vector<playerstate_t>::iterator it = others.begin();
		it != others.end();
		++it)
	{
		if(cull((*it)._pos)) continue;
		glPushMatrix();
		//translate
		glTranslatef((*it)._pos.x(), 0.2, -(*it)._pos.y());
		//rotate
		glRotatef((*it)._vel.x() * (180.0 / M_PI), 0.0, 1.0, 0.0);
		drawCharacter();
		glPopMatrix();
	}
}

#define MIN_AI_DISTANCE 7.0
#define AI_BOUNDS_MAX 40.0
#define AI_BOUNDS_MIN -40.0
void tickAi(uint32_t time){
	coord2d_t dummy;
	for(vector<playerstate_t>::iterator it = others.begin();
		it != others.end();
		it=((*it)._hp<=0)?others.erase(it):it+1)
	{
		if ((*it)._hp <= 0){
			player->_score++;
		}
		switch((*it)._state){
			case PSTATE_AI_SEARCHING:
				// move forward
				(*it)._pos.x() += (-sin((*it)._vel.x()) * (*it)._vel.y());
				(*it)._pos.y() += (cos((*it)._vel.x()) * (*it)._vel.y());
				(*it).body = bbody((*it)._pos.x(),-(*it)._pos.y(),1.0,0,BB_CIRC);
				(*it).front = bbody((*it).calcHotSpot(dummy,.6),.1,BB_CIRC);

				// check bounds
				if ((*it)._pos.x() > AI_BOUNDS_MAX){
					(*it)._pos.x() = AI_BOUNDS_MAX;
					(*it)._vel.x() += M_PI;
				}
				else if ((*it)._pos.x() < AI_BOUNDS_MIN){
					(*it)._pos.x() = AI_BOUNDS_MIN;
					(*it)._vel.x() += M_PI;
				}
				if ((*it)._pos.y() > AI_BOUNDS_MAX){
					(*it)._pos.y() = AI_BOUNDS_MAX;
					(*it)._vel.x() += M_PI;
				}
				else if ((*it)._pos.y() < AI_BOUNDS_MIN){
					(*it)._pos.y() = AI_BOUNDS_MIN;
					(*it)._vel.x() += M_PI;
				}

				if((*it)._pos.distanceTo((*player)._pos) < MIN_AI_DISTANCE )
					(*it)._state = PSTATE_AI_TARGETING_1;
				

				break;
			case PSTATE_AI_TARGETING_1:
				// if still in range
				if((*it)._pos.distanceTo((*player)._pos) < MIN_AI_DISTANCE ){
					coord2d_t pos = (*it)._pos-player->_pos ;
					float theta=(*it)._vel.x();
					if (pos.x()==0 && pos.x()<0) // handle div by zero case.
						theta = M_PI/2.0f;
					else if (pos.y()==0 && pos.x()>0) // handle div by zero case.
						theta = 3.0f*M_PI/2.0f;
					else if (pos.y()<0 && pos.x()<0)
						theta = atan((float)pos.x()/(float)pos.y());
					else if (pos.y()>0 && pos.x()<=0)
						theta = atan((float)pos.x()/(float)pos.y())+M_PI;
					else if (pos.y()<0 && pos.x()>=0)
						theta = atan((float)pos.x()/(float)pos.y())+2*M_PI;
					else if (pos.y()>0 && pos.x()>0)
						theta = atan((float)pos.x()/(float)pos.y())+M_PI;
						
					//angle=theta*(180.0f / M_PI);
					(*it)._vel.x() = -theta;
					(*it)._state = PSTATE_AI_TARGETING_2;
				}
				// else : return to searching state
				else
					(*it)._state = PSTATE_AI_SEARCHING;
				break;
			case PSTATE_AI_TARGETING_2:
				// if still in range
				if((*it)._pos.distanceTo((*player)._pos) < MIN_AI_DISTANCE ){
					(*it)._state = PSTATE_AI_TARGETING_3;
				}
				// else : return to searching state
				else
					(*it)._state = PSTATE_AI_SEARCHING;
				break;
			case PSTATE_AI_TARGETING_3:
				// if still in range
				if((*it)._pos.distanceTo((*player)._pos) < MIN_AI_DISTANCE ){
					
					(*it)._state = PSTATE_AI_ATACKING;
				}
				// else : return to searching state
				else
					(*it)._state = PSTATE_AI_SEARCHING;
				break;
			case PSTATE_AI_ATACKING:
				// TODO: FIRE
				{
					coord2d_t save = (*it)._vel;
					(*it)._vel.x() += (rand() % 10)*0.1f-0.5f;
					rapid((*it));
					(*it)._vel = save;
				}
				// if still in rage
				if((*it)._pos.distanceTo((*player)._pos) < MIN_AI_DISTANCE )
					(*it)._state = PSTATE_AI_TARGETING_1;
				// else : return to searching state
				else
					(*it)._state = PSTATE_AI_SEARCHING;
				break;
			default:
				(*it)._state = PSTATE_AI_SEARCHING;
		}
	}
}


//////////////////////////////////////////



void genTex(){
	alpha = new GLubyte[16 * 16];
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++) alpha[i+16*j] = 17*(MAX((i+j-5),0)); //17
		for(j=8;j<16;j++) alpha[i+16*j] = alpha[i+16*(15-j)]; //mirror
	}
	for(i=8;i<16;i++){
		for(j=0;j<16;j++) alpha[i+16*j] = alpha[(15-i)+16*j]; //mirror again
	}
}

unsigned int init_particletex() {
	unsigned int texid;
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	genTex();
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texid);
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 16, 16, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alpha);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);

	return texid;
}

void init_particle(){
	glNewList(PARTLIST,GL_COMPILE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[PARTICLE_TEXTURE]);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0.1f,0.1f,0);
	glTexCoord2d(0,1); glVertex3f(-0.1f,0.1f,0);
	glTexCoord2d(1,0); glVertex3f(0.1f,-0.1f,0);
	glTexCoord2d(0,0); glVertex3f(-0.1f,-0.1f,0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, textures[PARTICLE_TEXTURE]);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0.1f,0,0.1f);
	glTexCoord2d(0,1); glVertex3f(-0.1f,0,0.1f);
	glTexCoord2d(1,0); glVertex3f(0.1f,0,-0.1f);
	glTexCoord2d(0,0); glVertex3f(-0.1f,0,-0.1f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, textures[PARTICLE_TEXTURE]);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0,0.1f,0.1f);
	glTexCoord2d(0,1); glVertex3f(0,-0.1f,0.1f);
	glTexCoord2d(1,0); glVertex3f(0,0.1f,-0.1f);
	glTexCoord2d(0,0); glVertex3f(0,-0.1f,-0.1f);
	glEnd();
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEndList();
}

//initialization calls for opengl for static light
//note that we still need to enable lighting in order for it to work
void init_lighting() {
  //turn on light0
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_FLAT);
  //set up the diffuse, ambient and specular components for the light
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);
  //specify our lighting model as 1 normal per face
}

void pos_light() {
  //set the light's position
  glMatrixMode(GL_MODELVIEW);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}

void spawnFireball(){
	if (player->_mp<10) {
		return;
	}
	fbtim = 0;
	double fbx = -sin(theta);
	double fbz = -cos(theta);
	coord2d_t dummy;
	dummy = player->calcHotSpot(dummy,.6);
	fbsrc = new fireball_s(dummy.x(),dummy.y(),fbx/5.0,fbz/5.0,player->_id);
	for(int i=0;i<200;i++){
		fbpar.push_back(new fireball_p(fbsrc));
	}
	if (player->_mp>=10) {
		player->_mp -= 10;
		
	}


}

void detonate(source * ws, bool splin){
	explo = true;
	exsrc = new explosion_s(ws->_pos.x(),ws->_pos.z());
	if(!splin){
		for(int i=0;i<400;i++){
			expar.push_back(new explosion_p(exsrc));
		}
	}
	else{
		if (ws->_type == PARTICLE_FIREBALL){
			for(int i=0;i<200;i++){
				expar.push_back(new explosion_p(exsrc));
			}
			for(int i=0;i<200;i++){
				expar.push_back(new splinter(exsrc));
			}
		}
		else if (ws->_type == PARTICLE_RAPID){
			for(int i=0;i<200;i++){
				expar.push_back(new splinter(exsrc));
			}
		}
	}
}

void rapid(playerstate_t& player){
	if (player._mp<5){
		return;
	}
	if(rfpar.size()<100&&rfire==0){
		coord2d_t dummy;
		dummy = player.calcHotSpot(dummy,.6);
		double vx = -sin(player._vel.x())*.6;
		double vz = -cos(player._vel.x())*.6;
		rfpar.push_back(new rapidfire(dummy.x(),dummy.y(),vx,vz,player._id));
		if (player._mp>=5 && player._id == 0) {
			player._mp -= 5;
			
		}
	}

}

void reshape(int w, int h) {
  GW = w;
  GH = h;


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if(TOP_VIEW == 1) {
	  gluPerspective(80, w/h, .5, 76);
  }
  else {
	  if (h!=0){
		  gluPerspective(80, w/h, .5, 24);
	  }
  }

  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, w, h);
  
  glutPostRedisplay();
}



void drawGrid() {
  int rows = 50;
  int columns = 50;

  glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    materials(Black);
    /* Horizontal lines. */
    for (int i=-rows; i<=rows; i++) {
      glVertex3f(columns, 0, i);
      glVertex3f(-columns, 0, i);
    }
    /* Vertical lines. */
    for (int i=-columns; i<=columns; i++) {
      glVertex3f(i, 0, -rows);
      glVertex3f(i, 0, rows);
    }
  glEnd();
  materials(Sand);
  /*glBegin(GL_POLYGON);


	glVertex3f(columns,-0.01,rows);
	glVertex3f(columns,-0.01,-rows);
	glVertex3f(-columns,-0.01,-rows);
	glVertex3f(-columns,-0.01,rows);
  glEnd();*/
}

// draws the character facing forward.

void drawCharacter(){
	materials(Grey);
	glPushMatrix();

	glRotatef(180.0f, 0.0, 1.0, 0.0);

	glTranslatef(0.0, 0.25, 0.0);

	enemy->draw(eneAnim);

	glPopMatrix();
}

void drawPlayer() {
  materials(Grey);
  glPushMatrix();
  
    glTranslatef(0.0, 0.25, 0.0);
    glTranslatef(0.0, 0.0, -0.5);
    gluCylinder(gluNewQuadric(), .05, .2, 1, 12, 36);
  glPopMatrix();
}

void drawAniPlayer(bool walk){
   glPushMatrix();
   materials(ModeMat);
   fred->draw((walk)?walAnim:idlAnim);
   glPopMatrix();
}

void drawFireball() {
	fbsrc->draw();
	for(uint32_t i=0;i<fbpar.size();i++){
		fbpar[i]->draw();
	}
}

void drawExplosion() {
	//exsrc->draw();
	for(uint32_t i=0;i<expar.size();i++){
		expar[i]->draw();
	}
}

void drawRapid() {
	for(uint32_t i=0;i<rfpar.size();i++){
		rfpar[i]->draw();
	}
}
void renderBitmapString(
		float x, 
		float y, 
		void *text, 
		char *buf) {  
  char *d;
  glRasterPos3f(x, y, .1);
  for (d=buf; *d != '\0'; d++) {
    glutBitmapCharacter(text, *d);
  }
}



void setOrthoProjection() {


	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

	glLoadIdentity();

	gluOrtho2D(0, GW, 0, GH);

	glScalef(1, -1, 1);

	glTranslatef(0, -GH, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}


void drawCrates(){

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// it'll be 2x2x2 for now
	glBindTexture(GL_TEXTURE_2D, textures[OBJECTSTATE_CRATE]);
	// "front"
	for(unsigned int i = 0; i < crates.size(); i++){
		if(cull(crates[i]->_pos)) continue;
		glPushMatrix();
		glTranslatef(crates[i]->_pos.x(),0,-crates[i]->_pos.y());
		glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (-.5, 0.0, 0.5);
		glTexCoord2f (1.0, 0.0);
		glVertex3f (0.5, 0.0, 0.5);
		glTexCoord2f (1.0, 1.0);
		glVertex3f (0.5, 1.0, 0.5);
		glTexCoord2f (0.0, 1.0);
		glVertex3f (-.5, 1.0, 0.5);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, ((crate*)crates[i])->texture);
		glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (0.5, 0.0, -.5);
		glTexCoord2f (1.0, 0.0);
		glVertex3f (0.5, 0.0, 0.5);
		glTexCoord2f (1.0, 1.0);
		glVertex3f (0.5, 1.0, 0.5);
		glTexCoord2f (0.0, 1.0);
		glVertex3f (0.5, 1.0, -.5);
		glEnd();
		//glBindTexture(GL_TEXTURE_2D, ((crate*)crates[i])->texture);
		glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (-.5, 1.0, -.5);
		glTexCoord2f (1.0, 0.0);
		glVertex3f (0.5, 1.0, -.5);
		glTexCoord2f (1.0, 1.0);
		glVertex3f (0.5, 1.0, 0.5);
		glTexCoord2f (0.0, 1.0);
		glVertex3f (-.5, 1.0, 0.5);
		glEnd();
		//glBindTexture(GL_TEXTURE_2D, ((crate*)&crates[i])->texture);
		glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (-.5, 0.0, 0.5);
		glTexCoord2f (1.0, 0.0);
		glVertex3f (-.5, 0.0, -.5);
		glTexCoord2f (1.0, 1.0);
		glVertex3f (-.5, 1.0, -.5);
		glTexCoord2f (0.0, 1.0);
		glVertex3f (-.5, 1.0, 0.5);
		glEnd();
		//glBindTexture(GL_TEXTURE_2D, ((crate*)&crates[i])->texture);
		//back
		glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f (-.5, 0.0, -0.5);
		glTexCoord2f (1.0, 0.0);
		glVertex3f (0.5, 0.0, -0.5);
		glTexCoord2f (1.0, 1.0);
		glVertex3f (0.5, 1.0, -0.5);
		glTexCoord2f (0.0, 1.0);
		glVertex3f (-.5, 1.0, -0.5);
		glEnd();
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
}


void drawBox(unsigned int texture) {
	if (texture == 0) {
	
		glColor3f(.7, .7, .7);
		glBegin(GL_QUADS);
		glVertex2f(-25, -25);
		glVertex2f(-25, 25); 
		glVertex2f(25, 25); 
		glVertex2f(25, -25);
		glEnd();

	}
	else {
	  glColor3f(1, 1, 1);

	  glEnable(GL_TEXTURE_2D);
	  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	  glBindTexture(GL_TEXTURE_2D, textures[texture]);

	  glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex2f(-25, -25);
	  glTexCoord2f (1, 0.0);
	  glVertex2f(-25, 25); 
	  glTexCoord2f (1, 1);
	  glVertex2f(25, 25); 
	  glTexCoord2f (0.0, 1);
	  glVertex2f(25, -25);
	  glEnd();
	  glDisable(GL_TEXTURE_2D);

	}
}

void drawBar() {

    glBegin(GL_QUADS);
    glVertex2f(-15, -100);
    glVertex2f(-15, 100); 
    glVertex2f(15, 100); 
    glVertex2f(15, -100);
    glEnd();
}

void drawUIBar(int texture) {

	if (texture == 0) {
	
		glColor3f(.7, .7, .7);
		glBegin(GL_QUADS);
		glVertex2f(-40, -180);
		glVertex2f(-40, 180); 
		glVertex2f(40, 180); 
		glVertex2f(40, -180);
		glEnd();

	}
	else {
	  glColor3f(1, 1, 1);

	  glEnable(GL_TEXTURE_2D);
	  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);

	  glBindTexture(GL_TEXTURE_2D, textures[texture]);

	  glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex2f(-40, -180);
	  glTexCoord2f (1, 0.0);
	  glVertex2f(-40, 180); 
	  glTexCoord2f (1, 1);
	  glVertex2f(40, 180); 
	  glTexCoord2f (0.0, 1);
	  glVertex2f(40, -180);
	  glEnd();
	  glDisable(GL_TEXTURE_2D);


	}


}

void drawUIHBar(int texture) {
	if (texture == 0) {
	
		glColor3f(.7, .7, .7);
		glBegin(GL_QUADS);
		glVertex2f(-100, -120);
		glVertex2f(-100, 120); 
		glVertex2f(100, 120); 
		glVertex2f(100, -120);
		glEnd();

	}
	else {
	  glColor3f(1, 1, 1);

	  glEnable(GL_TEXTURE_2D);
	  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	  glBindTexture(GL_TEXTURE_2D, textures[texture]);

	  glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex2f(-50, -120);
	  glTexCoord2f (1, 0.0);
	  glVertex2f(-50, 120); 
	  glTexCoord2f (1, 1);
	  glVertex2f(50, 120); 
	  glTexCoord2f (0.0, 1);
	  glVertex2f(50, -120);
	  glEnd();
	  glDisable(GL_TEXTURE_2D);

	}

}


void draw_circle2() {
    //Code from wikipedia.
    int i;
    int sections = 30; 
    GLfloat radius = 70.0f;
    GLfloat twoPi = 2.0f * 3.14159f;


    glBegin(GL_TRIANGLE_FAN);
        //glColor3f(0.5, 0.2, 0.8); 

        glVertex2f(0, 0);
          //glVertex2f(.5, .5);
        for(i = 0; i <= sections;i++) {
          glVertex3f((radius * cos(i *  twoPi / sections)) + 0, 
          (radius* sin(i * twoPi / sections)) + 0, -.001);
        }
    
    glEnd();
}

void draw_circle() {
    //Code from wikipedia.
    /*int i;
    int sections = 30; 
    GLfloat radius = 50.0f;
    GLfloat twoPi = 2.0f * 3.14159f;


    glBegin(GL_TRIANGLE_FAN);
        //glColor3f(0.5, 0.2, 0.8); 

        glVertex2f(0, 0);
          //glVertex2f(.5, .5);
        for(i = 0; i <= sections;i++) {
          glVertex2f((radius * cos(i *  twoPi / sections)) + 0, 
          (radius* sin(i * twoPi / sections)) + 0);
        }
    
    glEnd();*/
    glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, textures[7]);

    glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex2f(-40, -40);
	glTexCoord2f (1, 0.0);
	glVertex2f(-40, 40); 
	glTexCoord2f (1, 1);
	glVertex2f(40, 40); 
	glTexCoord2f (0.0, 1);
	glVertex2f(40, -40);
	glEnd();
	glDisable(GL_TEXTURE_2D);




}

void displayHud(){
	char buff[40];
		/* BEGIN HUD */
	glDisable(GL_LIGHTING);
	//materials(Red);
	glColor3f(1.0, 0.0, 0.0);
	if(player->_hp == 0) {
		sprintf(buff, "YOU DIED");
		renderBitmapString((GW/2.0)-15, GH/2.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
	}

	//materials(Black);
	glColor3f(1, 1, 1);
	/*sprintf(buff, "Rapid");
	renderBitmapString((GW/3.0)-15, GH-112,GLUT_BITMAP_HELVETICA_12,buff);
	sprintf(buff, "Fire");
	renderBitmapString((GW/3.0)-10, GH-100,GLUT_BITMAP_HELVETICA_12,buff);*/
	sprintf(buff, "(a)");
	renderBitmapString(50, GH-240,GLUT_BITMAP_HELVETICA_12,buff);
	/*sprintf(buff, "FireBall");
	renderBitmapString((GW/3.0)+30, GH-110,GLUT_BITMAP_HELVETICA_12,buff);*/
	sprintf(buff, "(s)");
	renderBitmapString(50, GH-300,GLUT_BITMAP_HELVETICA_12,buff);
	/*sprintf(buff, "Crate");
	renderBitmapString((GW/3.0)+80, GH-110,GLUT_BITMAP_HELVETICA_12,buff);*/
	sprintf(buff, "(d)");
	renderBitmapString(50, GH-360,GLUT_BITMAP_HELVETICA_12,buff);
	sprintf(buff, "(f)");
	renderBitmapString(50, GH-420,GLUT_BITMAP_HELVETICA_12,buff);
	sprintf(buff, "(g)");
	renderBitmapString(50, GH-480,GLUT_BITMAP_HELVETICA_12,buff);
	/*sprintf(buff, "Melee");
	renderBitmapString((GW/5.0)-30, GH-65,GLUT_BITMAP_TIMES_ROMAN_24 ,buff);*/
	glColor3f(0, 0, 0);
	sprintf(buff, "(Left Click)");
	renderBitmapString(10, GH-110,GLUT_BITMAP_HELVETICA_12,buff);


    /*glColor3f(0.7, 0.7, 0.7);    //maybe used for later
	glPushMatrix();
	glTranslatef(GW/5.0, GH-150, 0);
	draw_circle2();
	glPopMatrix();*/

	//materials(Gray);
	glColor3f(0.7, 0.7, 0.7); //fist
	glPushMatrix();
	glTranslatef(40, GH-40, 0);
	draw_circle2();
	draw_circle();
	glPopMatrix();

	/*glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(GW/5.0, GH-150, 0);
	draw_circle2();
	glPopMatrix();*/


	//materials(Black);

    glLoadIdentity();

	glColor3f(0.7, 0.7, 0.7);


	glPushMatrix();  //UI bar that covers icons
	glTranslatef(40, GH-360, -.01);
	drawUIBar(8);
	glPopMatrix();

	glPushMatrix();  //UI bar that Health bars
	glTranslatef(GW-60, GH-95, -.01);
	drawUIHBar(9);
	glPopMatrix();

	glColor3f(.5, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(GW-80, GH + 100 + (200*(-player->_hp/100.0)), 0); //red health bar
	drawBar();
	glPopMatrix();


	glColor3f(0.0, 0.0, .5);
	glPushMatrix();
	glTranslatef(GW-40, GH + 100 + (200*(-player->_mp/200.0)), 0); // blue mana bar
	drawBar();
	glPopMatrix();

	glColor3f(0, 0, 0);
	glPushMatrix();
	glTranslatef(GW-40, GH-100, 0);  //gray behind red health bar
	drawBar();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(GW-80, GH-100, 0);  //gray behind blue mana bar
	drawBar();
	glPopMatrix();


	glPushMatrix();  //rapid fire (a)
	glTranslatef(25, GH-240, 0);
	drawBox(5);
	glPopMatrix();

    glPushMatrix();  //fireball (s)
	glTranslatef(25, GH-300, 0);
	drawBox(4);
	glPopMatrix();

	glPushMatrix();  //crate (d)
	glTranslatef(25, GH-360, 0);
	drawBox(6);
	glPopMatrix();

	glPushMatrix(); //(f)
	glTranslatef(25, GH-420, 0);
	drawBox(0);
	glPopMatrix();

	glPushMatrix(); //(g) 
	glTranslatef(25, GH-480, 0);
	drawBox(0);
	glPopMatrix();


	//materials(Black);
	glColor3f(0.0, 0.0, 0.0);
	sprintf(buff, "Health: %d", player->_hp);
    renderBitmapString(2*GW/10.0,GH/11.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
	sprintf(buff, "Kills: %d", player->_score);
	renderBitmapString(5*GW/10.0,GH/11.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
    sprintf(buff, "FPS: %f", fps);
	renderBitmapString(7*GW/10.0,GH/11.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
  glPopMatrix();
  resetPerspectiveProjection();
glEnable(GL_LIGHTING);
      /* END HUD */
}

void drawTiles(){
  glDisable(GL_LIGHTING);
  glColor3f(1, 1, 1);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, textures[2]);

  glBegin(GL_QUADS);


  glTexCoord2f (0.0, 0.0);
  glVertex3f(-MAP_SIZE, 0, MAP_SIZE);
  glTexCoord2f (MAP_SIZE, 0.0);
  glVertex3f(-MAP_SIZE, 0, -MAP_SIZE);
  glTexCoord2f (MAP_SIZE, MAP_SIZE);
  glVertex3f(MAP_SIZE, 0, -MAP_SIZE);
  glTexCoord2f (0.0, MAP_SIZE);
  glVertex3f(MAP_SIZE, 0, MAP_SIZE);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);




}

void waterTile() {

  glDisable(GL_LIGHTING);
  glColor3f(1, 1, 1);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, textures[3]);
  glBegin(GL_QUADS);


  glTexCoord2f (0.0, 0.0);
  glVertex3f(-1, TILE_HEIGHT, 1);
  glTexCoord2f (1, 0.0);
  glVertex3f(-1, TILE_HEIGHT, -1);
  glTexCoord2f (1, 1);
  glVertex3f(1, TILE_HEIGHT, -1);
  glTexCoord2f (0.0, 1);
  glVertex3f(1, TILE_HEIGHT, 1);
  glEnd();


  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

void drawWaterTiles(float x, float y) {
	coord2d_t pos; pos.x() = x; pos.y() = -y;
	if(cull(pos)) return;
	glPushMatrix();
	  glTranslatef(-MAP_SIZE+x, 0, MAP_SIZE-y);
	  waterTile();
	glPopMatrix();
}

void waterBlock(int angle) {
	glPushMatrix();
	glRotatef(angle, 0, 1, 0);
	drawWaterTiles(1, 1);
	drawWaterTiles(3, 1);
	drawWaterTiles(1, 3);
	drawWaterTiles(3, 3);
	
	drawWaterTiles(5, 1);
	drawWaterTiles(1, 5);
	drawWaterTiles(3, 5);
	drawWaterTiles(5, 3);

	drawWaterTiles(7, 1);
	drawWaterTiles(1, 7);
	drawWaterTiles(5, 5);
	drawWaterTiles(3, 7);
	drawWaterTiles(7, 3);
	drawWaterTiles(9, 1);
	drawWaterTiles(1, 9);

	drawWaterTiles(11, 1);
	drawWaterTiles(1, 11);
	drawWaterTiles(3, 9);
	drawWaterTiles(9, 3);
	drawWaterTiles(5, 7);
	drawWaterTiles(7, 5);

	drawWaterTiles(13, 1);
	drawWaterTiles(1, 13);
	drawWaterTiles(11, 3);
	drawWaterTiles(3, 11);
	drawWaterTiles(5, 9);
	drawWaterTiles(9, 5);
	drawWaterTiles(7, 7);
	drawWaterTiles(7, 7);

	glPopMatrix();
}

void drawWater() {
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, textures[3]);


	glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex3f(-MAP_SIZE-20, -0.1, MAP_SIZE+20);
	  glTexCoord2f (MAP_SIZE, 0.0);
	  glVertex3f(-MAP_SIZE-20, -0.1, -MAP_SIZE-20);
	  glTexCoord2f (MAP_SIZE, MAP_SIZE);
	  glVertex3f(MAP_SIZE+20, -0.1, -MAP_SIZE-20);
	  glTexCoord2f (0.0, MAP_SIZE);
	  glVertex3f(MAP_SIZE+20, -0.1, MAP_SIZE+20);
	glEnd();
	/*//Left body of water
	glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex3f(-MAP_SIZE-20, 0, MAP_SIZE);
	  glTexCoord2f (20, 0.0);
	  glVertex3f(-MAP_SIZE-20, 0, -MAP_SIZE);
	  glTexCoord2f (20, 20);
	  glVertex3f(-MAP_SIZE, 0, -MAP_SIZE);
	  glTexCoord2f (0.0, 20);
	  glVertex3f(-MAP_SIZE, 0, MAP_SIZE);
	glEnd();

	//Right body of water
	glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex3f(MAP_SIZE+20, 0, MAP_SIZE);
	  glTexCoord2f (20, 0.0);
	  glVertex3f(MAP_SIZE+20, 0, -MAP_SIZE);
	  glTexCoord2f (20, 20);
	  glVertex3f(MAP_SIZE, 0, -MAP_SIZE);
	  glTexCoord2f (0.0, 20);
	  glVertex3f(MAP_SIZE, 0, MAP_SIZE);
	glEnd();

	//Top body of water
	glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex3f(-MAP_SIZE-20, 0, -MAP_SIZE);
	  glTexCoord2f (20, 0.0);
	  glVertex3f(-MAP_SIZE-20, 0, -MAP_SIZE-20);
	  glTexCoord2f (20, 20);
	  glVertex3f(MAP_SIZE+20, 0, -MAP_SIZE-20);
	  glTexCoord2f (0.0, 20);
	  glVertex3f(MAP_SIZE+20, 0, -MAP_SIZE);
	glEnd();

	//Bottom body of water
	glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex3f(-MAP_SIZE-20, 0, MAP_SIZE);
	  glTexCoord2f (20, 0.0);
	  glVertex3f(-MAP_SIZE-20, 0, MAP_SIZE+20);
	  glTexCoord2f (20, 20);
	  glVertex3f(MAP_SIZE+20, 0, MAP_SIZE+20);
	  glTexCoord2f (0.0, 20);
	  glVertex3f(MAP_SIZE+20, 0, MAP_SIZE);
	glEnd();*/

	/*for (int i=0; i<5; i+=2) {
	  for (int j=0; j<5; j+=2) {
	    drawWaterTiles(i, j);
		drawWaterTiles(j, i);
	  }

	}*/
	glDisable(GL_TEXTURE_2D);

	waterBlock(0);
	waterBlock(90);
	waterBlock(180);
	waterBlock(270);
	



	glEnable(GL_LIGHTING);
}

/*returns true if it needs be culled*/
bool cull(coord2d_t pos){
	//return false; 
	if((pos.x()>player->_pos.x()+14.0)||(pos.x()<player->_pos.x()-14.0)) return true;
	return ((pos.y()>player->_pos.y()+14.0)||(pos.y()<player->_pos.y()-14.0));
}

void display() {
  static int frame=0;
  static int lasttime=0;
  
  
  int time = glutGet(GLUT_ELAPSED_TIME);


  
  ++frame;

  if (time - lasttime > 1000){
    fps = frame*1000.0/(time-lasttime);
    lasttime = time;
    frame = 0;
  }




  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 

  glMatrixMode(GL_MODELVIEW);



  glPushMatrix();
  



  setOrthoProjection();
  glPushMatrix();
	glLoadIdentity();
  

    displayHud();



    glPushMatrix();

	//player constraints
    if (player->_pos.x()>MAP_SIZE) {
		player->_pos.x() = MAP_SIZE;
    }
    if (player->_pos.x()<-MAP_SIZE) {
		player->_pos.x() = -MAP_SIZE;
    }
    if (player->_pos.y()>MAP_SIZE) {
		player->_pos.y() = MAP_SIZE;
    }
    if (player->_pos.y()<-MAP_SIZE) {
		player->_pos.y() = -MAP_SIZE;
    }

  //set up the camera

    gluLookAt(eyex + player->_pos.x(), eyey, eyez - player->_pos.y(), LAx + player->_pos.x(), LAy, LAz - player->_pos.y(), 0, 0, -1);
    glPushMatrix();



	drawTiles();
	drawWater();


    glPushMatrix();



		glTranslatef(player->_pos.x(), 0, -player->_pos.y());
        glRotatef(angle, 0, 1, 0);
        glRotatef(180,0,1,0);
		if(!(player->_hp == 0)) {
			//drawPlayer();
         drawAniPlayer((player->_vel.y()==.005));
		}

      glPopMatrix();

    

	  glPushMatrix();
	  drawAi();

	  glPopMatrix();

      glPushMatrix();
        glTranslatef(0.0, 0.01, 0.0);
		    //materials(Sand);
       // drawGrid();
		    //glTranslatef(-1.0,0,-1.0);
		    drawCrates();
		//glutSolidSphere(1.0,10,10);
		    if(beatim>-1) besrc->draw();
      glPopMatrix();
	  if(fbtim>-1) drawFireball();
	  if(explo) drawExplosion();
	  drawRapid();
    glPopMatrix();
  glPopMatrix();
  
  glutSwapBuffers();
    
}



void mouse(int button, int state, int x, int y) {
  if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) { 
		flag = true;
		vel.y() = 0.005;
    }
	else {
		flag = false;
		vel.y() = 0.000;
	}
  }
	if(button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			bflag = true;
			beatim = 5;
		}
		else bflag = false;
	}
}

void processMousePassiveMotion(int x, int y) {


	//float theta = 0;
	x -= GW/2;
	x *= .5;
	y -= GH/2 - 50;
	
	//        0  dir
	//        | /
	//        |/ 
	// 270----------90
	//        |
	//        |
	//       180
	
	if (y==0 && x<0) // handle div by zero case.
		theta = M_PI/2.0f;
	else if (y==0 && x>0) // handle div by zero case.
		theta = 3.0f*M_PI/2.0f;
	else if (y<0 && x<0)
		theta = atan((float)x/(float)y);
	else if (y>0 && x<=0)
		theta = atan((float)x/(float)y)+M_PI;
	else if (y<0 && x>=0)
		theta = atan((float)x/(float)y)+2*M_PI;
	else if (y>0 && x>0)
		theta = atan((float)x/(float)y)+M_PI;
		
	angle=theta*(180.0f / M_PI);
	vel.x() = theta;
	
	
  glutPostRedisplay();

}

void processMouseActiveMotion(int x, int y) {
	
	//float theta = 0;
	x -= GW/2;
	x *= .5;
	y -= GH/2 - 50;
	
	//        0  dir
	//        | /
	//        |/ 
	// 270----------90
	//        |
	//        |
	//       180
	
	if (y==0 && x<0) // handle div by zero case.
		theta = M_PI/2.0f;
	else if (y==0 && x>0) // handle div by zero case.
		theta = 3.0f*M_PI/2.0f;
	else if (y<0 && x<0)
		theta = atan((float)x/(float)y);
	else if (y>0 && x<=0)
		theta = atan((float)x/(float)y)+M_PI;
	else if (y<0 && x>=0)
		theta = atan((float)x/(float)y)+2*M_PI;
	else if (y>0 && x>0)
		theta = atan((float)x/(float)y)+M_PI;
		





	angle=theta*(180.0f / M_PI);
	//myX += -sin(theta);
	//myZ += cos(theta);

	vel.x() = theta;


	vel.y() = (flag)?.005:0;
	if(bflag) beatim = 5;

  glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y ){
  float dist = 3.0;
  switch( key ) {
    case 'q': case 'Q' :
		for(uint32_t i=0;i<fbpar.size();i++){
			delete fbpar[i];
		}
		for(uint32_t i=0;i<expar.size();i++){
			delete expar[i];
		}
		for(uint32_t i=0;i<rfpar.size();i++){
			delete rfpar[i];
		}
		fbpar.clear(); expar.clear(); rfpar.clear(); //fred->clear();
		//delete fbsrc; delete exsrc;
      exit( EXIT_SUCCESS );
      break;
	case 'a': case 'A' :
		rapid(*player);
		break;
	case 's': case 'S' :
		if(fbtim<0) {spawnFireball();}
		break;
	case 'f': case 'F' :
		beatim = 5;
		break;
	case '0': hit_damage = 0;
		break;
	case '-': hit_damage = 10;
		break;

	case 't':
		printf("showing hps\n");
		for(vector<playerstate_t>::iterator it = others.begin();it != others.end();++it)
		{
			printf("hp: %d\n",(*it)._hp);
		}
		break;
	case 'm':
		printf("sizeof fbsrc: %d\n", sizeof(*fbsrc));
		printf("sizeof exsrc: %d\n", sizeof(*exsrc));
		break;
	case 'd': case 'D' :
		if (player->_mp>=25) {
			crate *temp = new crate(0, 10, OBJECTSTATE_CRATE, coord2d_t(player->_pos.x() + (-sin(player->_vel.x()) * dist),(player->_pos.y() + (cos(player->_vel.x()) * dist))), textures[OBJECTSTATE_CRATE]);
			
			crates.push_back(temp);
			double px = crates[crates.size()-1]->_pos.x();
			double pz = -(crates[crates.size()-1]->_pos.y());
			crates[crates.size()-1]->body = bbody(px-.5,pz-.5,px+.5,pz+.5,BB_AABB);
			player->_mp -= 25;
		}
		break;

  }
}

int checkPaCollision(source * src){
	for(unsigned int i = 0 ; i < crates.size(); i++){
		if(collide(src->body,crates[i]->body)){
			if (src->_type == PARTICLE_FIREBALL){
				damage(&crates[i]->_hp,10);
			}
			else if (src->_type == PARTICLE_RAPID){
				damage(&crates[i]->_hp,1);
			}
			else if (src->_type == PARTICLE_BEAM){
				damage(&crates[i]->_hp,1);
				//continue;
			}
			// this is wrong - hacked way of having the splinter effect on dead crate
			if (crates[i]->_hp == 0){
			}
			return HIT_CRATE;
		}
	}
	if (collide(src->body,player->body)&&!player->checkID(src->pid)){
		damage(&player->_hp,hit_damage);
		if(src->_type == PARTICLE_FIREBALL) detonate(fbsrc,false);
		return HIT_PLAYER;
	}
	for(unsigned int i=0;i<others.size();i++){
		if(collide(src->body,others[i].body)&&!others[i].checkID(src->pid)) {
			damage(&others[i]._hp,5);
			//if(src->_type == PARTICLE_BEAM) continue;
			return HIT_PLAYER;
		}
	}
	return 0;
}

bool checkPlCollision(playerstate_t * pls){
	for(unsigned int i=0; i < crates.size(); i++){
		if(collide(pls->front,crates[i]->body)) return true;
	}
	return false;
}

void tick(int state) {
	int coll = 0;
	if(checkPlCollision(player)) vel.y() = 0;
	player->change_velocity(vel);
	player->tick(worldtime);
	tickAi(worldtime);
	rfire = (rfire+1)%5;
	if (fbtim>-1){
		fbtim++;
		fbsrc->move();
		for(uint32_t i=0;i<fbpar.size();i++){
			fbpar[i]->move();
			if(fbpar[i]->life<0.0f){
				//fbpar[i] = new fireball_p(fbsrc);
				fbpar[i]->refresh();
			}
		}
		if(fbtim<15){
			fbsrc->_pos.x() = player->front.VCENX;
			fbsrc->_pos.z() = player->front.VCENZ;
			fbsrc->_vel.x() = -sin(player->_vel.x())/5.0;
			fbsrc->_vel.z() = -cos(player->_vel.x())/5.0;
		}
	}
	if(fbtim>50){
		fbtim=-1;
		fbsrc->active = false;
		for(uint32_t i=0;i<fbpar.size();i++) delete fbpar[i];
		fbpar.clear();
		detonate(fbsrc,false);
		delete fbsrc;
		explo = true;
	}
	else if(fbtim>-1&&(coll = checkPaCollision(fbsrc))){
		fbtim=-1;
		fbsrc->active = false;
		for(uint32_t i=0;i<fbpar.size();i++) delete fbpar[i];
		fbpar.clear();
		detonate(fbsrc,(coll==HIT_CRATE)); //if fbtim less than 50, fb must have collided with something
		delete fbsrc;
		explo = true;
	}
	if (explo){
		exsrc->move();
		for(int i=expar.size()-1;i>-1;i--){
			expar[i]->move();
			if(expar[i]->life<0.0f){
				delete expar[i];
				expar.erase(expar.begin()+i);
			}
		}
		if(expar.empty()){
			explo = false;
			delete exsrc;
		}
	}
	for(int i=rfpar.size()-1;i>-1;i--){
		rfpar[i]->move();
		if(!rfpar[i]->boom&&checkPaCollision(rfpar[i])){
			rfpar[i]->boom = true;
			rfpar[i]->life = 0.0;
		}
		if(!rfpar[i]->active){
			delete rfpar[i];
			rfpar.erase(rfpar.begin()+i);
		}
	}
	if(beatim>-1){
		beatim--;
		besrc->move();
		checkPaCollision(besrc);
	}
	for(vector<objectstate_t*>::iterator it = crates.begin();
		it != crates.end();
		it = (*it)->_hp == 0 ? crates.erase(it) : it + 1){
	}

   Animate(&fred->md5anim[0],&idlAnim,WORLD_TIME_RESOLUTION);
   Animate(&fred->md5anim[1],&walAnim,WORLD_TIME_RESOLUTION);
	glutPostRedisplay();

	worldtime+=WORLD_TIME_RESOLUTION;

	glutTimerFunc(WORLD_TIME_RESOLUTION, &tick, 0);
}

void initModel(){
   unsigned int rupTexture; 
  rupTexture = BindTextureBMP((char *)"textures/rupee2.bmp", false);
  textures.push_back(rupTexture);
   //fred = new mdmodel("rupee.md5mesh",NULL,rupTexture);
   fred = new mdmodel("model/hero.md5mesh","model/hero_idle.md5anim",rupTexture);
   initAnimInfo(&idlAnim,0);
   idlAnim.max_time = 1.0/fred->md5anim[0].frameRate;
   if(fred->loadAnim("model/hero_walk.md5anim")!=-1){
      initAnimInfo(&walAnim,1);
      walAnim.max_time = 1.0/fred->md5anim[1].frameRate;
   }
   //enemy = new mdmodel("model/characterModel.md5mesh",NULL,rupTexture);
   enemy = new mdmodel("model/rupee.md5mesh",NULL,rupTexture);
   initAnimInfo(&eneAnim,0);
   cerr << "hello?" << endl;
}

void mana(int pass) {
	if (player->_mp<=195) {
		player->_mp+=5;
	}
	else {
		player->_mp = 200;
	}
	glutTimerFunc(1000, mana,0);
}

int main( int argc, char** argv ) {
    
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //used to find memory leaks

	bool windo = true; //true - window; false - full screen. quicker than commenting/uncommenting
  //set up my window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  GW = 800;
  GH = 600;
  if(windo){
		glutInitWindowSize(800, 600); 
		glutInitWindowPosition(0, 0);
		glutCreateWindow("Island");
  }else{
	  glutGameModeString("800x600:32");
	  if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)){
		glutEnterGameMode();
	  }
	  else{
		  exit(1);
	  }
  }
  //glClearColor(0.0, 0.0, 0.0, 1.0);
  glClearColor(1.0, 1.0, 1.0, 1.0);

  myX = 0;
  myY = 0;
  myZ = 0;
  angle = 0;
  theta = 0;

  float shift = 1.0f;

  if(TOP_VIEW == 1) {
	  eyex=0;
      eyey=75;
      eyez=0;
  }
  else {
    eyex = 0;
    eyey = 5;//4.33;
    eyez = 3.5+shift;//5;
  }
  LAx = 0;
  LAy = 0;
  LAz = shift;

  player = new playerstate_t(worldtime);
  player->_hp = 100;
  player->_mp = 200;
  fbtim = -1;
  explo = false;

  srand(time(NULL));


  
  //register glut callback functions
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutMouseFunc(mouse);
  glutKeyboardFunc( keyboard );
  glutPassiveMotionFunc(processMousePassiveMotion);
  glutMotionFunc(processMouseActiveMotion);
  glutTimerFunc(WORLD_TIME_RESOLUTION,&tick,0);
  glutTimerFunc(1000, mana,0);
  glEnable(GL_DEPTH_TEST);

  init_lighting();
  init_ai();
  initModel();
  glEnable(GL_LIGHTING);

	// loading textures
	// clearing the vector
	// Prentice says a vector is overkill for holding textures, but I'll do it for now
	textures.clear();
	unsigned int crateTexture;

	//crateTexture = BindTextureBMP((char *)"crate.bmp"); //same file, different location -Seth
	crateTexture = BindTextureBMP((char *)"textures/crate.bmp", false); //0
	textures.push_back(crateTexture);
	unsigned int partTexture = init_particletex(); //1
	textures.push_back(partTexture);

  unsigned int tileTexture; 
  tileTexture = BindTextureBMP((char *)"textures/images.bmp", true); //2
  textures.push_back(tileTexture);

  unsigned int waterTexture;
  waterTexture = BindTextureBMP((char *)"textures/water.bmp", true); //3
  textures.push_back(waterTexture);

  unsigned int fireTexture;
  fireTexture = BindTextureBMP((char *)"textures/fireball.bmp", true); //4
  textures.push_back(fireTexture);

  unsigned int rapidTexture;
  rapidTexture = BindTextureBMP((char *)"textures/rapid1.bmp", true); //5
  textures.push_back(rapidTexture);

  unsigned int crate1Texture;
  crate1Texture = BindTextureBMP((char *)"textures/crate1.bmp", true); //6
  textures.push_back(crate1Texture);

  unsigned int fistTexture;
  fistTexture = BindTextureBMP((char *)"textures/fist.bmp", true); //7
  textures.push_back(fistTexture);

  unsigned int noxTexture;
  noxTexture = BindTextureBMP((char *)"textures/wood3.bmp", true); //8
  textures.push_back(noxTexture);

  unsigned int patternTexture;
  patternTexture = BindTextureBMP((char *)"textures/palm.bmp", true); //9
  textures.push_back(patternTexture);

  for(int i = 0; i < 10; i++){
	  crate *temp = new crate(0, 10, OBJECTSTATE_CRATE, coord2d_t(rand()%20-10,rand()%20-10), textures[OBJECTSTATE_CRATE]);
	  crates.push_back(temp);
  }
  for(int i=0;i<10;i++) {
	  double px = crates[i]->_pos.x();
	  double pz = -(crates[i]->_pos.y());
	  crates[i]->body = bbody(px-.5,pz-.5,px+.5,pz+.5,BB_AABB);
  }

	init_particle();
  
	besrc = new beam(player); //only need one beam right now, so might as well initialize it now
  glutMainLoop();
}
