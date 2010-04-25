
#include <cstdio>
#include <cstdlib>

extern "C"{
#include <GL/glut.h>
}

#include <iostream>

#include "netutil.h"
#include "network.h"

using namespace std;

server gSrv;
gamestate_t gObj(0, "default.map");

void tick(int i){
	gSrv.tickSnd();
	gSrv.tickRcv();
		
	glutTimerFunc(30,&tick,0);
}

void display(){

}

int main( int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Server");


	cerr << "INFO: setting up server" << endl;
	gSrv.setGameObj(&gObj); 
	gSrv.setup("13370");

	cerr << "INFO: initializing callbacks" << endl;
	glutTimerFunc(30,&tick,0);

	glutDisplayFunc(&display);

	glutMainLoop();
}