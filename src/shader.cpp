/* Taken from: ZJ Wood for CSC 476 Spring 2007
 */

#include "shader.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <map>

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


/*Some multi-texturing stuff */
int  gbMultitexturing = 1;
GLfloat gOffset = 0.0f;

/*Global to access the shader program - used in keyboard to turn on and off */
int ShadeProg;

/* Code to set up using the GLSL shader */
int InstallShader(const char *vShaderText, const char *fShaderText) {
	GLuint VertVS, FragFS; //handles to shader object
	int vCompiled, fCompiled, linked; //status of shader
	
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