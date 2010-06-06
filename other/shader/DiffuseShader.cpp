/*
 *  DiffuseShader.cpp
 *  CSC473
 *  glut/OpenGL application which renders a cube.  
 *  works in conjunction with a GLSL vertex shader to model diffuse lighting only
 *
 *  Created by zwood on 2/21/10.
 *  Copyright 2010 Cal Poly. All rights reserved.
 *
 *****************************************************************************/

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "GLSL_helper.h"

//flag and ID to toggle on and off the shader
int shade = 1;
int ShadeProg;

/*function to help load the shader */
int InstallShader(const GLchar *vShaderName) {
	GLuint DiffuseVS; //handles to shader object
	GLint vCompiled, linked; //status of shader
	
	DiffuseVS = glCreateShader(GL_VERTEX_SHADER);
	
	//load the source
	glShaderSource(DiffuseVS, 1, &vShaderName, NULL);
	
	//compile shader and print log
	glCompileShader(DiffuseVS);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetShaderiv(DiffuseVS, GL_COMPILE_STATUS, &vCompiled);
	printShaderInfoLog(DiffuseVS);
	
	if (!vCompiled) {
		printf("Error compiling the shader %s", vShaderName);
		return 0;
	}
	 
	//create a program object and attach the compiled shader
	ShadeProg = glCreateProgram();
	glAttachShader(ShadeProg, DiffuseVS);
	
	glLinkProgram(ShadeProg);
	/* check shader status requires helper functions */
	printOpenGLError();
	glGetProgramiv(ShadeProg, GL_LINK_STATUS, &linked);
	printProgramInfoLog(ShadeProg);

	glUseProgram(ShadeProg);
	
	/* example of setting a uniform variable in the shader - could access light position if one defined
	    through LightSource[i].position */
	glUniform3f(getUniLoc(ShadeProg,	"LightPos"), 4.0f, 1.0f, 4.0f);
	glUniform3f(getUniLoc(ShadeProg,	"EyePos"), 0.0f, 0.0f, 4.0f);
	
	printf("sucessfully installed shader %d\n", ShadeProg);
	return 1;
	
}

/* Some OpenGL initialization */
bool Initialize ()					// Any GL Init Code 
{

	// Start Of User Initialization
	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);								// Black Background
	glClearDepth (1.0f);												// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);											// The Type Of Depth Testing
	glEnable (GL_DEPTH_TEST);											// Enable Depth Testing
	glShadeModel (GL_SMOOTH);											// Select Smooth Shading
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);							// Draw Our cube filled in

	return true;														
}


/* some keyboard toggles - notably space bar toggles shader */
void Update (int key,int x,int y)										
{
	if ( key==27 )		//ESC key							
	{
		exit(0);						
	};

	if ( key==' ' ) {
		shade = !shade;
		if (!shade) {
		 glUseProgram(0);
		} else {
		  glUseProgram(ShadeProg);
		}
	}

	glutPostRedisplay();
}

/*Draw a cube face by face */
void DrawCube() {
	//left face: for lighting you must specify a normal for each face!
  glNormal3f(-1.0, 0.0, 0.0);
  glBegin(GL_POLYGON);
  glVertex3f(-0.5, 0.5, 0.5); //v5
  glVertex3f(-0.5, 0.5, -0.5); //v4
  glVertex3f(-0.5, -0.5, -0.5); //v1
  glVertex3f(-0.5, -0.5, 0.5); //v8
  glEnd();
  
  //right face:  for lighting you must specify a normal for each face!
  glNormal3f(1.0, 0.0, 0.0);
  glBegin(GL_POLYGON);
  glVertex3f(0.5, -0.5, -0.5); //v2
  glVertex3f(0.5, 0.5, -0.5); //v3
  glVertex3f(0.5, 0.5, 0.5); //v6
  glVertex3f(0.5, -0.5, 0.5); 
  glEnd();
  
  //bottom face:  for lighting you must specify a normal for each face!
  glNormal3f(0, -1.0, 0.0);
  glBegin(GL_POLYGON);
  glVertex3f(-0.5, -0.5, 0.5); //v8
  glVertex3f(0.5, -0.5, 0.5); //v7
  glVertex3f(0.5, -0.5, -0.5); //v2
  glVertex3f(-0.5, -0.5, -0.5); //v1
  glEnd();
  
  //top face:  for lighting you must specify a normal for each face!
  glNormal3f(0, 1.0, 0.0);
  glBegin(GL_POLYGON);
  glVertex3f(0.5, 0.5, -0.5); //v3
  glVertex3f(-0.5, 0.5, -0.5); //v4
  glVertex3f(-0.5, 0.5, 0.5); //v5
  glVertex3f(0.5, 0.5, 0.5); //v6
  glEnd();
  
  //front face:  for lighting you must specify a normal for each face!
  glNormal3f(0, 0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3f(-0.5, 0.5, 0.5); //v5
  glVertex3f(0.5, 0.5, 0.5); //v6
  glVertex3f(0.5, -0.5, 0.5); //v7
  glVertex3f(-0.5, -0.5, 0.5); //v8
  glEnd();
  
  //back face:  for lighting you must specify a normal for each face!
  glNormal3f(0, 0, -1.0);
  glBegin(GL_POLYGON);
  glVertex3f(-0.5, -0.5, -0.5); //v1
  glVertex3f(0.5, -0.5, -0.5); //v2
  glVertex3f(0.5, 0.5, -0.5); //v3
  glVertex3f(-0.5, 0.5, -0.5); //v4
  glEnd();

}

/* Main display function */
void Draw (void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				
	
	glLoadIdentity ();													

	// Position The Camera To Look At Our Mesh From A Distance
	gluLookAt(0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0, 1, 0);

	glRotatef(45, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	
	DrawCube();
													
	glutSwapBuffers();
}

/* Reshape */
void ReshapeGL (int width, int height)								
{
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				
	glMatrixMode (GL_PROJECTION);										
	glLoadIdentity ();													
	gluPerspective (60.0f, (GLfloat)(width)/(GLfloat)(height),			
					0.1f, 5.0f);		
	glMatrixMode (GL_MODELVIEW);										
	glLoadIdentity ();													

}


// glut keyboard
void Key(unsigned char key,int x,int y)
{
	Update(key,x,y);
	return ;
};


int main( int argc, char *argv[] )
{
   glutInit( &argc, argv );
   glutInitWindowPosition( 20, 20 );
   glutInitWindowSize( 400, 400 );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   glutCreateWindow("diffuse shader with no openGL lights??");
   glutReshapeFunc( ReshapeGL );
   glutKeyboardFunc( Key );
   glutSpecialFunc( Update );
   glutDisplayFunc( Draw );
   Initialize();
	
	//test the openGL version
	getGLversion();
	//install the shader
	if (!InstallShader(textFileRead("GLSL_Diffuse.glsl"))) {
		printf("Error installing shader!\n");
		return 0;
	}
		
   glutMainLoop();
   return 0;
}
