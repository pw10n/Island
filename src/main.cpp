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
#include "network.h"
#include "texture.h"

#include "types.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/glut.h>

#define __STDC_LIMIT_MACROS
#include "stdint.h"


//#include "types.h"
//#include "gs_types.h"

using namespace std;

client gClient;

#define WORLD_TIME_RESOLUTION 30

uint32_t worldtime=0;

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

materialStruct Grey = {
  {0.3, 0.3, 0.3, 1.0},
  {0.3, 0.3, 0.3, 1.0},
  {0.3, 0.3, 0.3, 1.0},
  {0.3}
};

materialStruct Sand = {
	{0.85, 0.79, 0.71, 1.0},
	{0.85, 0.79, 0.71, 1.0},
	{0.0, 0.0, 0.0, 1.0},
	{0.0}
};
coord2d_t vel;
playerstate_t* player;

gamestate_t* gsObj;

//sets up a specific material
void materials(materialStruct materials) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, materials.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materials.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, materials.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, materials.shininess);
}


int light;
//globals for lighting - use a white light and apply materials
//light position
GLfloat light_pos[4] = {1.0, 5.0, 1.5, 1.0};
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
bool rfire = false;
GLubyte * alpha;

fireball_s * fbsrc;
vector<fireball_p *> fbpar;
int fbtim;
explosion_s * exsrc;
vector<particle *> expar;
bool explo;
vector<rapidfire *> rfpar;

vector<unsigned int> textures;
vector<objectstate_t> crates;

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
	float fbx = -sin(theta);
	float fbz = -cos(theta);
	fbsrc = new fireball_s(player->_pos.x(),-player->_pos.y(),fbx/5.0f,fbz/5.0f);
	for(int i=0;i<200;i++){
		fbpar.push_back(new fireball_p(fbsrc));
	}
}

void detonate(fireball_s * fbs, bool splin){
	exsrc = new explosion_s(fbs->x,fbs->z);
	if(!splin){
		for(int i=0;i<400;i++){
			expar.push_back(new explosion_p(exsrc));
		}
	}
	else{
		for(int i=0;i<200;i++){
			expar.push_back(new explosion_p(exsrc));
		}
		for(int i=0;i<200;i++){
			expar.push_back(new splinter(exsrc));
		}
	}
}

void rapid(){
	if(rfpar.size()<100)
		rfpar.push_back(new rapidfire(player->_pos.x(),-player->_pos.y(),theta,angle));
}

void reshape(int w, int h) {
  GW = w;
  GH = h;


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (h!=0){
      gluPerspective(80, w/h, .5, 24);
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
  glBegin(GL_POLYGON);
	materials(Sand);
	glVertex3f(columns,-0.01,rows);
	glVertex3f(columns,-0.01,-rows);
	glVertex3f(-columns,-0.01,-rows);
	glVertex3f(-columns,-0.01,rows);
  glEnd();
}


void drawPlayer() {
  materials(Grey);
  glPushMatrix();
  
    glTranslatef(0.0, 0.25, 0.0);
    glTranslatef(0.0, 0.0, -0.5);
    gluCylinder(gluNewQuadric(), .05, .2, 1, 12, 36);
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

void drawCrate(){
	objectstate_t crate;
	crate.setType(OBJECTSTATE_CRATE);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// it'll be 2x2x2 for now
	glBindTexture(GL_TEXTURE_2D, textures[OBJECTSTATE_CRATE]);
	// "front"
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (-0.5, 0.0, 1.0);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (1.0, 0.0, 1.0);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (1.0, 1.0, 1.0);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (0.0, 1.0, 1.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, textures[OBJECTSTATE_CRATE]);
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (1.0, 0.0, 0.0);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (1.0, 0.0, 1.0);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (1.0, 1.0, 1.0);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (1.0, 1.0, 0.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, textures[OBJECTSTATE_CRATE]);
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (0.0, 1.0, 0.0);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (1.0, 1.0, 0.0);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (1.0, 1.0, 1.0);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (0.0, 1.0, 1.0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, textures[OBJECTSTATE_CRATE]);
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (0.0, 0.0, 1.0);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (0.0, 0.0, 0.0);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (0.0, 1.0, 0.0);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (0.0, 1.0, 1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void display() {

  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
  glMatrixMode(GL_MODELVIEW);
    
  glPushMatrix();
  //set up the camera
    gluLookAt(eyex + player->_pos.x(), eyey, eyez - player->_pos.y(), LAx + player->_pos.x(), LAy, LAz - player->_pos.y(), 0, 0, -1);
    glPushMatrix();

      glPushMatrix();
		glTranslatef(player->_pos.x(), 0, -player->_pos.y());
        glRotatef(angle, 0, 1, 0);

        drawPlayer();
      glPopMatrix();

      glPushMatrix();
        glTranslatef(0.0, 0.01, 0.0);
        drawGrid();
		glTranslatef(-1.0,0,-1.0);
		drawCrate();
		//glutSolidSphere(1.0,10,10);
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
			if(rfire) { rapid(); }
			else if(fbtim<0) {spawnFireball(); fbtim = 0;}
		}
	}
}

void processMousePassiveMotion(int x, int y) {


	//float theta = 0;
	x -= GW/2;
	x *= .5;
	y -= GH/2;
	
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
	y -= GH/2;
	
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

  glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y ){
  switch( key ) {
    case 'q': case 'Q' :
      exit( EXIT_SUCCESS );
      break;
	case ' ':
		rfire = !rfire;
		break;
  }
}

void tick(int state) {
	player->change_velocity(vel);
	player->tick(worldtime);
	if (flag){

		//myX += -sin(theta);
		///myZ += cos(theta);

	}
	if (fbtim>-1){
		fbtim++;
		fbsrc->move();
		for(uint32_t i=0;i<fbpar.size();i++){
			fbpar[i]->move();
			if(fbpar[i]->life<0.0f){
				fbpar[i] = new fireball_p(fbsrc);
			}
		}
	}
	if(fbtim>50){
		fbtim=-1;
		fbsrc->active = false;
		fbpar.clear();
		detonate(fbsrc,false);
		explo = true;
	}
	else if(fbtim>-1&&fbsrc->collide(1.0,1.0,1.0)){
		fbtim=-1;
		fbsrc->active = false;
		fbpar.clear();
		detonate(fbsrc,true); //if fbtim less than 50, fb must have collided with something
		explo = true;
	}
	if (explo){
		exsrc->move();
		for(int i=expar.size()-1;i>-1;i--){
			expar[i]->move();
			if(expar[i]->life<0.0f){
				expar.erase(expar.begin()+i);
			}
		}
		if(expar.empty()){
			explo = false;
		}
	}
	for(int i=rfpar.size()-1;i>-1;i--){
		rfpar[i]->move();
		if(!rfpar[i]->boom&&rfpar[i]->collide(1.0,1.0,1.0)){
			rfpar[i]->boom = true;
			rfpar[i]->life = 0.0;
		}
		if(!rfpar[i]->active){
			rfpar.erase(rfpar.begin()+i);
		}
	}
	glutPostRedisplay();

	worldtime+=WORLD_TIME_RESOLUTION;

	glutTimerFunc(WORLD_TIME_RESOLUTION, &tick, 0);
}

int main( int argc, char** argv ) {
  

  //set up my window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600); 
  GW = 800;
  GH = 600;
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Island");
  /*glutGameModeString("800x600:32");
  if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)){
	glutEnterGameMode();
  }
  else{
	  exit(1);
  }*/
  //glClearColor(0.0, 0.0, 0.0, 1.0);
  glClearColor(1.0, 1.0, 1.0, 1.0);

  myX = 0;
  myY = 0;
  myZ = 0;
  angle = 0;
  theta = 0;
  eyex = 0;
  eyey = 4.33;//4.33;
  eyez = 5;//5;
  LAx = 0;
  LAy = 0;
  LAz = 0;

  player = new playerstate_t(worldtime);
  gsObj = new gamestate_t(worldtime, "default.map");

  



  fbtim = -1;
  explo = false;

  cerr << "Connecting to gameserver... ";
  try{
	gClient.connectTo("127.0.0.1","13370");
	gClient.setGameObj(gsObj);
	cerr << " OK" << endl;
  }
  catch (gException e){
	cerr << e.what() << endl;
	exit(1);
  }

  gsObj->addPlayer(player);
  gClient.addPlayer(player);

  //
  //register glut callback functions
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutMouseFunc(mouse);
  glutKeyboardFunc( keyboard );
  glutPassiveMotionFunc(processMousePassiveMotion);
  glutMotionFunc(processMouseActiveMotion);
  glutTimerFunc(WORLD_TIME_RESOLUTION,&tick,0);
  glEnable(GL_DEPTH_TEST);

  init_lighting();
  glEnable(GL_LIGHTING);

	// loading textures
	// clearing the vector
	// Prentice says a vector is overkill for holding textures, but I'll do it for now
	textures.clear();
	unsigned int crateTexture;
	crateTexture = BindTextureBMP((char *)"../../../resources/textures/crate.bmp");
	textures.push_back(crateTexture);
	unsigned int partTexture = init_particletex();
	textures.push_back(partTexture);
	init_particle();
  
  glutMainLoop();
}
