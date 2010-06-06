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
#include <time.h>
#include "particle.h"
#include "collision.h"
#include "netutil.h"
#include "texture.h"
#include "types.h"
//#include "Bin.h" don't need anymore
#include "gameobjects.h"
#include "shader.h"
#include "GLSL_helper.h"
#include "enemy.h"


//#include "md5mesh.cpp"
//#include "md5anim.cpp"
#include "md5model.h"
#include "mdmodel.h"
#include "objloader.h"

#include "gamestate.h"

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

//#define HIT_CRATE 1
//#define HIT_PLAYER 2
#define MAP_SIZE 50 //(set as 50)Equivalent to 100x100 this is the number of sand tiles
#define TILE_HEIGHT .0001 //used for how high water tiles above 0 level .0001 for 0 top view, .01 for 1 top view
#define TOP_VIEW 0 //Set to 1 to see birds eye view of island

#define PLAYERID 0
// ENEMYID defined in enemy.h
#define CRATEID 200
#define HUTID 300
#define TREEID 400
#define ROCKID 500
#define ROCKID2 600

bool shadeOn; //are shaders on?

uint32_t worldtime=0;

int hit_damage = 10;

int cid = 0;
int hid = 0;
int tid = 0;
int rid = 0;
int rid2 = 0;
int eid = 0;

#define MIN(x,y) ((x>y)?y:x)
#define MAX(x,y) ((x>y)?x:y)

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
  {1.0, 1.0, 1.0, 1.0},
  {1.0, 1.0, 1.0, 1.0},
  {1.0, 1.0, 1.0, 1.0},
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
//coord2d_t vel;
//playerstate* gs->player;
vector<playerstate*> others;

struct obj_model_t *hutmdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));
struct obj_model_t *rockmdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));

struct obj_model_t oceanmdl;
struct obj_model_t *shellmdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));

struct obj_model_t *rock2mdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));

struct obj_model_t *treemdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));



gamestate* gs;

//sets up a specific material
void materials(materialStruct materials) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, materials.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materials.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, materials.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, materials.shininess);
}

//void rapid(playerstate&);
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

float eyex, eyey, eyez;
float LAx, LAy, LAz;
float theta;
float angle;
float myX, myY, myZ;
bool flag = false;

GLubyte * alpha;
float fps;



vector<unsigned int> textures;

vector<objectstate*> crates;



mdmodel* playerMod;
mdmodel* enemyMod;
struct anim_info_t idlAnim, walAnim, eneAnim;

unsigned int hutTex, rockTex, rock2Tex;

#if 0
// Variables Necessary For FogCoordfEXT
#define GL_FOG_COORDINATE_SOURCE_EXT	0x8450					// Value Taken From GLEXT.H
#define GL_FOG_COORDINATE_EXT		0x8451					// Value Taken From GLEXT.H

typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);		// Declare Function Prototype

PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;					// Our glFogCoordfEXT Function
GLfloat	fogColor[4] = {0.6f, 0.3f, 0.0f, 1.0f};					// Fog Colour 

#endif

float p2w_x(int x) {
  float x1;
  x1  = (x*(2/(float)gs->GW)) + (((1/(float)gs->GW) - 1));
  x1 = x1 * ((float)gs->GW/(float)gs->GH);

return x1;

}

float p2w_y(int y) {
  
  float y1;
  y1  = (y*(2/(float)gs->GH)) + (((1/(float)gs->GH) - 1));

return y1;
}

void damage(uint8_t *target, int dam){
	if(*target>dam) *target -= dam;
	else *target = 0;
}

bool cull(coord2d_t pos){
	//return false; 
	if((pos.x()>gs->player->_pos.x()+14.0)||(pos.x()<gs->player->_pos.x()-14.0)) return true;
	return ((pos.y()>gs->player->_pos.y()+14.0)||(pos.y()<gs->player->_pos.y()-14.0));
}

void drawCharacter();



void init_ai(){
	for(int i=0; i<20; ++i){
		playerstate* temp;

		//playerstate temp(0);
		if(i%2) temp = new meleeAI();//temp);
		else temp = new rangedAI();
		spwanEnemyHelperRandPos(temp,10,200,worldtime);
		others.push_back(temp);


	}
}

void drawAi(){
	for(unsigned int i=0; i<others.size(); i++)
	{
		if(cull(others[i]->_pos)) continue;
		materials(Blue);
		glPushMatrix();
		//translate
		//glTranslatef(others[i]->_pos.x(), 0.2, -(others[i]->_pos.y()));
		//rotate
		//glRotatef(others[i]->_vel.x() * (180.0 / M_PI), 0.0, 1.0, 0.0);
		//drawCharacter();
		glEnable(GL_LIGHTING);
		others[i]->draw();
		glDisable(GL_LIGHTING);
		glPopMatrix();
	}
}

#define MIN_AI_DISTANCE 7.0
#define AI_BOUNDS_MAX 40.0
#define AI_BOUNDS_MIN -40.0
void tickAi(uint32_t time){
	coord2d_t dummy;
	for(unsigned int i=0; i<others.size(); ) //i++ is at the end of for loop
	{
		
		if (others[i]->_hp <= 0){


			gs->updatBinLists(others[i],REMOV);
			delete others[i];
			others.erase(others.begin()+i);

			gs->player->_score++;
			continue;
		}
		if(others[i]->_id >200){
			printf("corruption found\n");
		}
		
		i++;
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

void init_dispList(){
	glNewList(TREELIST,GL_COMPILE);
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glScalef(1, 1, 1);
		RenderOBJModel (treemdl);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEndList();

	glNewList(HUTLIST,GL_COMPILE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, hutTex);

	glColor3f(1,1,1);
	glPushMatrix();
		glScalef(.04, .04, .04);
		RenderOBJModel (hutmdl);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEndList();

	glNewList(ROCKLIST,GL_COMPILE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, rockTex);

	glColor3f(1,1,1);
	glPushMatrix();
		glScalef(.009, .009, .009);
		RenderOBJModel (rockmdl);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEndList();

	glNewList(ROCK2LIST,GL_COMPILE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, rock2Tex);

	glColor3f(1,1,1);
	glPushMatrix();
		glScalef(.009, .009, .009);
		RenderOBJModel (rock2mdl);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
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


//void smiteEm(int x, int y){
//	if (gs->player->_mp<50) {
//		return;
//	}
//	gs->player->_mp -= 50;
//	//first calc target position
//	double rx = ((double)x-gs->GW/2.0)/2.0;
//	double lx = ((double)x-gs->GW/2.0-71.0)/2.0;
//	double by = (double)y-gs->GH/2.0;
//	double ty = (double)y-gs->GH/2.0-50.0;
//	//double bth = atan2(by,rx); //one unit near from gs->player
//	//double cth = atan2(ty,rx); //position of gs->player
//	//double lth = atan2(ty,lx); //one unit left from gs->player
//	double mb = by/rx;
//	double mc = ty/rx;
//	double ml = ty/lx;
//	coord2d_t targ;
//	if(rx==0.0&&ty==0.0) //this is where the gs->player is.
//		targ = gs->player->_pos;
//	else if(ml!=mc){
//		targ.x() = 1.0/(mb-mc) + gs->player->_pos.x();
//		targ.y() = -mc/(mb-mc) + gs->player->_pos.y() -1.0;
//	}
//	else{
//		targ.x() = ml/(mc-ml) + gs->player->_pos.x();
//		targ.y() = mc*ml/(mc-ml) + gs->player->_pos.y();
//	}
//
//	gs->smit = true;
//	smsrc = new smite_s(targ.x(),-targ.y(),gs->player->_id);
//	for(int i=0;i<400;i++){
//		gs->smpar.push_back(new smite_p(smsrc));
//	}
//}



void reshape(int w, int h) {
  gs->GW = w;
  gs->GH = h;


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

	enemyMod->draw(eneAnim);
	
	glPopMatrix();
}

void drawPlayer() {
	glEnable(GL_LIGHTING);
  materials(Grey);
  glPushMatrix();
  
    glTranslatef(0.0, 0.25, 0.0);
    glTranslatef(0.0, 0.0, -0.5);
    gluCylinder(gluNewQuadric(), .05, .2, 1, 12, 36);
  glPopMatrix();
  glDisable(GL_LIGHTING);
}

void drawAniPlayer(bool walk){
	glEnable(GL_LIGHTING);
   glPushMatrix();
   materials(ModeMat);
   playerMod->draw((walk)?walAnim:idlAnim);
   glPopMatrix();
   glDisable(GL_LIGHTING);
}

void drawFireball() {
	gs->fbsrc->draw();
	/*for(uint32_t i=0;i<gs->fbpar.size();i++){
		gs->fbpar[i]->draw();
	}*/
}

//void drawExplosion() {
//	//gs->exsrc->draw();
//	for(uint32_t i=0;i<gs->expar.size();i++){
//		gs->expar[i]->draw();
//	}
//}

void drawSmite() {
	gs->smsrc->draw();
	/*for(uint32_t i=0;i<gs->smpar.size();i++){
		gs->smpar[i]->draw();
	}*/
}

void drawRapid() {
	for(uint32_t i=0;i<gs->rfpar.size();i++){
		gs->rfpar[i]->draw();
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

	gluOrtho2D(0, gs->GW, 0, gs->GH);

	glScalef(1, -1, 1);

	glTranslatef(0, -gs->GH, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

#if 0
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
		glBindTexture(GL_TEXTURE_2D, ((goCrate*)crates[i])->_tid);
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
		//glBindTexture(GL_TEXTURE_2D, ((goCrate*)crates[i])->texture);
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
		//glBindTexture(GL_TEXTURE_2D, ((goCrate*)&crates[i])->texture);
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
		//glBindTexture(GL_TEXTURE_2D, ((goCrate*)&crates[i])->texture);
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
#endif


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
	/*glPushMatrix();
	  glEnable(GL_TEXTURE_2D);
	  //glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);

	  glDisable(GL_LIGHTING);
	  //glDisable(GL_DEPTH_TEST);
	
	  glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	  glBindTexture(GL_TEXTURE_2D, textures[11]);
	  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	   glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	  glBegin(GL_QUADS);
	  glTexCoord2f (0.0, 0.0);
	  glVertex2f(-40.0, -155.0);
	  glTexCoord2f (0.0, 1.0);
	  glVertex2f(-40.0, -88.0); 
	  glTexCoord2f (1.0, 1.0);
	  glVertex2f(30.0, -88.0); 
	  glTexCoord2f (1.0, 0.0);
	  glVertex2f(30.0, -155.0);
	  glEnd();
	  glDisable(GL_TEXTURE_2D);
	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	  //glDisable(GL_BLEND);	   
	  //glEnable(GL_DEPTH_TEST);
	   glEnable(GL_LIGHTING);
	  glPopMatrix();
	return;*/
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
	if(gs->player->_hp == 0) {
		sprintf(buff, "YOU DIED");
		renderBitmapString((gs->GW/2.0)-15, gs->GH/2.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
	}

	//materials(Black);
	glColor3f(1, 1, 1);
	/*sprintf(buff, "Rapid");
	renderBitmapString((gs->GW/3.0)-15, gs->GH-112,GLUT_BITMAP_HELVETICA_12,buff);
	sprintf(buff, "Fire");
	renderBitmapString((gs->GW/3.0)-10, gs->GH-100,GLUT_BITMAP_HELVETICA_12,buff);*/
	sprintf(buff, "(a)");
	renderBitmapString(50, gs->GH-240,GLUT_BITMAP_HELVETICA_12,buff);
	/*sprintf(buff, "FireBall");
	renderBitmapString((gs->GW/3.0)+30, gs->GH-110,GLUT_BITMAP_HELVETICA_12,buff);*/
	sprintf(buff, "(s)");
	renderBitmapString(50, gs->GH-300,GLUT_BITMAP_HELVETICA_12,buff);
	/*sprintf(buff, "Crate");
	renderBitmapString((gs->GW/3.0)+80, gs->GH-110,GLUT_BITMAP_HELVETICA_12,buff);*/
	sprintf(buff, "(d)");
	renderBitmapString(50, gs->GH-360,GLUT_BITMAP_HELVETICA_12,buff);
	sprintf(buff, "(f)");
	renderBitmapString(50, gs->GH-420,GLUT_BITMAP_HELVETICA_12,buff);
	sprintf(buff, "(g)");
	renderBitmapString(50, gs->GH-480,GLUT_BITMAP_HELVETICA_12,buff);
	/*sprintf(buff, "Melee");
	renderBitmapString((gs->GW/5.0)-30, gs->GH-65,GLUT_BITMAP_TIMES_ROMAN_24 ,buff);*/
	glColor3f(0, 0, 0);
	sprintf(buff, "(Left Click)");
	renderBitmapString(10, gs->GH-110,GLUT_BITMAP_HELVETICA_12,buff);


    /*glColor3f(0.7, 0.7, 0.7);    //maybe used for later
	glPushMatrix();
	glTranslatef(gs->GW/5.0, gs->GH-150, 0);
	draw_circle2();
	glPopMatrix();*/

	//materials(Gray);
	glColor3f(0.7, 0.7, 0.7); //fist
	glPushMatrix();
	glTranslatef(40, gs->GH-40, 0);
	draw_circle2();
	draw_circle();
	glPopMatrix();

	/*glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(gs->GW/5.0, gs->GH-150, 0);
	draw_circle2();
	glPopMatrix();*/


	//materials(Black);

    //glLoadIdentity();

	glColor3f(0.7, 0.7, 0.7);


	glPushMatrix();  //UI bar that covers icons
	glTranslatef(40, gs->GH-360, -.01);
	drawUIBar(8);
	glPopMatrix();

	glPushMatrix();  //UI bar that Health bars
	glTranslatef(gs->GW-60, gs->GH-95, -.01);
	drawUIHBar(9);
	glPopMatrix();

	glColor3f(.5, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(gs->GW-80, gs->GH + 100 + (200*(-gs->player->_hp/100.0)), 0); //red health bar
	drawBar();
	glPopMatrix();


	glColor3f(0.0, 0.0, .5);
	glPushMatrix();
	glTranslatef(gs->GW-40, gs->GH + 100 + (200*(-gs->player->_mp/200.0)), 0); // blue mana bar
	drawBar();
	glPopMatrix();

	glColor3f(0, 0, 0);
	glPushMatrix();
	glTranslatef(gs->GW-40, gs->GH-100, 0);  //gray behind red health bar
	drawBar();
	glPopMatrix();


	glPushMatrix();
	glTranslatef(gs->GW-80, gs->GH-100, 0);  //gray behind blue mana bar
	drawBar();
	glPopMatrix();


	glPushMatrix();  //rapid fire (a)
	glTranslatef(25, gs->GH-240, 0);
	drawBox(5);
	glPopMatrix();

    glPushMatrix();  //fireball (s)
	glTranslatef(25, gs->GH-300, 0);
	drawBox(4);
	glPopMatrix();

	glPushMatrix();  //crate (d)
	glTranslatef(25, gs->GH-360, 0);
	drawBox(6);
	glPopMatrix();

	glPushMatrix(); //(f)
	glTranslatef(25, gs->GH-420, 0);
	drawBox(0);
	glPopMatrix();

	glPushMatrix(); //(g) 
	glTranslatef(25, gs->GH-480, 0);
	drawBox(10);
	glPopMatrix();


	//materials(Black);
	glColor3f(0.0, 0.0, 0.0);
	sprintf(buff, "Health: %d", gs->player->_hp);
    renderBitmapString(2*gs->GW/10.0,gs->GH/11.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
	sprintf(buff, "Kills: %d", gs->player->_score);
	renderBitmapString(5*gs->GW/10.0,gs->GH/11.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
    sprintf(buff, "FPS: %f", fps);
	renderBitmapString(7*gs->GW/10.0,gs->GH/11.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
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

void drawWaterTile(){
	glBegin(GL_QUADS);
	glTexCoord2f(0.0,1.0);
	glNormal3f(0.0,1.0,0.0);
	glVertex3f(-1.0, -0.3, 1.0);
	glTexCoord2f(0.0,0.0);
	glNormal3f(0.0,1.0,0.0);
	glVertex3f(-1.0, -0.3, -1.0);
	glTexCoord2f(1.0,0.0);
	glNormal3f(0.0,1.0,0.0);
	glVertex3f(1.0, -0.3, -1.0);
	glTexCoord2f(1.0,1.0);
	glNormal3f(0.0,1.0,0.0);
	glVertex3f(1.0, -0.3, 1.0);
	glEnd();
}

void drawWater() {
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, textures[3]);

  for(int i=MAP_SIZE; i<MAP_SIZE+20; i+=2){
	  for(int j=-MAP_SIZE; j<MAP_SIZE; j+=2){
	glPushMatrix();
	glTranslatef(i,0,j);
	drawWaterTile();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-i,0,-j);
	drawWaterTile();
	glPopMatrix();

		glPushMatrix();
	glTranslatef(j,0,i);
	drawWaterTile();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-j,0,-i);
	drawWaterTile();
	glPopMatrix();
	  }
  }

#if 0
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
#endif
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



#if 0
//vfog ext
int Extension_Init()
{
	char Extension_Name[] = "EXT_fog_coord";

	// Allocate Memory For Our Extension String
	char* glextstring=(char *)malloc(strlen((char *)glGetString(GL_EXTENSIONS))+1);
	strcpy (glextstring,(char *)glGetString(GL_EXTENSIONS));		// Grab The Extension List, Store In glextstring

	if (!strstr(glextstring,Extension_Name))				// Check To See If The Extension Is Supported
		return FALSE;							// If Not, Return FALSE

	free(glextstring);	
	// Free Allocated Memory
	// Setup And Enable glFogCoordEXT
	glFogCoordfEXT = (PFNGLFOGCOORDFEXTPROC) wglGetProcAddress("glFogCoordfEXT");

	return true;
}
#endif

void gsDisplay(){
	if (!gs){
		//TODO: uninit

		cerr << "DEBUG: No init" << endl;
	}
	else{

		if(gs->_state == GSSTATE_ACTIVE){
			gs->draw();
		}
		else{
			//TODO: not active

			cerr << "DEBUG: Not Active" << endl;

		}
	}
}


void drawTree(double x, double y, double z) {

glDisable(GL_LIGHTING);
glEnable(GL_TEXTURE_2D);
glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
//glBindTexture(GL_TEXTURE_2D, textures[13]);

glColor3f(1,1,1);
glPushMatrix();
    glTranslatef(x, y, z);
	//glScalef(.009, .009, .009); rocks
	glScalef(.5, .5, .5);
	//RenderOBJModel (plantemdl);
    //RenderOBJModelt (plantemdl, textures[13], textures[14]);
glPopMatrix();
glDisable(GL_TEXTURE_2D);
glEnable(GL_LIGHTING);

}




void display() {
  static int frame=0;
  static int lasttime=0;
  
  int time = glutGet(GLUT_ELAPSED_TIME);
  
  ++frame;

  if (time - lasttime > 1000){
    fps = frame * 1000.0/((float)(time-lasttime));
    lasttime = time;
    frame = 0;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glMatrixMode(GL_MODELVIEW);
  

	glPushMatrix(); // push -1
  
  setOrthoProjection();
  glPushMatrix(); //push 0


  //glLoadIdentity(); not needed
printOpenGLError();
  

	glPushMatrix(); // push 1
    displayHud();
	
  //glPopMatrix();
  //resetPerspectiveProjection();

	  //set up the camera

		gluLookAt(eyex + gs->player->_pos.x(), eyey, eyez - gs->player->_pos.y(), LAx + gs->player->_pos.x(), LAy, LAz - gs->player->_pos.y(), 0, 0, -1);
	    
		glPushMatrix(); // Tiles
			drawTiles();
		glPushMatrix(); // End Tiles

		glPushMatrix(); // Ocean
			if(shadeOn){
				glUseProgram(ShadeProg);
				printOpenGLError();
				glUniform1f(getUniLoc(ShadeProg, "wtime"), ((float)worldtime)*0.03);
				printOpenGLError();
				//glUniform1f(getUniLoc(ShadeProg, "wHeight"), 0.5);
				//glUniform1f(getUniLoc(ShadeProg, "wTilt"), 0.0);
			}
			glScalef(8.0,1.0,8.0);
			glTranslatef(0.0,0.0,-7.2);
			//glEnable(GL_LIGHTING);
			glDisable(GL_LIGHTING);

			//materials(Blue);
			printOpenGLError();
			glColor3f(0.0,0.0,1.0);
			RenderOBJModel(&oceanmdl);
			printOpenGLError();
			//drawWater();
			materials(White);
			if(shadeOn) glUseProgram(0);
			glDisable(GL_LIGHTING);
		glPopMatrix(); // end Ocean

		glPushMatrix(); // GS display (all game objects should be drawn here)
			gsDisplay();	

			printOpenGLError();
		glPopMatrix();	// end GS
	glPopMatrix(); // pop 1
    printOpenGLError();

	  glPushMatrix(); // ai
	  drawAi();

	  glPopMatrix(); // end ai

	  glPushMatrix(); // tree
		glTranslatef(0.0, 0.01, 0.0);
			//materials(Sand);
		//drawGrid();
			//glTranslatef(-1.0,0,-1.0);
			drawTree(1, 0, -1);
			//drawCrates();

			printOpenGLError();
			//glutSolidSphere(1.0,10,10);
			if(gs->beatim>-1) gs->besrc->draw();
	  glPopMatrix(); // end tree


	  if(gs->smit) drawSmite();
	  drawRapid();
    glPopMatrix(); //pop 0
  glPopMatrix(); //pop -1
  
  glutSwapBuffers();
    printOpenGLError();
}




void mouse(int button, int state, int x, int y) {
	gs->bflag = false;
  if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) { 
		flag = true;
		gs->player->_vel.y() = 0.005;
    }
	else {
		flag = false;
		gs->player->_vel.y() = 0.000;
	}
  }
	if(button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			gs->bflag = true;
			gs->beatim = 5;
		}
		else gs->bflag = false;
	}
}

void processMousePassiveMotion(int x, int y) {


	//float theta = 0;
	x -= gs->GW/2;
	x *= .5;
	y -= gs->GH/2 - 50;
	
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
	gs->player->_vel.x() = theta;
	
	
  glutPostRedisplay();

}

void processMouseActiveMotion(int x, int y) {
	
	//float theta = 0;
	x -= gs->GW/2;
	x *= .5;
	y -= gs->GH/2 - 50;
	
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

	gs->player->_vel.x() = theta;


	gs->player->_vel.y() = (flag)?.005:0;
	if(gs->bflag) gs->beatim = 5;

  glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y ){
  float dist = 3.0;
  switch( key ) {
    case 'q': case 'Q' :
		for(uint32_t i=0;i<gs->rfpar.size();i++){
			delete gs->rfpar[i];
		}
		for(uint32_t i=0;i<gs->_pars.size();i++){
			delete gs->_pars[i];
		}

		others.clear();
		delete playerMod;
		delete enemyMod;
		gs->_pars.clear(); gs->rfpar.clear();
		//delete fbsrc; delete exsrc;
      exit( EXIT_SUCCESS );
      break;
	case 'a': case 'A' :
		gs->rapid(*gs->player);
		break;
	case 'w': case 'W' :
		gs->spread(*gs->player);
		break;
	case 's': case 'S' :
		if(gs->fbtim<0) {gs->spawnFireball();}
		break;
	case 'f': case 'F' :
		gs->beatim = 5;
		break;
	case 'g': case 'G' :
		if(!gs->smit) gs->smiteEm(x,y);
		break;
	case 'c': gs->player->_mp = 200;
		break;
	case '0': hit_damage = 0;
		break;
	case '-': hit_damage = 10;
		break;
	case 'r': printf("coordinates: %d, %d\n",x,y); break;

	case 't':
		printf("showing hps\n");
		for(unsigned int i=0;i<others.size();i++)
		{
			printf("hp: %d\n",others[i]->_hp);
		}
		break;
	case 'm':
		//printf("sizeof fbsrc: %d\n", sizeof(*fbsrc));
		//printf("sizeof exsrc: %d\n", sizeof(*exsrc));
		break;
	case 'd': case 'D' :

		//TODO: refactor with new gamestate code

		if (gs->player->_mp>=25) {
			//goCrate *temp = new goCrate(0, 10, OBJECTSTATE_CRATE, coord2d_t(gs->player->_pos.x() + (-sin(gs->player->_vel.x()) * dist),(gs->player->_pos.y() + (cos(gs->player->_vel.x()) * dist))), textures[OBJECTSTATE_CRATE]);
			goCrate* crt = new goCrate(textures[OBJECTSTATE_CRATE]);
			
			crt->_pos.x() = gs->player->_pos.x() + (-sin(gs->player->_vel.x()) * dist);
			crt->_pos.y() = gs->player->_pos.y() + (cos(gs->player->_vel.x()) * dist);
			double px = crt->_pos.x();
			double pz = -(crt->_pos.y());
			crt->body = bbody(px-.5,pz-.5,px+.5,pz+.5,BB_AABB);

			crt->_id = CRATEID + (cid++);
			crt->_hp = 10;

			gs->addObject(crt);

			gs->player->_mp -= 25;

		}
		break;

  }
}

void tick(int state) {
	gs->tick(worldtime);
			//gs->player constraints
		if (gs->player->_pos.x()>MAP_SIZE) {
			gs->player->_pos.x() = MAP_SIZE;
		}
		if (gs->player->_pos.x()<-MAP_SIZE) {
			gs->player->_pos.x() = -MAP_SIZE;
		}
		if (gs->player->_pos.y()>MAP_SIZE) {
			gs->player->_pos.y() = MAP_SIZE;
		}
		if (gs->player->_pos.y()<-MAP_SIZE) {
			gs->player->_pos.y() = -MAP_SIZE;
		}
	int coll = 0;
	//if(gs->SmaPlCollision(gs->player)) vel.y() = 0;
	//gs->player->change_velocity(vel);
	//gs->player->_vel = vel;
	gs->player->tick(worldtime);
	gs->updatBinLists(gs->player,UPDAT);
	//tickAi(worldtime);
	for(unsigned int i=0;i<others.size();i++){
		if(others[i]->_hp==0){
			gs->updatBinLists(others[i],REMOV);
			delete others[i];
			others.erase(others.begin()+i);
			i--;
			gs->player->_score++;
			continue;
		}
		others[i]->tick(worldtime);
	}
	gs->rfire = (gs->rfire+1)%5;
	if (gs->fbtim>-1){
		gs->fbtim++;
		gs->fbsrc->move();
		//for(uint32_t i=0;i<gs->fbpar.size();i++){
		//	gs->fbpar[i]->move();
		//	if(gs->fbpar[i]->life<0.0f){
		//		//gs->fbpar[i] = new fireball_p(fbsrc);
		//		gs->fbpar[i]->refresh();
		//	}
		//}
		if(gs->fbtim<15){
			gs->fbsrc->_pos.x() = gs->player->front.VCENX;
			gs->fbsrc->_pos.z() = gs->player->front.VCENZ;
			gs->fbsrc->_vel.x() = -sin(gs->player->_vel.x())/5.0;
			gs->fbsrc->_vel.z() = -cos(gs->player->_vel.x())/5.0;
		}
	}
	if(gs->fbtim>50){
		gs->fbtim=-1;
		gs->fbsrc->active = false;
		//for(uint32_t i=0;i<gs->fbpar.size();i++) delete gs->fbpar[i];
		//gs->fbpar.clear();
		gs->detonate(gs->fbsrc,false);
		delete gs->fbsrc;
		gs->explo = true;
	}

	else if(gs->fbtim>-1&&(coll = gs->SmaPaCollision(gs->fbsrc))){
		gs->fbtim=-1;
		gs->fbsrc->active = false;
		//for(uint32_t i=0;i<gs->fbpar.size();i++) delete gs->fbpar[i];
		//gs->fbpar.clear();
		gs->detonate(gs->fbsrc,(coll==HIT_CRATE)); //if gs->fbtim less than 50, fb must have collided with something
		delete gs->fbsrc;
		gs->explo = true;
	}
	if (gs->explo){
		gs->exsrc->move();
		/*for(int i=gs->expar.size()-1;i>-1;i--){
			gs->expar[i]->move();
			if(gs->expar[i]->life<0.0f){
				delete gs->expar[i];
				gs->expar.erase(gs->expar.begin()+i);
			}
		}*/

		gs->LarPaCollision(gs->exsrc,0,100,0,100);

		//if(gs->expar.empty()){
		if(gs->exsrc->subPar<1){
			gs->explo = false;
			delete gs->exsrc;
		}
	}
	if (gs->smit){
		gs->smsrc->move();
		/*for(int i=gs->smpar.size()-1;i>-1;i--){
			gs->smpar[i]->move();
			if(gs->smpar[i]->life<0.0f){
				delete gs->smpar[i];
				gs->smpar.erase(gs->smpar.begin()+i);
			}
		}*/

		gs->LarPaCollision(gs->smsrc,0,100,0,100);

		//if(gs->smpar.empty()){
		if(gs->smsrc->subPar<1){
			gs->smit = false;
			delete gs->smsrc;
		}
	}
	for(int i=gs->rfpar.size()-1;i>-1;i--){
		gs->rfpar[i]->move();

		if(!gs->rfpar[i]->boom&&(coll = gs->SmaPaCollision(gs->rfpar[i]))){

			gs->rfpar[i]->boom = true;
			gs->rfpar[i]->life = 0.0;
			if(coll==HIT_CRATE||coll==HIT_HUT||coll==HIT_TREE){
			for(int j=0;j<10;j++)
				gs->_pars.push_back(new splinter(gs->rfpar[i]->_pos));
			}
			else if(coll==HIT_PLAYER){
			for(int j=0;j<10;j++)
				gs->_pars.push_back(new blood(gs->rfpar[i]->_pos));
			}
		}
		if(!gs->rfpar[i]->active){
			delete gs->rfpar[i];
			gs->rfpar.erase(gs->rfpar.begin()+i);
		}
	}
	if(gs->beatim>-1){
		gs->beatim--;
		gs->besrc->move();


		gs->LarPaCollision(gs->besrc,0,100,0,100);

	}
	/*for(vector<objectstate*>::iterator it = crates.begin();
		it != crates.end();
		it = (*it)->_hp == 0 ? crates.erase(it) : it + 1){*/
	/*
	for(unsigned int i=0; i<crates.size();){

		if(crates[i]->_hp == 0) {
			gs->updatBinLists(crates[i],REMOV);
			delete crates[i];
			crates.erase(crates.begin()+i);
		}
		else i++;

	}
	*/

   Animate(&playerMod->md5anim[0],&idlAnim,WORLD_TIME_RESOLUTION);
   Animate(&playerMod->md5anim[1],&walAnim,WORLD_TIME_RESOLUTION);
	glutPostRedisplay();

	worldtime+=WORLD_TIME_RESOLUTION;

	int eger = worldtime/WORLD_TIME_RESOLUTION;
	for(int i=0;i<100;i++){
		gs->janitor(eger%100,i);
		//janitor(eger%50+50,i);
	}

	glutTimerFunc(WORLD_TIME_RESOLUTION, &tick, 0);
}

void initModel(){
   unsigned int rupTexture; 
  rupTexture = BindTextureBMP((char *)"textures/rupee2.bmp", false);
  textures.push_back(rupTexture);
   //playerMod = new mdmodel("rupee.md5mesh",NULL,rupTexture);
   playerMod = new mdmodel("model/hero.md5mesh","model/hero_idle.md5anim",rupTexture);
   enemyMod = new mdmodel("model/characterModel.md5mesh","model/characterModelIdle.md5anim",rupTexture);
   initAnimInfo(&gs->player->idlAni,0);
   gs->player->idlAni.max_time = 1.0/playerMod->md5anim[0].frameRate;
   if(playerMod->loadAnim("model/hero_walk.md5anim")!=-1){
      initAnimInfo(&gs->player->walAni,1);
      gs->player->walAni.max_time = 1.0/playerMod->md5anim[1].frameRate;
   }
   if(playerMod->loadAnim("model/hero_attack_sword.md5anim")!=-1){
      initAnimInfo(&gs->player->attAni,2);
      gs->player->attAni.max_time = 1.0/playerMod->md5anim[2].frameRate;
   }
   //enemyMod = new mdmodel("model/rupee.md5mesh",NULL,rupTexture);
   //initAnimInfo(&eneAnim,0);
   double imt = 1.0/enemyMod->md5anim[0].frameRate;
   double wmt = 0;
   if(enemyMod->loadAnim("model/characterModelRunning2.md5anim")!=-1){
      wmt = 1.0/enemyMod->md5anim[1].frameRate;
   }
   for(unsigned int i=0;i<others.size();i++){
		initAnimInfo(&others[i]->idlAni,0);
		others[i]->idlAni.max_time = imt;
		if(wmt!=0){
			initAnimInfo(&others[i]->walAni,1);
			others[i]->walAni.max_time = wmt;
		}
   }
}

void mana(int pass) {
	if (gs->player->_mp<=195) {
		gs->player->_mp+=5;
	}
	else {
		gs->player->_mp = 200;
	}
	glutTimerFunc(1000, mana,0);
}





void fnExit1(){
	system("pause");
}

int main( int argc, char** argv ) {
	srand (time (NULL));


	gs = new gamestate();
	// use this for debugging unexpected exits: 
	//atexit (fnExit1);


	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //used to find memory leaks


	bool windo = true; //true - window; false - full screen. quicker than commenting/uncommenting
	//set up my window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gs->GW = 800;
	gs->GH = 600;
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
	eyez = 3.5;//+shift;//5;

	}
	LAx = 0;
	LAy = 0;
	LAz = 0;//shift;


	gs->player = new playerstate(worldtime);
	gs->player->_hp = 100;
	gs->player->_mp = 200;
	gs->player->_pos.x() = 0;
	gs->player->_pos.y() = 35;
	gs->fbtim = -1;
	gs->explo = false;
	gs->smit = false;

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

#if 0
	// Set Up Fog 
	glEnable(GL_FOG);							// Enable Fog
	glFogi(GL_FOG_MODE, GL_LINEAR);						// Fog Fade Is Linear
	glFogfv(GL_FOG_COLOR, fogColor);					// Set The Fog Color
	glFogf(GL_FOG_START,  0.0f);						// Set The Fog Start (Least Dense)
	glFogf(GL_FOG_END,    1.0f);						// Set The Fog End (Most Dense)
	glHint(GL_FOG_HINT, GL_NICEST);						// Per-Pixel Fog Calculation
	glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);		// Set Fog Based On Vertice Coordinates
#endif 


	cerr << "INFO: init lighting.. " << endl;

  init_lighting();

  cerr << "INFO: init ai.. " << endl;

  init_ai();

  cerr << "INFO: init model.. " << endl;

  initModel();


cerr << "INFO: init gamestate.. " << endl;

  


  gs->start(0);
  

  //glEnable(GL_LIGHTING);

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

  unsigned int blastTexture;
  blastTexture = BindTextureBMP((char *)"textures/smite.bmp", true); //10
  textures.push_back(blastTexture);
 
  hutTex = BindTextureBMP((char *)"textures/hut.bmp", true); //11
  textures.push_back(hutTex);


  rockTex = BindTextureBMP((char *)"textures/rock.bmp", true); //12
  textures.push_back(rockTex);

  rock2Tex = BindTextureBMP((char *)"textures/rock2.bmp", true); //13
  textures.push_back(rock2Tex);

  /*unsigned int bgaTexture;
  bgaTexture = BindTextureBMP((char *)"textures/bg_attack.bmp", false); //10
  textures.push_back(bgaTexture);*/


  /*unsigned int shellTexture;
  shellTexture = BindTextureBMP((char *)"textures/shell.bmp", true); //13
  textures.push_back(shellTexture);*/

  /*unsigned int shellTexture2;
  shellTexture2 = BindTextureBMP((char *)"textures/shell2.bmp", true); //14
  textures.push_back(shellTexture2);*/


  for(int i = 0; i < 20; i++){  

	  /* uses gamestate object crate:
	  goCrate *temp = new goCrate(textures[OBJECTSTATE_CRATE]);
	  temp->_hp = 10;
	  temp->_pos = coord2d_t(rand()%20-10,rand()%20-10);
      gs->_objects.push_back(temp);
	  */
	  //goCrate *temp = new goCrate(CRATEID+i, 10, OBJECTSTATE_CRATE, coord2d_t(rand()%20-10,rand()%20-10), textures[OBJECTSTATE_CRATE]);

	    if (i > 10) {
		goCrate *crt = new goCrate(textures[OBJECTSTATE_CRATE]);
		crt->_pos.x() = rand()%30-15;
		crt->_pos.y() = 35 + rand()%30-15;
		crt->body = bbody(crt->_pos.x()-.5,-crt->_pos.y()-.5,crt->_pos.x()+.5,-crt->_pos.y()+.5,BB_AABB);
		crt->_hp = 10;
		crt->_id = CRATEID + (cid++);
		gs->addObject(crt);
	    }

		if (i > 10) {
		Hut *hut = new Hut(textures[OBJECTSTATE_HUT], hutmdl);
		hut->_pos.x() = rand()%30-15;
		hut->_pos.y() = 35 +rand()%30-15;
		hut->body = bbody(hut->_pos.x()-1,-hut->_pos.y()-1,hut->_pos.x()+1,-hut->_pos.y()+1,BB_AABB);
		hut->_hp = 100;
		hut->_id = HUTID + (hid++);
		gs->addObject(hut);
		}


		palmTree *tree = new palmTree(textures[0], treemdl);
		tree->_pos.x() = rand()%30-15;
		tree->_pos.y() = 35 +rand()%30-15;
		tree->body = bbody(tree->_pos.x()-.2,-tree->_pos.y()-.2,tree->_pos.x()+.2,-tree->_pos.y()+.2,BB_AABB);
		tree->_hp = 10;
		tree->_id = TREEID + (tid++);
		gs->addObject(tree);

		if (i > 10) {
		rock *rck = new rock(textures[OBJECTSTATE_ROCK], rand()%90, rockmdl);
		rck->_pos.x() = rand()%30-15;
		rck->_pos.y() = 35 +rand()%30-15;
		rck->body = bbody(rck->_pos.x()-.2,-rck->_pos.y()-.2,rck->_pos.x()+.2,-rck->_pos.y()+.2,BB_AABB);
		rck->_hp = 10;
		rck->_id = ROCKID + (rid++);
		gs->addObject(rck);
		}

		if (i > 10) {
		rock2 *rck2 = new rock2(textures[OBJECTSTATE_ROCK2], rand()%90, rock2mdl);
		rck2->_pos.x() = rand()%30-15;
		rck2->_pos.y() = 35 +rand()%30-15;
		rck2->body = bbody(rck2->_pos.x()-.2,-rck2->_pos.y()-.2,rck2->_pos.x()+.2,-rck2->_pos.y()+.2,BB_AABB);
		rck2->_hp = 10;
		rck2->_id = ROCKID + (rid2++);
		gs->addObject(rck2);
		}



		
		
		
		


  }
  //for(int i=0;i<10;i++) {

	 // /* uses gamestate object crate:
	 // // TODO: This code does NOT belong here.
	 // double px = gs->_objects[i]->_pos.x();
	 // double pz = -(gs->_objects[i]->_pos.y());

	 // //gs->_objects[i]->_id = CRATEID + (cid++);
	 // gs->_objects[i]->body = bbody(px-.5,pz-.5,px+.5,pz+.5,BB_AABB);
	 // */


	 // double px = crates[i]->_pos.x();
	 // double pz = -(crates[i]->_pos.y());


	 // crates[i]->_id = CRATEID + (cid++);

	 // crates[i]->body = bbody(px-.5,pz-.5,px+.5,pz+.5,BB_AABB);
  //}


  init("model/palmTree.obj", treemdl);
  init("model/afro hut.obj", hutmdl);
  init("model/rock_a.obj", rockmdl);
  init("model/planeMesh.obj", &oceanmdl);
  init("model/rock_b.obj", rock2mdl);

  init_dispList();


  //init("model/conch.obj", plantemdl);
  //init("model/hut.obj", mdl);

	init_particle();

//  atexit (cleanup(test));


  
	gs->besrc = new beam(gs->player); //only need one beam right now, so might as well initialize it now

	cerr << "INFO: Init glew... ";
	if(GLEW_OK!=glewInit()){
		cerr << "FAILED" << endl;
		exit(-8);
	}
	else
		cerr << "OK" << endl;


	shadeOn = !(glCreateShader==NULL); //if computer can use shaders, glCreateShader won't be null
	if(shadeOn){
		//shader
		cerr << "INFO: Loading Shaders... ";

		if(!InstallShader(textFileRead("shaders\\VWave.glsl"),textFileRead("shaders\\FLight.glsl"))){
			cerr << "FAILED" << endl;
			exit(-9);
		}
		else
			cerr << "OK" << endl;
	}
	else
		cerr << "Shaders don't work on this computer. Going without." << endl;

  glutMainLoop();
}
