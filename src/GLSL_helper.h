/*
 *  GLSL_helper.h
 *  CSC473
 *
 *  Many useful helper functions for GLSL shaders - gleaned from various sources including orange book
 *  Created by zwood on 2/21/10.
 *
 */
#pragma once
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;



int printOglError (const char *file, int line);

/* Print out the information log for a program object */
void printProgramInfoLog (GLuint program);

void printShaderInfoLog (GLuint shader);

#define printOpenGLError() printOglError(__FILE__, __LINE__)

//A helper routine to make it easier to set uniform variables in the shader
int getUniLoc(GLuint program, const char *name);

void getGLversion();

int textFileWrite(char *fn, char *s);
char *textFileRead(char *fn);
