/*
 *  GLSL_helper.h
 *  CSC473
 *
 *  Many useful helper functions for GLSL shaders - gleaned from various sources including orange book
 *  Created by zwood on 2/21/10.
 *
 */

#ifdef __APPLE__
#include "GLUT/glut.h"
#include <OPENGL/gl.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int printOglError (const char *file, int line);

/* Print out the information log for a program object */
void printProgramInfoLog (GLuint program);

void printShaderInfoLog (GLuint shader);

#define printOpenGLError() printOglError(__FILE__, __LINE__)

//A helper routine to make it easier to set uniform variables in the shader
GLint getUniLoc(GLuint program, const GLchar *name);

void getGLversion();

int textFileWrite(char *fn, char *s);
char *textFileRead(char *fn);
