#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <vector>
//#include "particle.cpp"
#include "particle.h"

using namespace std;

//#define NUM_PARTS 1000
#define PI 3.14159265
#define DSIN(x) sin(x*.01745329)
#define DCOS(x) cos(x*.01745329)
#define MIN(x,y) (x>y)?y:x
#define MAX(x,y) (x>y)?x:y

#define FIREBALL 1
#define BOOSTA   2
#define BOOSTB   3
#define SPRAY    4
#define EXPLOSION 5

GLubyte * alpha;
float theta=0.0f,phi=-45.0f;
float vx=0.0f,vy=2.0f,vz=-2.0f,startx,starty;
float absV=.05f; //absolute velocity
int GW,GH;
int num = 500,mode = FIREBALL;
int now,then=0;
source * src;
vector<particle *> parts;

float p2w_x(int x){
    //return (1*x-.5*GW)/(float)GH;
	return (4*x-2*GW)/(float)GH;
}

float p2w_y(int y){
    //return (1*y-.5*GH)/(float)GH;
	return (4*y-2*GH)/(float)GH;
}

void reshape(int w, int h) {
	GW = w;
	GH = h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,(float)w/h,.10,25.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
	
	glutPostRedisplay();
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

void init_tex() {
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	genTex();
	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 16, 16, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alpha);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


}

void init_particle(){
	glNewList(PARTLIST,GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0.1f,0.1f,0);
	glTexCoord2d(0,1); glVertex3f(-0.1f,0.1f,0);
	glTexCoord2d(1,0); glVertex3f(0.1f,-0.1f,0);
	glTexCoord2d(0,0); glVertex3f(-0.1f,-0.1f,0);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0.1f,0,0.1f);
	glTexCoord2d(0,1); glVertex3f(-0.1f,0,0.1f);
	glTexCoord2d(1,0); glVertex3f(0.1f,0,-0.1f);
	glTexCoord2d(0,0); glVertex3f(-0.1f,0,-0.1f);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0,0.1f,0.1f);
	glTexCoord2d(0,1); glVertex3f(0,-0.1f,0.1f);
	glTexCoord2d(1,0); glVertex3f(0,0.1f,-0.1f);
	glTexCoord2d(0,0); glVertex3f(0,-0.1f,-0.1f);
	glEnd();
	glEndList();
}

void init_parts(){
	src = new source(0,0,0,1.0f,0,0);
	//if(mode==BOOSTA) src->vx = 0;
	for(int i=0;i<num;i++){
		switch(mode){
			case FIREBALL : parts.push_back(new fireball(src,true)); break;
			case BOOSTA : parts.push_back(new boosta(src)); break;
			case BOOSTB : parts.push_back(new boostb(src)); break;
			case SPRAY : parts.push_back(new spray(src,-90,true)); break;
			case EXPLOSION : parts.push_back(new explosion(src)); break;
		}
	}
}

void drawGrid(){
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_LINES);
	for(int i=-25;i<26;i++){
		glVertex3i(-25,0,i);
		glVertex3i(25,0,i);
		glVertex3i(i,0,-25);
		glVertex3i(i,0,25);
	}
	glEnd();
	if(mode==FIREBALL){
		glColor3f(1.0f,1.0f,0.0f);
		glutSolidSphere(.5,10,10);
	}else{
		glutWireSphere(.5,10,10);
	}
}

void drawStuff(){
	glDisable(GL_TEXTURE_2D);
	/*glBegin(GL_LINES);
	glVertex3i(-1,0,0); glVertex3i(1,0,0);
	glVertex3i(0,-1,0); glVertex3i(0,1,0);
	glVertex3i(0,0,-1); glVertex3i(0,0,1);
	glEnd();*/
	drawGrid();
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(-(src->x),-(src->y),-(src->z));
	for(int i=0;i<parts.size();i++){
		if(parts[i]->active) parts[i]->draw();
	}
	glPopMatrix();
}

void display() {
	if(phi>85.0) phi=85.0;
	if(phi<-85.0) phi=-85.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	//set up the camera
	float dx = DSIN(theta)*DCOS(phi);
	float dy = DSIN(phi);
	float dz = DCOS(theta)*DCOS(phi);
	gluLookAt(vx,vy,vz,vx+dx,vy+dy,vz+dz, 0.0,1.0,0.0);
	
	drawStuff();
	
	glPopMatrix();
	
	glutSwapBuffers();
	
}

void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) { /* if the left button is clicked */
      printf("mouse clicked at %d %d\n", x, GH-y-1);
      startx = p2w_x(x);
      starty = p2w_y(GH-y-1);
      printf("world: %f %f\n",startx,starty);
	  printf("theta: %f\n",theta);
	  printf("  phi: %f\n",phi);
    } 
  }
}

void mouseMove(int x, int y) {
	//printf("mouse moved at %d %d\n", x, GH-y-1);
	printf("world: %f %f\n",p2w_x(x),p2w_y(GH-y-1));
	theta += 10.0*(p2w_x(x)-startx);
	phi -= 10.0*(p2w_y(GH-y-1)-starty);
	startx = p2w_x(x);
    starty = p2w_y(GH-y-1);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
	switch (key){
		case 'w' : //move forward
			vx += .1*DSIN(theta)*DCOS(phi);
			vy += .1*DSIN(phi);
			vz += .1*DCOS(theta)*DCOS(phi);
			break;
		case 's' : //move back
			vx -= .1*DSIN(theta)*DCOS(phi);
			vy -= .1*DSIN(phi);
			vz -= .1*DCOS(theta)*DCOS(phi);
			break;
		case 'a' : //move left
			vx += .1*DCOS(theta);
			vz -= .1*DSIN(theta);
			break;
		case 'd' : //move right
			vx -= .1*DCOS(theta);
			vz += .1*DSIN(theta);
			break;
		case '1' :
			if(mode!=FIREBALL){
				parts.clear();
				num = 500;
				mode = FIREBALL;
				init_parts();
			}
			break;
		case '2' :
			if(mode!=BOOSTA){
				parts.clear();
				num = 200;
				mode = BOOSTA;
				init_parts();
			}
			break;
		case '3' :
			if(mode!=BOOSTB){
				parts.clear();
				num = 200;
				mode = BOOSTB;
				init_parts();
			}
			break;
		case '4' :
			if(mode!=SPRAY){
				parts.clear();
				num = 500;
				mode = SPRAY;
				init_parts();
			}
			break;
		case '5' :
			if(mode!=EXPLOSION){
				parts.clear();
				num = 500;
				mode = EXPLOSION;
				init_parts();
			}
			break;
		case ' ' :  //restart explosion
			if(mode==EXPLOSION){
				parts.clear();
				init_parts();
			}
			break;
	}
	glutPostRedisplay();
}

void idle(){
	now = glutGet(GLUT_ELAPSED_TIME);
	float dt = (float)(now-then)/100.0f;
	src->move(dt);
	for(int i=0;i<parts.size();i++){
		if(parts[i]->active) parts[i]->move(dt);
		if(parts[i]->life<0.0f){
			if(mode==FIREBALL) parts[i] = new fireball(src,false);
			else if(mode==BOOSTA) parts[i] = new boosta(src);
			else if(mode==BOOSTB) parts[i] = new boostb(src);
			else if(mode==SPRAY) parts[i] = new spray(src,-90,false);
			else if(mode==EXPLOSION) parts[i]->active = false; //explosions don't refresh
		}
	}
	then = now;
	glutPostRedisplay();
}

int main( int argc, char** argv ) {
  
	//set up my window
	glutInit(&argc, argv);
	//main window
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 700); 
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Main");
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutIdleFunc( idle );
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glDisable(GL_DEPTH_TEST);

	//init_lighting();
	init_tex();
	init_particle();

	init_parts();
	glutMainLoop();
}
