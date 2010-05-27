/* ZJ Wood for CSC 476 Spring 2007
  Example program which includes both multi-texturing and a pixel shader for using a diffuse light map 
  along with a decal texture - Multi-texturing code is based of ATI's example light mapping code and the
  vertex and pixel shader integration are based off examples from Nvidia's Cg example code (#7 two texture example)
  Use "l" to toggle the two modes (shader currently includes a bumped up red value for differentiation purposes only!
  Note that you will need the texture examples and .cg files for this to work - ALSO this code was compiled and
  tested on a Macintosh - you will need to change header directories for a PC - May contain random unused code
   2/22 converted to GLSL
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <map>

#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#include <OPENGL/glext.h>
#endif

#include "GLSL_helper.h"
#include "tex.h"

#define BASE_TEX_NAME	0
#define LIGHT_TEX_NAME	1

/* Vector3D struct mostly used only for camera nav */
typedef struct Vector3 {
  float x;
  float y;
  float z;
  
  float nx;
  float ny;
  float nz;
  Vector3(float in_x, float in_y, float in_z) : x(in_x), y(in_y), z(in_z) {}
  Vector3() : x(0), y(0), z(0) {}
  Vector3(const Vector3& in_v) : x(in_v.x), y(in_v.y), z(in_v.z) {}
  Vector3(float in) : x(in), y(in), z(in) {}
  inline Vector3& operator =(const Vector3& v2) { x = v2.x; y=v2.y; z=v2.z; return *this;}
  inline void set(float in_x, float in_y, float in_z) { x = in_x; y = in_y; z = in_z;}
} Vector3;

//globals used mostly for camera
Vector3 eye, la;
int GW;
int GH;
int mouseX;
int mouseY;
int startx, starty, endx, endy;
float phi, theta;

/*Some multi-texturing stuff */
int  gbMultitexturing = 1;
GLfloat gOffset = 0.0f;

/*Global to access the shader program - used in keyboard to turn on and off */
int ShadeProg;

void display();

/* Code to set up using the GLSL shader */
int InstallShader(const GLchar *vShaderText, const GLchar *fShaderText) {
	GLuint VertVS, FragFS; //handles to shader object
	GLint vCompiled, fCompiled, linked; //status of shader
	
	VertVS = glCreateShader(GL_VERTEX_SHADER);
	FragFS = glCreateShader(GL_FRAGMENT_SHADER);
	
	//load the source
	glShaderSource(VertVS, 1, &vShaderText, NULL);
	glShaderSource(FragFS, 1, &fShaderText, NULL);
	
	//compile vertex shader and print log
	glCompileShader(VertVS);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetShaderiv(VertVS, GL_COMPILE_STATUS, &vCompiled);
	printShaderInfoLog(VertVS);
	
	//compile shader and print log
	glCompileShader(FragFS);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetShaderiv(FragFS, GL_COMPILE_STATUS, &fCompiled);
	printShaderInfoLog(FragFS);
	
	if (!vCompiled || !fCompiled) {
		printf("Error compiling the shader vertex: %d frag: %d", vCompiled, fCompiled);
		return 0;
	}
	
	//create a program object and attach the compiled shader
	ShadeProg = glCreateProgram();
	glAttachShader(ShadeProg, VertVS);
	glAttachShader(ShadeProg, FragFS);
	
	glLinkProgram(ShadeProg);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
	printProgramInfoLog(ShadeProg);

	printf("sucessfully installed shader\n");
	return 1;
	
}	
										
/* Initialization for some OpenGL stuff */
bool Initialize ()					
{

	// Start Of User Initialization
	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);								// Black Background
	glClearDepth (1.0f);												// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);											// The Type Of Depth Testing
	glEnable (GL_DEPTH_TEST);											// Enable Depth Testing
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);					// Set Perspective Calculations To Most Accurate
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);							// Draw polygons filled in

	return true;														// Return true (Initialization Successful)
}


//helper functions for working with vectors
Vector3 cross(const Vector3& a, const Vector3& b) {

	Vector3 c;
	c.x = a.y*b.z - a.z*b.y;
	c.y = a.z*b.x - a.x*b.z;
	c.z = a.x*b.y - a.y*b.x;

	return(c);
}

float dot(const Vector3& a, const Vector3& b) {
	return(a.x*b.x + a.y*b.y + a.z*b.z);
}

float length(const Vector3& a) {
	return(sqrt(dot(a, a)));
}

Vector3 normalize(const Vector3& a) {
	Vector3 u;
	float mag_a = length(a);
	u.x = a.x/mag_a;
	u.y = a.y/mag_a;
	u.z = a.z/mag_a;
	return(u);
}


/*Draw ground plane from ATI example - fairly straight forward - this is for shader */
void DrawGroundPlane(void)
{
	float size = 12.0f;
	float altitude = -0.001f;

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-size, altitude, -size);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-size, altitude, size);

		glTexCoord2f(1.0f, 1.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(size, altitude, -size);

		glTexCoord2f(1.0f, 0.0f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(size, altitude, size);
	glEnd();
}

/*Draw ground plane from ATI example - fairly straight forward - this is for multi-texturing */
void DrawMultitextureGroundPlane(float offset)
{
	float size = 12.0f;
	float altitude = -0.001f;

	glBegin(GL_TRIANGLE_STRIP);
		glMultiTexCoord2f(GL_TEXTURE0_ARB, 0.0f, 1.0f);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 0.0f+offset, 1.0f+offset);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-size, altitude, -size);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 0.0f, 0.0f);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 0.0f+offset, 0.0f+offset);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-size, altitude, size);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 1.0f, 1.0f);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 1.0f+offset, 1.0f+offset);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(size, altitude, -size);

		glMultiTexCoord2f(GL_TEXTURE0_ARB, 1.0f, 0.0f);
		glMultiTexCoord2f(GL_TEXTURE1_ARB, 1.0f+offset, 0.0f+offset);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(size, altitude, size);
	glEnd();
}


/*Display function */
void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		//set up the camera
		gluLookAt(eye.x, eye.y, eye.z, la.x, la.y, la.z, 0, 1,0);
	
	cout << "eye " << eye.x << " " << eye.y << " " << eye.z << "lA: " << la.x << " " << la.y << " " << la.z << endl;
		glPushMatrix();
			
			if(gbMultitexturing==0)
			{
				cout << "using multitexturing" << endl;
				glActiveTexture(GL_TEXTURE0_ARB);
				glBindTexture(GL_TEXTURE_2D, BASE_TEX_NAME);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glEnable(GL_TEXTURE_2D);

				glActiveTexture(GL_TEXTURE1_ARB);
				glBindTexture(GL_TEXTURE_2D, LIGHT_TEX_NAME);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glEnable(GL_TEXTURE_2D);

				DrawMultitextureGroundPlane(gOffset);

			}
			else if(gbMultitexturing==1)
			{	
				cout << "using a shader" << endl;
			  	glUseProgram(ShadeProg);
				
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, BASE_TEX_NAME);
				
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, LIGHT_TEX_NAME);
				
				//set relationship of texturs to samplers in shader
				glUniform1i(getUniLoc(ShadeProg, "ltex"), LIGHT_TEX_NAME);
				glUniform1i(getUniLoc(ShadeProg, "dtex"), BASE_TEX_NAME);
				
				glUniform1f(getUniLoc(ShadeProg, "red"),0.55);

				DrawGroundPlane();
				
				//turn off the use of any shader
				glUseProgram(0);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_2D);
			  
			} else {
				cout << "using texturing alone, no lights" << endl;
				glActiveTexture(GL_TEXTURE0_ARB);
				glBindTexture(GL_TEXTURE_2D, BASE_TEX_NAME);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glEnable(GL_TEXTURE_2D);
				DrawGroundPlane();
				glDisable(GL_TEXTURE_2D);
			  }
		glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();

}


/* ASWD camera controls */
void keyboard(unsigned char key, int x, int y) {

  float delta = 0.2;
  Vector3 gaze,left;
  switch(key) {
	  case 'a':
		gaze.x = la.x - eye.x;
		gaze.y = la.y - eye.y;
		gaze.z = la.z - eye.z;
		left = cross(Vector3(0, 1, 0), gaze);
		eye.x += delta*left.x;
		eye.y += delta*left.y;
		eye.z += delta*left.z;
		la.x += delta*left.x;
		la.y += delta*left.y;
		la.z += delta*left.z;
		break;
	  case 'd':
		gaze.x = la.x - eye.x;
		gaze.y = la.y - eye.y;
		gaze.z = la.z - eye.z;
		left = cross(Vector3(0, 1, 0), gaze);
		eye.x -= delta*left.x;
		eye.y -= delta*left.y;
		eye.z -= delta*left.z;
		la.x -= delta*left.x;
		la.y -= delta*left.y;
		la.z -= delta*left.z;
		glutPostRedisplay();
		break;
	  case 's':
		gaze.x = la.x - eye.x;
		gaze.y = la.y - eye.y;
		gaze.z = la.z - eye.z;
		eye.x -= delta*gaze.x;
		eye.y -= delta*gaze.y;
		eye.z -= delta*gaze.z;
		la.x -= delta*gaze.x;
		la.y -= delta*gaze.y;
		la.z -= delta*gaze.z;
		glutPostRedisplay();
		break;
	   case 'w':
		gaze.x = la.x - eye.x;
		gaze.y = la.y - eye.y;
		gaze.z = la.z - eye.z;
		eye.x += delta*gaze.x;
		eye.y += delta*gaze.y;
		eye.z += delta*gaze.z;
		la.x += delta*gaze.x;
		la.y += delta*gaze.y;
		la.z += delta*gaze.z;
		glutPostRedisplay();
		break;
	  case 'q':
		  exit( EXIT_SUCCESS );
		  break;
	  case 'l':
		  gbMultitexturing +=1;
		  gbMultitexturing = gbMultitexturing%3;
		  printf("gbmult %d\n", gbMultitexturing);
		  break;
	  default:
		  break;
	}
	glutPostRedisplay();
}

void reshape(int w, int h) {
	GW = w;
	GH = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0, (float)w/h, 0.1, 50);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);

	glutPostRedisplay();
}


int main( int argc, char** argv ) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Light map example");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	/*register glut call backs */
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc(keyboard);
	
	/* projection/modelview set up */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 0.1, 50);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, 500, 500); //image to pixel

    /*initialize camera stuff */
	GW=GH = 500;
	phi = 0.0; theta = -90.0;	
	eye.x = 0.0; eye.y = 10.0; eye.z = 2.0;
	la.x = 0; la.y = 1.0; la.z = 0;
	/* Start with the shader - this variable is defined in tex.h */
	gbMultitexturing = 2;
  
	/* Load in the two textures we'll use for this program and create texture objects */
	LoadTexture("texture/image1.bmp",    BASE_TEX_NAME);	// Here we load a normal texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	LoadTexture("texture/lightmap.bmp", LIGHT_TEX_NAME);	// Here we load a lightmap
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	

	Initialize ();
	
	/*set up shader after we've loaded in the textures*/
	getGLversion();
	if (!InstallShader(textFileRead("VLight.glsl"),textFileRead("FLight.glsl"))) {
		printf("Error installing shader!\n");
		return 0;
	}

	glutMainLoop();
}


