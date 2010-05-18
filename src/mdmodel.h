#pragma once
#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "md5model.h"
#include "texture.h"

#define MAX_ANIMS 2

class mdmodel
{
   public:
   struct md5_model_t md5file;
   struct md5_anim_t md5anim[MAX_ANIMS];
   int animated; // = # of animations; 0 = no animations
   struct md5_joint_t *skeleton;
   int max_verts;
   int max_tris;
   int textNum;
   vec3_t *vertexArray;
   GLuint *vertexIndices;
   vec2_t *textureArray;
   vec3_t *normalArray;

   mdmodel(void);
   mdmodel(const char*, const char*, int);
   ~mdmodel(void);
   //void AllocVertexArrays(void);
   //void fillTextureArray(void);
   void PrepareMesh (const struct md5_mesh_t*, const struct md5_joint_t*);
   void draw(struct anim_info_t);
   int ReadMD5Model (const char*, struct md5_model_t*);
   int loadAnim(const char*);
};
