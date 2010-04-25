#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <cstring>
#include <map>
#include <vector>
#include "collision.h"
#include "gs_types.h"
#include "types.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/glut.h>

#define __STDC_LIMIT_MACROS
#include "stdint.h"

#define WORLD_TIME_RESOLUTION 30

uint32_t worldtime=0;

//#include "types.h"
//#include "gs_types.h"

using namespace std;


typedef struct materialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess[1];
} materialStruct;

materialStruct Black = {
  {0.0, 0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0, 0.0},
  {0.0}
};

materialStruct Grey = {
  {0.3, 0.3, 0.3, 0.3},
  {0.3, 0.3, 0.3, 0.3},
  {0.3, 0.3, 0.3, 0.3},
  {0.3}
};

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
coord2d_t vel;
playerstate_t* player;




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

}


void drawPlayer() {
  materials(Grey);
  glPushMatrix();
  
    glTranslatef(0.0, 0.25, 0.0);
    glTranslatef(0.0, 0.0, -0.5);
    gluCylinder(gluNewQuadric(), .05, .2, 1, 12, 36);
  glPopMatrix();
}


void display() {

  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
  glMatrixMode(GL_MODELVIEW);
    
  glPushMatrix();
  //set up the camera
    gluLookAt(eyex + (myX/10.0), eyey, eyez - (myZ/10.0), LAx + (myX/10.0), LAy, LAz - (myZ/10.0), 0, 0, -1);
    glPushMatrix();

      glPushMatrix();
		//glTranslatef((myX/10.0), 0, (-myZ/10.0));
		printf("x: %d\n", player->_pos.x());
		glTranslatef(player->_pos.x(), 0, player->_pos.y());
		glRotatef(angle, 0, 1, 0);
        drawPlayer();
      glPopMatrix();

      glPushMatrix();
        glTranslatef(0.0, 0.01, 0.0);
        drawGrid();
      glPopMatrix();

    glPopMatrix();
  glPopMatrix();
  
  glutSwapBuffers();
    
}



void mouse(int button, int state, int x, int y) {
  if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) { 
		flag = true;
    }
	else {
		flag = false;
	}
  }
}

void processMousePassiveMotion(int x, int y) {


	//float theta = 0;
	x -= GW/2;
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
	
	
  glutPostRedisplay();

}

void processMouseActiveMotion(int x, int y) {
	
	//float theta = 0;
	x -= GW/2;
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
	
  glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y ){
  switch( key ) {
    case 'q': case 'Q' :
      exit( EXIT_SUCCESS );
      break;
  }
}

void tick(int state) {

	


	if (flag){
		myX += -sin(theta);
		myZ += cos(theta);
		vel.x() = 2;
		vel.y() = 2;
		
		player->change_velocity(vel);
	}

	glutPostRedisplay();
	worldtime+=WORLD_TIME_RESOLUTION;
	player->tick(worldtime);
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
  glutCreateWindow("Mesh display");
  /*glutGameModeString("800x600:32");
  if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)){
	glutEnterGameMode();
  }
  else{
	  exit(1);
  }*/
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

  
  glutMainLoop();
}
