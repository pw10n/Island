#ifndef _TEX_H_
#define _TEX_H_

#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>


#define glMultiTexCoord2f	glMultiTexCoord2fARB
#define glActiveTexture		glActiveTextureARB



using namespace std;

/*data structure for the image used for  texture mapping */
typedef struct Image {
  unsigned long sizeX;
  unsigned long sizeY;
  char *data;
} Image;

extern Image *TextureImage;

typedef struct RGB {
  GLubyte r;
  GLubyte g; 
  GLubyte b;
} RGB;

//RGB myimage[64][64];
extern RGB* pixel;

//forward declaration of image loading and texture set-up code
int ImageLoad(char *filename, Image *image);
GLvoid LoadTexture(char* image_file, int tex_id);
void init_tex();

#endif

