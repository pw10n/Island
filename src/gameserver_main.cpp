
#include <cstdio>
#include <cstdlib>

extern "C"{
#include <GL/glut.h>
}

#include <iostream>

#include "netutil.h"
#include "network.h"

using namespace std;

int previous_gSrvState = SERVER_DISCONNECT;
server gSrv;
gamestate_t gObj(0, "default.map");


void refresh(int i){
	glutPostRedisplay();
	glutTimerFunc(1000,&refresh,0);
}
void tick(int i){
	if (gSrv.getState() != previous_gSrvState){
		previous_gSrvState = gSrv.getState();
		cerr << "DEBUG: Serverstate changed to " << previous_gSrvState << endl;
	}

	gSrv.tickSnd();
	gSrv.tickRcv();
		
	glutTimerFunc(30,&tick,0);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y){
	switch (key){
		case 's': case 'S':
			gSrv.startGame(); break;
	}
}

int main( int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 200);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Server Console");

	cerr << "INFO: setting up server" << endl;
	gSrv.setGameObj(&gObj); 
	gSrv.setup("13370");
	gSrv.setState(SERVER_W4PLAYERS);

	cerr << "INFO: initializing callbacks" << endl;
	glutTimerFunc(30,&tick,0);
	glutTimerFunc(1000,&refresh,0);

	glutKeyboardFunc(&keyboard);

	glutDisplayFunc(&display);

	glutMainLoop();
}
