#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

/* OBJ model structure */
struct obj_model_t
{
  int num_verts;                     /* number of vertices */
  int num_texCoords;                 /* number of texture coords. */
  int num_normals;                   /* number of normal vectors */
  int num_faces;                     /* number of polygons */

  int has_texCoords;                 /* has texture coordinates? */
  int has_normals;                   /* has normal vectors? */

  struct obj_vertex_t *vertices;     /* vertex list */
  struct obj_texCoord_t *texCoords;  /* tex. coord. list */
  struct obj_normal_t *normals;      /* normal vector list */
  struct obj_face_t *faces;          /* model's polygons */
};




void init (const char *filename, obj_model_t *objfile);
void cleanup (obj_model_t objfile);
void RenderOBJModel (struct obj_model_t *mdl);
void RenderOBJModelt (struct obj_model_t *mdl, int tex1, int tex2);
