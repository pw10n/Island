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
#include "attack.h"
#include <SDL_mixer.h>
#include <SDL.h>





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

unsigned int menutex0, menutex1;
int menuchoice=0;
int mainmenu=0;
double position[5] = {-1.4, -.7, 0, .7, 1.4};
int indexp=0;
int indexa=0;
int attack[5];

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
#define VEGID 600

bool shadeOn; //are shaders on?

uint32_t worldtime=0;

int hit_damage = 10;

int cid = 0;
int hid = 0;
int tid = 0;
int rid = 0;
int rid2 = 0;
int eid = 0;
int vid = 0;

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


void init_ai();
void init_lighting();
void initModel();

//coord2d_t vel;
//playerstate* gs->player;
vector<playerstate*> others;

//Models
struct obj_model_t *hutmdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));
struct obj_model_t *rockmdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));
struct obj_model_t oceanmdl;
struct obj_model_t *shellmdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));
struct obj_model_t *rock2mdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));
struct obj_model_t *treemdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));
struct obj_model_t *vegmdl = (struct obj_model_t*) malloc(sizeof(obj_model_t));

//Materials
struct mtl_file *treemtl = (struct mtl_file*) malloc(sizeof(mtl_file));
struct mtl_file *vegmtl = (struct mtl_file*) malloc(sizeof(mtl_file));






float *depth_up = 0, *depth_down = 0, *depth_left = 0, *depth_right = 0;

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
GLfloat light_pos[4] = {1.0, 20.0, 1.5, 1.0}; //1.0,5.0,1.5,1.0
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



unsigned int partTex, crateTex, tileTex, waterTex, woodTex, palmTex, hutTex, rockTex, rock2Tex, woodTex2, gameover;


mdmodel* playerMod;
mdmodel* enemyMod;
struct anim_info_t idlAnim, walAnim, eneAnim;


#if 0
// Variables Necessary For FogCoordfEXT
#define GL_FOG_COORDINATE_SOURCE_EXT	0x8450					// Value Taken From GLEXT.H
#define GL_FOG_COORDINATE_EXT		0x8451					// Value Taken From GLEXT.H

typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);		// Declare Function Prototype

PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;					// Our glFogCoordfEXT Function
GLfloat	fogColor[4] = {0.6f, 0.3f, 0.0f, 1.0f};					// Fog Colour 

#endif


void get_gl_size(int &width, int &height){
	int iv[4];
	glGetIntegerv(GL_VIEWPORT, iv);
	width = iv[2];
	height = iv[3];

}

void damage(uint8_t *target, int dam){
	if(*target>dam) *target -= dam;
	else *target = 0;
}

bool cull(coord2d_t pos){
	//return false; 
	if((pos.x()>gs->player->_pos.x()+14.0)||(pos.x()<gs->player->_pos.x()-14.0)) return true;
	return ((pos.y()>gs->player->_pos.y()+16.0)||(pos.y()<gs->player->_pos.y()-8.0));
}

void drawCharacter();

void init_ai(){
	for(int i=0; i<50; ++i){
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
		materials(White);
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
	glBindTexture(GL_TEXTURE_2D, partTex);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0.1f,0.1f,0);
	glTexCoord2d(0,1); glVertex3f(-0.1f,0.1f,0);
	glTexCoord2d(1,0); glVertex3f(0.1f,-0.1f,0);
	glTexCoord2d(0,0); glVertex3f(-0.1f,-0.1f,0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, partTex);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0.1f,0,0.1f);
	glTexCoord2d(0,1); glVertex3f(-0.1f,0,0.1f);
	glTexCoord2d(1,0); glVertex3f(0.1f,0,-0.1f);
	glTexCoord2d(0,0); glVertex3f(-0.1f,0,-0.1f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, partTex);
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
	//glDisable(GL_LIGHTING);
	glPushMatrix();
		glScalef(1, 1, 1);
		RenderOBJModelt (treemdl, treemtl);
	glPopMatrix();
	//glEnable(GL_LIGHTING);
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

	glNewList(VEGLIST,GL_COMPILE);
	//glDisable(GL_LIGHTING);
	glPushMatrix();
		glTranslatef(0,-.07,0);
		glScalef(.5, .5, .5);
		RenderOBJModelt (vegmdl, vegmtl);
	glPopMatrix();
	//glEnable(GL_LIGHTING);
	glEndList();


}
///////////////// DEPTH BUFFER ////////////////////////////




void save_depth_buffer(float *data){
	int width, height;
	get_gl_size(width, height);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, data);
}

void visualize_saved_depth_buffer(float *data){
	if (data!=0){
		int width, height;
		get_gl_size(width, height);
		glDrawPixels(width, height, GL_LUMINANCE, GL_FLOAT, data);
	}
}

//////////////////////////////////////////////////////////



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

void reshape_depthbuf(){
	int width, height;
	get_gl_size(width, height);
	if (depth_up!=0)
		delete[] depth_up;
	depth_up = new float[width*height];
	if (depth_down!=0)
		delete[] depth_down;
	depth_down = new float[width*height];
	if (depth_left!=0)
		delete[] depth_left;
	depth_left = new float[width*height];
	if (depth_right!=0)
		delete[] depth_right;
	depth_right = new float[width*height];
}

void reshape(int w, int h) {
	gs->GW = w;
	gs->GH = h;

reshape_depthbuf();

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
	glBindTexture(GL_TEXTURE_2D, crateTex);
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
	  glBindTexture(GL_TEXTURE_2D, texture);

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

	  glBindTexture(GL_TEXTURE_2D, texture);

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
	  glBindTexture(GL_TEXTURE_2D, texture);

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
	glBindTexture(GL_TEXTURE_2D, gs->_attacks[3]._iconNum);

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
    setOrthoProjection();//push2
    glPushMatrix(); // ortho + 1 //push 3
	glDisable(GL_LIGHTING);
	//materials(Red);
	glColor3f(1.0, 0.0, 0.0);
	if(gs->player->_hp == 0) {
		sprintf(buff, "YOU DIED");
		renderBitmapString((gs->GW/2.0)-15, gs->GH/2.0,GLUT_BITMAP_TIMES_ROMAN_24,buff);
		Mix_HaltMusic(); 
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
	glPushMatrix();//1
	glTranslatef(40, gs->GH-40, 0);
	draw_circle2();
	draw_circle();
	glPopMatrix();//1

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
	drawUIBar(woodTex);
	glPopMatrix();

	glPushMatrix();  //UI bar that Health bars
	glTranslatef(gs->GW-60, gs->GH-95, -.01);
	drawUIHBar(palmTex);
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
	drawBox(gs->_attacks[gs->player->_ability[0]]._iconNum);
	glPopMatrix();
	//printf("ability %d\n", gs->player->_ability[0]);


    glPushMatrix();  //fireball (s)
	glTranslatef(25, gs->GH-300, 0);
	drawBox(gs->_attacks[gs->player->_ability[1]]._iconNum);
	glPopMatrix();

	glPushMatrix();  //crate (d)
	glTranslatef(25, gs->GH-360, 0);
	drawBox(gs->_attacks[gs->player->_ability[2]]._iconNum);
	glPopMatrix();

	glPushMatrix(); //(f)
	glTranslatef(25, gs->GH-420, 0);
	drawBox(gs->_attacks[gs->player->_ability[3]]._iconNum);
	glPopMatrix();

	glPushMatrix(); //(g) 
	glTranslatef(25, gs->GH-480, 0);
	drawBox(gs->_attacks[gs->player->_ability[4]]._iconNum);
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
glPushMatrix();
  glDisable(GL_LIGHTING);
  glColor3f(1, 1, 1);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, tileTex);

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
glPopMatrix();

}

void waterTile() {

  glDisable(GL_LIGHTING);
  glColor3f(1, 1, 1);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, waterTex);
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
  glBindTexture(GL_TEXTURE_2D, waterTex);

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


glEnable(GL_LIGHTING);

//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
//glBindTexture(GL_TEXTURE_2D, textures[13]);

//glColor3f(1,1,1);
glPushMatrix();
    glTranslatef(x, y, z);
	//glScalef(.009, .009, .009); rocks
	glScalef(100, 100, 100);
	//RenderOBJModel (plantemdl);
    //RenderOBJModelt (logmdl, logmtl);
glPopMatrix();

glDisable(GL_LIGHTING);

}


void displayicon(double x, double y, double z, int texture, double size) {
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTranslatef(x, -1, z);
		glRotatef(-90, 0, 0, 1);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-.25*size,.25*size,-3.1);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-.25*size,-.25*size,-3.1);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(.25*size,-.25*size,-3.1);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(.25*size,.25*size,-3.1);
		glEnd();
	glPopMatrix();
}

void display() {


	// time here is for calculating framerate...
	// DO NOT USE this for worldtime / ticks.
  static int frame=0;
  static int lasttime=0;
  
  int time = glutGet(GLUT_ELAPSED_TIME);
  
  ++frame;

  if (time - lasttime > 1000){
    fps = frame * 1000.0/((float)(time-lasttime));
    lasttime = time;
    frame = 0;
  }
	/////////////////////////////////////// fps calc end //

  if(gs && gs->_state == GSSTATE_ACTIVE){
	  	

		glPushMatrix();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	/*
	gluLookAt(-gs->player->_pos.x(), 0, -gs->player->_pos.y(),
		-(gs->player->_pos.x() - sin(gs->player->_vel.x())), 0,
		-(gs->player->_pos.y() + cos(gs->player->_vel.x())),
		0, 1, 0);
	*/
	//cout << gs->player->_pos.x() << " "<< gs->player->_pos.y() << endl
	//	<< gs->player->_vel.x() << " " << gs->player->_vel.y() << endl;

	
	//glEnable(GL_DEPTH_TEST);
	float theta;
	theta =0; // UP
	gluLookAt(gs->player->_pos.x(), 0, -gs->player->_pos.y(),
		gs->player->_pos.x() - sin(theta), 0,
		-(gs->player->_pos.y() + cos(theta)),
		0, 1, 0);
	gs->draw_objects();
	save_depth_buffer(depth_up);

	glPopMatrix();
	  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPushMatrix();
glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	theta = -M_PI/2; // RIGHT
	gluLookAt(gs->player->_pos.x(), 0, -gs->player->_pos.y(),
		gs->player->_pos.x() - sin(theta), 0,
		-(gs->player->_pos.y() + cos(theta)),
		0, 1, 0);
	gs->draw_objects();
	save_depth_buffer(depth_right);
 glPopMatrix();
	  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 		glPushMatrix();
glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	theta = M_PI/2; // LEFT
	gluLookAt(gs->player->_pos.x(), 0, -gs->player->_pos.y(),
		gs->player->_pos.x() - sin(theta), 0,
		-(gs->player->_pos.y() + cos(theta)),
		0, 1, 0);
	gs->draw_objects();
	save_depth_buffer(depth_left);
 glPopMatrix();
	  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 		glPushMatrix();
glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	theta = M_PI; // DOWN
	gluLookAt(gs->player->_pos.x(), 0, -gs->player->_pos.y(),
		gs->player->_pos.x() - sin(theta), 0,
		-(gs->player->_pos.y() + cos(theta)),
		0, 1, 0);
	gs->draw_objects();
	save_depth_buffer(depth_down);
 glPopMatrix();
	  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		
#if 0
		
		visualize_saved_depth_buffer(depth_up);

 
  glutSwapBuffers();

	glutPostRedisplay();

return;
#endif

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();  // ortho     //push1

  //glLoadIdentity(); not needed
//printOpenGLError();

    displayHud();

  //glPopMatrix();

  //resetPerspectiveProjection();
    //glPushMatrix(); // ortho + 2  //still at one push


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

	  //set up the camera

		gluLookAt(eyex + gs->player->_pos.x(), eyey, eyez - gs->player->_pos.y(), LAx + gs->player->_pos.x(), LAy, LAz - gs->player->_pos.y(), 0, 0, -1);
	    

			drawTiles();

		//printOpenGLError();
		glPushMatrix(); // Ocean 
			if(shadeOn){
				glUseProgram(ShadeProg);
				//printOpenGLError();
				glUniform1f(getUniLoc(ShadeProg, "wTime"), ((float)worldtime)*0.03);
				//printOpenGLError();
				//glUniform1f(getUniLoc(ShadeProg, "wHeight"), 0.5);
				//glUniform1f(getUniLoc(ShadeProg, "wTilt"), 0.0);
			}
			glScalef(8.0,1.0,8.0);
			glTranslatef(0.0,0.0,-7.2);
			//glEnable(GL_LIGHTING);
			glDisable(GL_LIGHTING);

			//materials(Blue);
			//printOpenGLError();
			glColor3f(0.0,0.0,1.0);
			RenderOBJModel(&oceanmdl);
			//printOpenGLError();
			//drawWater();
			materials(White);
			if(shadeOn) glUseProgram(0);
			glDisable(GL_LIGHTING);
		glPopMatrix(); // end Ocean

		glPushMatrix(); // push ai
	  drawAi();

	  glPopMatrix(); // pop ai
	  materials(ModeMat);

		glPushMatrix(); // GS display (all game objects should be drawn here)
			gsDisplay();	
		glPopMatrix();	// end GS


 // pop ortho + 2

    //printOpenGLError();


      glPushMatrix();//pushtree
        glTranslatef(0.0, 0.01, 0.0);
		    //materials(Sand);
        //drawGrid();
		    //glTranslatef(-1.0,0,-1.0);


        drawTree(0, 0, 0);
		    //drawCrates();

//printOpenGLError();

		//glutSolidSphere(1.0,10,10);
		    if(gs->beatim>-1) gs->besrc->draw();
      glPopMatrix();//poptree (tree used for testing)
	  if(gs->smit) drawSmite();
	  drawRapid();

  glPopMatrix(); // pop ortho
  glLoadIdentity();
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define SCRW 1.6
#define SCRH 1.6
  glPushMatrix();
		int height, width;
		get_gl_size(width, height);
		glPushMatrix();
		glTranslatef(-SCRW,SCRH,-2.0);
		glutSolidCube(0.05);
		glTranslatef(2*SCRW,0.0,0.0);
		glutSolidCube(0.05);
		glPopMatrix();
		glDisable(GL_LIGHTING);
		//glTranslatef(-1.2,0.0,-2.0);
		for(int i=0; i<width; ++i){
			//cout << depth_up[((height+5)/3*2)*width+i] << " ";
			if(depth_up[((height+5)/3*2)*width+i] < 0.98){
				glPushMatrix();
				glTranslatef(-SCRW+2*SCRW/width*i, depth_up[((height+5)/3*2)*width+i]* SCRH, -2.0);
				glColor3f(1.0,0.0,0.0);
				glutSolidCube(0.01);
				glPopMatrix();
			}
			
		}
		//cout << endl << endl << endl;

  glPopMatrix();


  //visualize_saved_depth_buffer();
  
  glutSwapBuffers();
    //printOpenGLError();
}

else if(mainmenu==0){

	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, menuchoice?menutex1:menutex0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-2.0,1.5,-3.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-2.0,-1.5,-3.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(2.0,-1.5,-3.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(2.0,1.5,-3.0);
	glEnd();

	glPopMatrix();
	glutSwapBuffers();

	glutPostRedisplay();
}
else if (mainmenu==1) {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, woodTex2);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-2.0,1.5,-3.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-2.0,-1.5,-3.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(2.0,-1.5,-3.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(2.0,1.5,-3.0);
	glEnd();

	glPopMatrix();
	displayicon(position[indexp], 0, 0, 0, 1.2);
	if (indexp < 1){
		displayicon(-1.4, 0, 0, gs->_attacks[indexa]._iconNum, 1);
	}
	else {
		displayicon(-1.4, 0, 0, gs->_attacks[attack[0]]._iconNum, 1);
		//gs->_attacks[gs->player->_ability[0]]._iconNum
	}

	if (indexp == 1){
		displayicon(-.7, 0, 0, gs->_attacks[indexa]._iconNum, 1);
	}
	else if(indexp==1){
		
	}
	else if(indexp>1){
		displayicon(-.7, 0, 0, gs->_attacks[attack[1]]._iconNum, 1);
	}

	if (indexp == 2){
		displayicon(0, 0, 0, gs->_attacks[indexa]._iconNum, 1);
	}
	/*else if(indexp==2){
		
	}*/
	else if(indexp>2){
		displayicon(0, 0, 0, gs->_attacks[attack[2]]._iconNum, 1);
	}

	if (indexp == 3){
		displayicon(.7, 0, 0, gs->_attacks[indexa]._iconNum, 1);
	}
	else if(indexp==3){
		
	}
	else if(indexp>3){
		displayicon(.7, 0, 0, gs->_attacks[attack[3]]._iconNum, 1);
	}

	if (indexp == 4){
		displayicon(1.4, 0, 0, gs->_attacks[indexa]._iconNum, 1);
	}
	else if(indexp==4){
		
	}
	else if(indexp>4){
		displayicon(1.4, 0, 0, gs->_attacks[attack[4]]._iconNum, 1);
	}

	/*displayicon(-0.7, 0, 0, gs->_attacks[1]._iconNum, 1);
	displayicon(0.0, 0, 0, gs->_attacks[2]._iconNum, 1);
	displayicon(0.7, 0, 0, gs->_attacks[3]._iconNum, 1);
	displayicon(1.4, 0, 0, gs->_attacks[4]._iconNum, 1);*/


	glutSwapBuffers();

	glutPostRedisplay();
}
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
	y -= gs->GH/2;// - 50;
	
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
	y -= gs->GH/2;// - 50;
	
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
		//gs->prapid();
		gs->_attacks[gs->player->_ability[0]].fire(x,y);
		break;
	case 'w': case 'W' :
		gs->_attacks[gs->player->_ability[3]].fire(x,y);
		break;
	case 's': case 'S' :
		//gs->spawnFireball();
		gs->_attacks[gs->player->_ability[1]].fire(x,y);
		break;
	case 'f': case 'F' :
		gs->_attacks[gs->player->_ability[3]].fire(x,y);
		break;
	case 'g': case 'G' :
		//if(!gs->smit) gs->smiteEm(x,y);
		gs->_attacks[gs->player->_ability[4]].fire(x,y);
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
		gs->_attacks[gs->player->_ability[2]].fire(x,y);
		break;

  }
}

void tick(int state) {
	gs->tick(worldtime);
	int coll = 0;

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
	if (gs->smit){
		gs->smsrc->move();

		gs->LarPaCollision(gs->smsrc,0,100,0,100);

		if(!gs->smsrc->active){
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
   unsigned int heroTexture = BindTextureBMP((char *)"textures/herotext.bmp", false);
   unsigned int eneTexture = BindTextureBMP((char *)"textures/cmtexture.bmp", false);
   playerMod = new mdmodel("model/hero.md5mesh","model/hero_idle.md5anim",heroTexture);
   enemyMod = new mdmodel("model/characterModel.md5mesh","model/characterModelIdle.md5anim",eneTexture);
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

void menuKbdS(int k, int x, int y){

	if(GLUT_KEY_DOWN == k){
		if (mainmenu==0) {
			menuchoice = 1;
		}
		else if (mainmenu==1){
				indexa--;
				if (indexa == -1) {
					indexa = 4;
				}
			cerr << "indexa = "<< indexa<<endl;
		}
	}
	else if(GLUT_KEY_UP == k){
		if (mainmenu==0){
			menuchoice = 0;
		}
		else if (mainmenu==1){
				indexa++;
				if (indexa==5) {
					indexa=0;
				}
				cerr << "indexa = "<< indexa<<endl;
		}
	}
	/*else if(GLUT_KEY_RIGHT == k) {
		if (indexp!=4){
			indexp++;
		}

	}*/
	else if(GLUT_KEY_LEFT == k){
		if (indexp!=0){
			indexp--;
		}
	}

}

void EnterGameMode();

void menuKbd(unsigned char k, int x, int y){
	if(13 == k){
		if(0 == menuchoice && mainmenu==0) {
			mainmenu=1;
		}
		else if (1 == menuchoice) {
			exit(0);
		}
		else if (mainmenu==1) {
			
			attack[indexp] = indexa;
			cerr << "choose attack " << indexa<<endl;
			//gs->player->_hp = 0;//ability(indexa, indexp); //= indexa;
			indexp++;
			if (indexp==5){
				EnterGameMode();
			}
		}





	}
}

void initMusic() {
  if(SDL_InitSubSystem(SDL_INIT_AUDIO) == -1){
    // SDL Audio subsystem could not be started
	  exit(7);
  }

  if(Mix_OpenAudio(44100, AUDIO_S16, 1, 1024) == -1){
    // SDL_Mixer could not be started
	  exit(8);
  }
  Mix_AllocateChannels(32);



	
}

// leave menu
void EnterGameMode(){

	 gs->start(0);



  Mix_Music * music = Mix_LoadMUS("music/beach.mp3");
  if(!music){
	printf("Mix_LoadMUS(\"beach.mp3\"): %s\n", Mix_GetError());

	//exit(9);


  }
  Mix_PlayMusic(music, -1);

	gs->player = new playerstate(worldtime, attack);
	gs->player->_hp = 100;
	gs->player->_mp = 200;
	gs->player->_pos.x() = 0.00;
	gs->player->_pos.y() = 0.00;
	gs->fbtim = -1;
	gs->explo = false;
	gs->smit = false;

	//loadAttacks();
	gs->updatBinLists(gs->player,UPDAT);





	for(int i = 0; i < 100; i++){  

	  /* uses gamestate object crate:
	  goCrate *temp = new goCrate(crateTex);
	  temp->_hp = 10;
	  temp->_pos = coord2d_t(rand()%20-10,rand()%20-10);
	  gs->_objects.push_back(temp);
	  */
	  //goCrate *temp = new goCrate(CRATEID+i, 10, OBJECTSTATE_CRATE, coord2d_t(rand()%20-10,rand()%20-10), crateTex);



		palmTree *tree = new palmTree(0, treemdl);
		tree->_pos.x() = rand()%100-50;
		tree->_pos.y() = rand()%100-50;
		tree->body = bbody(tree->_pos.x()-.2,-tree->_pos.y()-.2,tree->_pos.x()+.2,-tree->_pos.y()+.2,BB_AABB);
		tree->_hp = 10;
		tree->_id = TREEID + (tid++);
		gs->addObject(tree);

	}

	for(int i = 0; i < 100; i++){ 
		if (i>70) {
			goCrate *crt = new goCrate(crateTex);
			crt->_pos.x() = rand()%100-50;
			crt->_pos.y() = rand()%100-50;
			crt->body = bbody(crt->_pos.x()-.5,-crt->_pos.y()-.5,crt->_pos.x()+.5,-crt->_pos.y()+.5,BB_AABB);
			crt->_hp = 10;
			crt->_id = CRATEID + (cid++);
			gs->addObject(crt);

			Hut *hut = new Hut(hutTex, hutmdl);
			hut->_pos.x() = rand()%100-50;
			hut->_pos.y() = rand()%100-50;
			hut->body = bbody(hut->_pos.x()-1,-hut->_pos.y()-1,hut->_pos.x()+1,-hut->_pos.y()+1,BB_AABB);
			hut->_hp = 100;
			hut->_id = HUTID + (hid++);
			gs->addObject(hut);

			rock2 *rck2 = new rock2(rock2Tex, rand()%90, rock2mdl);
			rck2->_pos.x() = rand()%100-50;
			rck2->_pos.y() = rand()%100-50;
			rck2->body = bbody(rck2->_pos.x()-.2,-rck2->_pos.y()-.2,rck2->_pos.x()+.2,-rck2->_pos.y()+.2,BB_AABB);
			rck2->_hp = 10;
			rck2->_id = ROCKID + (rid2++);
			gs->addObject(rck2);

			veg *bush = new veg(0, rand()%90, vegmdl);
			bush->_pos.x() = rand()%100-50;
			bush->_pos.y() = rand()%100-50;
			bush->body = bbody(bush->_pos.x()-.2,-bush->_pos.y()-.2,bush->_pos.x()+.2,-bush->_pos.y()+.2,BB_AABB);
			bush->_hp = 10;
			bush->_id = VEGID + (vid++);
			gs->addObject(bush);
		}

	

	}

	cerr << "INFO: init lighting.. " << endl;
	init_lighting();
	pos_light();


	cerr << "INFO: init ai.. " << endl;
	init_ai();

	cerr << "INFO: init model.. " << endl;
	initModel();

	gs->besrc = new beam(gs->player); //only need one beam right now, so might as well initialize it now

	glutMouseFunc(mouse);
	glutKeyboardFunc( keyboard );
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutMotionFunc(processMouseActiveMotion);
	glutTimerFunc(WORLD_TIME_RESOLUTION,&tick,0);
	glutTimerFunc(1000, mana,0);
	glEnable(GL_DEPTH_TEST);
}

// return to menu
void ExitGameMode(){
	
}



void fnExit1(){
	system("pause");
}

int main( int argc, char** argv ) {
	srand (time (NULL));


	gs = new gamestate();
	// use this for debugging unexpected exits: 
	atexit (fnExit1);

	initMusic();
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

	initAttacks();


	//register glut callback functions
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( menuKbd );
	glutSpecialFunc( menuKbdS );

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





//cerr << "INFO: init gamestate.. " << endl;


  
  
  

  //glEnable(GL_LIGHTING);

	// loading textures

	crateTex = BindTextureBMP((char *)"textures/crate.bmp", false); //0
	partTex = init_particletex(); //1

  tileTex = BindTextureBMP((char *)"textures/images.bmp", true); //2

  waterTex = BindTextureBMP((char *)"textures/water.bmp", true); //3

  woodTex = BindTextureBMP((char *)"textures/wood3.bmp", true); //4

  palmTex = BindTextureBMP((char *)"textures/palm.bmp", true); //5

  hutTex = BindTextureBMP((char *)"textures/hut.bmp", true); //6

  rockTex = BindTextureBMP((char *)"textures/rock.bmp", true); //7

  rock2Tex = BindTextureBMP((char *)"textures/rock2.bmp", true); //8

  menutex0 = BindTextureBMP((char *)"textures/menu_new.bmp", false);
  menutex1 = BindTextureBMP((char *)"textures/menu_quit.bmp", false);
  woodTex2 = BindTextureBMP((char *)"textures/wood34.bmp", true); //4
  //gameover = BindTextureBMP((char *)"textures/gameover.bmp", true); //4


  


  init("model/palmTree.obj", treemdl);
  init("model/afro hut.obj", hutmdl);
  init("model/rock_a.obj", rockmdl);
  init("model/planeMesh.obj", &oceanmdl);
  init("model/rock_b.obj", rock2mdl);
  init("model/veg.obj", vegmdl);
  //init("model/log.obj", logmdl);
  mtlLoad("model/palm-arecaceae.mtl", treemtl, 4);
  mtlLoad("model/veg.mtl", vegmtl, 4);
  //mtlLoad("model/log.mtl", logmtl, 3);




  init_dispList();

  //init("model/conch.obj", plantemdl);
  //init("model/hut.obj", mdl);

	init_particle();

//  atexit (cleanup(test));


  
	

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
