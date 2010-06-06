#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "objloader.h"
#include <ctype.h>

/* Vectors */
typedef float vec3_t[3];
typedef float vec4_t[4];
int hasname = 0;

/*** An OBJ model ***/
//struct obj_model_t objfile;

char test[256];







/* Vertex */
struct obj_vertex_t
{
  vec4_t xyzw;
};

/* Texture coordinates */
struct obj_texCoord_t
{
  vec3_t uvw;
};

/* Normal vector */
struct obj_normal_t
{
  vec3_t ijk;
};

/* Polygon */
struct obj_face_t
{
  GLenum type;        /* primitive type */
  int num_elems;      /* number of vertices */
  char mat[100];		/* material name*/
  int matindex;          
  int *vert_indices;  /* vertex indices */
  int *uvw_indices;   /* texture coordinate indices */
  int *norm_indices;  /* normal vector indices */
};






/**
 * Free resources allocated for the model.
 */
void
FreeModel (struct obj_model_t *mdl)
{
  int i;

  if (mdl)
    {
      if (mdl->vertices)
	{
	  free (mdl->vertices);
	  mdl->vertices = NULL;
	}

      if (mdl->texCoords)
	{
	  free (mdl->texCoords);
	  mdl->texCoords = NULL;
	}

      if (mdl->normals)
	{
	  free (mdl->normals);
	  mdl->normals = NULL;
	}

      if (mdl->faces)
	{
	  for (i = 0; i < mdl->num_faces; ++i)
	    {
	      if (mdl->faces[i].vert_indices)
		free (mdl->faces[i].vert_indices);

	      if (mdl->faces[i].uvw_indices)
		free (mdl->faces[i].uvw_indices);

	      if (mdl->faces[i].norm_indices)
		free (mdl->faces[i].norm_indices);
	    }

	  free (mdl->faces);
	  mdl->faces = NULL;
	}
    }
}

/**
 * Allocate resources for the model after first pass.
 */
int
MallocModel (struct obj_model_t *mdl)
{
  if (mdl->num_verts)
    {
      mdl->vertices = (struct obj_vertex_t *)
	malloc (sizeof (struct obj_vertex_t) * mdl->num_verts);
      if (!mdl->vertices)
	return 0;
    }

  if (mdl->num_texCoords)
    {
      mdl->texCoords = (struct obj_texCoord_t *)
	malloc (sizeof (struct obj_texCoord_t) * mdl->num_texCoords);
      if (!mdl->texCoords)
	return 0;
    }

  if (mdl->num_normals)
    {
      mdl->normals = (struct obj_normal_t *)
	malloc (sizeof (struct obj_normal_t) * mdl->num_normals);
      if (!mdl->normals)
	return 0;
    }

  if (mdl->num_faces)
    {
      mdl->faces = (struct obj_face_t *)
	calloc (mdl->num_faces, sizeof (struct obj_face_t));
      if (!mdl->faces)
	return 0;
    }

  return 1;
}

/**
 * Load an OBJ model from file -- first pass.
 * Get the number of triangles/vertices/texture coords for
 * allocating buffers.
 */
int
FirstPass (FILE *fp, struct obj_model_t *mdl)
{
  int v, t, n, i;
  char buf[256];

  while (!feof (fp))
    {
      /* Read whole line */
      fgets (buf, sizeof (buf), fp);

      switch (buf[0])
	{
	case 'm':
	  {
		  /*i=0;
		  while(buf[i] != ' '){
		    i++;
			  
		  }*/
		  sscanf(buf/*+i*/, "%*s %s", test);//mdl->name->name); 
		  hasname=1;
		  break;
	  }
	case 'v':
	  {
	    if (buf[1] == ' ')
	      {
		/* Vertex */
		mdl->num_verts++;
	      }
	    else if (buf[1] == 't')
	      {
		/* Texture coords. */
		mdl->num_texCoords++;
	      }
	    else if (buf[1] == 'n')
	      {
		/* Normal vector */
		mdl->num_normals++;
	      }
	    else
	      {
		printf ("Warning: unknown token \"%s\"! (ignoring)\n", buf);
	      }

	    break;
	  }

	case 'f':
	  {
	    /* Face */
	    if (sscanf (buf + 2, "%d/%d/%d", &v, &n, &t) == 3)
	      {
		mdl->num_faces++;
		mdl->has_texCoords = 1;
		mdl->has_normals = 1;
	      }
	    else if (sscanf (buf + 2, "%d//%d", &v, &n) == 2)
	      {
		mdl->num_faces++;
		mdl->has_texCoords = 0;
		mdl->has_normals = 1;
	      }
	    else if (sscanf (buf + 2, "%d/%d", &v, &t) == 2)
	      {
		mdl->num_faces++;
		mdl->has_texCoords = 1;
		mdl->has_normals = 0;
	      }
	    else if (sscanf (buf + 2, "%d", &v) == 1)
	      {
			  //printf("huh\n");
		mdl->num_faces++;
		mdl->has_texCoords = 0;
		mdl->has_normals = 0;
	      }
	    else
	      {
		/* Should never be there or the model is very crappy */
		fprintf (stderr, "Error: found face with no vertex!\n");
	      }

	    break;
	  }

	case 'g':
	  {
	    /* Group */
	    /*	fscanf (fp, "%s", buf); */
	    break;
	  }

	default:
	  break;
	}
    }

  /* Check if informations are valid */
  if ((mdl->has_texCoords && !mdl->num_texCoords) ||
      (mdl->has_normals && !mdl->num_normals))
    {
      fprintf (stderr, "error: contradiction between collected info!\n");
      return 0;
    }

  if (!mdl->num_verts)
    {
      fprintf (stderr, "error: no vertex found!\n");
      return 0;
    }

  printf ("first pass results: found\n");
  printf ("   * %i vertices\n", mdl->num_verts);
  printf ("   * %i texture coords.\n", mdl->num_texCoords);
  printf ("   * %i normal vectors\n", mdl->num_normals);
  printf ("   * %i faces\n", mdl->num_faces);
  printf ("   * has texture coords.: %s\n", mdl->has_texCoords ? "yes" : "no");
  printf ("   * has normals: %s\n", mdl->has_normals ? "yes" : "no"); 
  if (hasname){
	  printf ("   * has mtl file named: %s\n", test);
  }
  //mdl->name->name

  return 1;
}

/**
 * Load an OBJ model from file -- first pass.
 * This time, read model data and feed buffers.
 */
int
SecondPass (FILE *fp, struct obj_model_t *mdl)
{
  struct obj_vertex_t *pvert = mdl->vertices;
  struct obj_texCoord_t *puvw = mdl->texCoords;
  struct obj_normal_t *pnorm = mdl->normals;
  struct obj_face_t *pface = mdl->faces;
  char buf[128], *pbuf;
  int i;
  int index=0;

  while (!feof (fp))
    {
      /* Read whole line */
      fgets (buf, sizeof (buf), fp);

      switch (buf[0])
	{
	case 'v':
	  {
	    if (buf[1] == ' ')
	      {
		/* Vertex */
		if (sscanf (buf + 2, "%f %f %f %f",
			    &pvert->xyzw[0], &pvert->xyzw[1],
			    &pvert->xyzw[2], &pvert->xyzw[3]) != 4)
		  {
		    if (sscanf (buf + 2, "%f %f %f", &pvert->xyzw[0],
				&pvert->xyzw[1], &pvert->xyzw[2] ) != 3)
		      {
			fprintf (stderr, "Error reading vertex data!\n");
			return 0;
		      }
		    else
		      {
			pvert->xyzw[3] = 1.0;
		      }
		  }

		pvert++;
	      }
	    else if (buf[1] == 't')
	      {
		/* Texture coords. */
		if (sscanf (buf + 2, "%f %f %f", &puvw->uvw[0],
			    &puvw->uvw[1], &puvw->uvw[2]) != 3)
		  {
		    if (sscanf (buf + 2, "%f %f", &puvw->uvw[0],
				&puvw->uvw[1]) != 2)
		      {
			if (sscanf (buf + 2, "%f", &puvw->uvw[0]) != 1)
			  {
			    fprintf (stderr, "Error reading texture coordinates!\n");
			    return 0;
			  }
			else
			  {
			    puvw->uvw[1] = 0.0;
			    puvw->uvw[2] = 0.0;
			  }
		      }
		    else
		      {
			puvw->uvw[2] = 0.0;
		      }
		  }

		puvw++;
	      }
	    else if (buf[1] == 'n')
	      {
		/* Normal vector */
		if (sscanf (buf + 2, "%f %f %f", &pnorm->ijk[0],
			    &pnorm->ijk[1], &pnorm->ijk[2]) != 3)
		  {
		    fprintf (stderr, "Error reading normal vectors!\n");
		    return 0;
		  }

		pnorm++;
	      }

	    break;
	  }
	case 'u':
		{
			//pface->matindex = &pface;//((&mdl->faces) - (&pface));//(sizeof mdl->faces); 
			sscanf(buf, "%*s %s", &pface->mat);
		
			break;
		}
	case 'f':
	  {
	    pbuf = buf;
	    pface->num_elems = 0;

	    /* Count number of vertices for this face */
	    while (*pbuf)
	      {
			if (*pbuf == ' ')
			if(isalnum(*(pbuf+1))) {
				pface->num_elems++;
			}

			pbuf++;
	      }

	    /* Select primitive type */
	    if (pface->num_elems < 3)
	      {
		fprintf (stderr, "Error: a face must have at least 3 vertices!\n");
		return 0;
	      }
	    else if (pface->num_elems == 3)
	      {
		pface->type = GL_TRIANGLES;
	      }
	    else if (pface->num_elems == 4)
	      {
		pface->type = GL_QUADS;
	      }
	    else
	      {
		pface->type = GL_POLYGON;
	      }

	    /* Memory allocation for vertices */
	    pface->vert_indices = (int *)malloc (sizeof (int) * pface->num_elems);

	    if (1/*mdl->has_texCoords*/)
	      pface->uvw_indices = (int *)malloc (sizeof (int) * pface->num_elems);

	    if (1/*mdl->has_normals*/)
	      pface->norm_indices = (int *)malloc (sizeof (int) * pface->num_elems);

	    /* Read face data */
	    pbuf = buf;
	    i = 0;

	    for (i = 0; i < pface->num_elems; ++i)
	      {
		pbuf = strchr (pbuf, ' ');
		pbuf++; /* Skip space */

	//	pface->vert_indices[i]=2;
	//	pface->uvw_indices[i]=2;
	//	pface->norm_indices[i]=2;

		/* Try reading vertices */
		if (sscanf_s (pbuf, "%d/%d/%d",
			    &pface->vert_indices[i],
			    &pface->uvw_indices[i],
			    &pface->norm_indices[i]) != 3)
		  {
		    if (sscanf_s (pbuf, "%d//%d", &pface->vert_indices[i],
				&pface->norm_indices[i]) != 2)
		      {
			if (sscanf_s (pbuf, "%d/%d", &pface->vert_indices[i],
				&pface->uvw_indices[i]) != 2)
			  {
			    sscanf (pbuf, "%d", &pface->vert_indices[i]);
			  }
		      }
		  }

		/* Indices must start at 0 */
		pface->vert_indices[i]--;

		if (mdl->has_texCoords)
		  pface->uvw_indices[i]--;

		if (mdl->has_normals)
		  pface->norm_indices[i]--;
	      }

	    pface++;
	    break;
	  }
	}
    }

  printf ("second pass results: read\n");
  printf ("   * %i vertices\n", pvert - mdl->vertices);
  printf ("   * %i texture coords.\n", puvw - mdl->texCoords);
  printf ("   * %i normal vectors\n", pnorm - mdl->normals);
  printf ("   * %i faces\n", pface - mdl->faces);
  //printf ("   * %s test name\n", mdl->faces[0].mat);
  //printf("wtf %d\n", mdl->num_faces);

  return 1;
}

/**
 * Load an OBJ model from file, in two passes.
 */
int
ReadOBJModel (const char *filename, struct obj_model_t *mdl)
{
  FILE *fp;

  fp = fopen (filename, "r");
  if (!fp)
    {
      fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
      return 0;
    }

  /* reset model data */
  memset (mdl, 0, sizeof (struct obj_model_t));

  /* first pass: read model info */
  if (!FirstPass (fp, mdl))
    {
      fclose (fp);
      return 0;
    }

  rewind (fp);

  /* memory allocation */
  if (!MallocModel (mdl))
    {
      fclose (fp);
      FreeModel (mdl);
      return 0;
    }

  /* second pass: read model data */
  if (!SecondPass (fp, mdl))
    {
      fclose (fp);
      FreeModel (mdl);
      return 0;
    }

  fclose (fp);
  return 1;
}


int mtlLoad (const char *filename, struct mtl_file *mtlfile, int materials) {
  FILE *fp;
  char buf[256];
  int i=-1;

  mtlfile->mtl = (struct mtl*)malloc(sizeof(struct mtl)*materials);
  mtlfile->materials = materials;

  fp = fopen (filename, "r");
  if (!fp)
    {
      fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
      return 0;
    }

  while (!feof (fp))
    {
      /* Read whole line */
      fgets (buf, sizeof (buf), fp);

      switch (buf[0])
	  {
	  case 'N':
		  if (buf[1] == 's') {
			  sscanf(buf+2, "%f", &mtlfile->mtl[i].ns);
		  }
		  if (buf[1] == 'i') {
			  sscanf(buf+2, "%f", &(mtlfile->mtl[i].ni));
		  }
		  break;
	  case 'n':
		  if (buf[1] == 'e') {
			  if (i<materials) {
				  i++;
			  }
			  sscanf(buf, "%*s %s", &mtlfile->mtl[i].name);
			  //sscanf(buf, "%s", &mtlfile->mtl[i].name);

		  }
		  break;
	  case 'K':
		  if (buf[1] == 'a') {
			  sscanf(buf+2, "%f %f %f", &mtlfile->mtl[i].ka[0], &mtlfile->mtl[i].ka[1], &mtlfile->mtl[i].ka[2]);
		  }
		  if (buf[1] == 's') {
			  sscanf(buf+2, "%f %f %f", &mtlfile->mtl[i].ks[0], &mtlfile->mtl[i].ks[1], &mtlfile->mtl[i].ks[2]);
		  }
		  if (buf[1] == 'd') {
			  sscanf(buf+2, "%f %f %f", &mtlfile->mtl[i].kd, &mtlfile->mtl[i].kd[1], &mtlfile->mtl[i].kd[2]);
		  }
		  break;
	  case 'i':
		  if (buf[1] == 'l') {
			  //sscanf(buf+2, "%*s %d", mtlfile->mtl[i].illum);
			  sscanf(buf+2, "%d", mtlfile->mtl[i].illum);
		  }
		  break;
	  case 'd':
		  if (buf[1] == 'l') {
			  sscanf(buf+2, "%f", mtlfile->mtl[i].d);
		  }
		  break;
	  }
  }
  /*printf ("mtl results: read\n");
  printf ("   * %s name\n", mtlfile->mtl[0].name);
  printf ("   * %f ns\n", mtlfile->mtl[0].ns);
  printf ("   * %f %f %f ka\n", mtlfile->mtl[3].ka[0], mtlfile->mtl[3].ka[1], mtlfile->mtl[3].ka[2]);
  printf ("   * %f %f %f kd\n", mtlfile->mtl[3].kd[0], mtlfile->mtl[3].kd[1], mtlfile->mtl[3].kd[2]);
  printf ("   * %f %f %f ks\n", mtlfile->mtl[3].ks[0], mtlfile->mtl[3].ks[1], mtlfile->mtl[3].ks[2]);
  printf ("   * %f ni\n", mtlfile->mtl[0].ni);*/
  //printf ("   * %i faces\n", pface - mdl->faces);

}

/**
 * Draw the OBJ model.
 */
void
RenderOBJModel (struct obj_model_t *mdl)
{
  int i, j;

  /*if (hasname) {
	  glEnable(GL_LIGHTING);
  }*/
  //printf("%d\n", mdl->num_faces);
  for (i = 0; i < mdl->num_faces; ++i)
  {

        
    glBegin (mdl->faces[i].type);
	for (j = 0; j < mdl->faces[i].num_elems; ++j)
	{

		/*printf("test4 %d\n", mdl->texCoords[mdl->faces[i].uvw_indices[j]].uvw);
		if (i==100) {
			printf("test4 %d\n", mdl->texCoords[mdl->faces[i].uvw_indices[j]].uvw);

		}*/
		/*printf("i=%d j=%d %d/", i, j, mdl->faces[i].vert_indices[j]);
		printf("%d/", mdl->faces[i].uvw_indices[j]);
		printf("%d\n", mdl->faces[i].norm_indices[j]);*/

	  if (mdl->has_texCoords) {
		  //printf("test2 %d\n", i);
		  //printf("test3 %d\n", mdl->texCoords[mdl->faces[i].uvw_indices[j]].uvw);
	      glTexCoord3fv (mdl->texCoords[mdl->faces[i].uvw_indices[j]].uvw);
		  //glTexCoord3fv (mdl->texCoords[mdl->faces[219].uvw_indices[3]].uvw);
      }
        if (mdl->has_normals){
		  //glNormal3fv (mdl->normals[mdl->faces[219].norm_indices[3]].ijk);
	      glNormal3fv (mdl->normals[mdl->faces[i].norm_indices[j]].ijk);
        
        }
	    glVertex4fv (mdl->vertices [mdl->faces[i].vert_indices[j]].xyzw);
	}
	glEnd();
    }
	/*if (hasname) {
	  glDisable(GL_LIGHTING);
    }*/

}


void
RenderOBJModelt (struct obj_model_t *mdl, struct mtl_file *mtlfile)
{
  int i, j, k;

  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess[1];

  if (hasname) {
	  glEnable(GL_LIGHTING);
  }
  //printf("%d\n", mdl->num_faces);
  for (i = 0; i < mdl->num_faces; ++i)
  {
	  if(isalpha(mdl->faces[i].mat[0])) {

	  ///printf("====== material name %s =====\n", mdl->faces[i].mat);
	  
	//if(mdl->faces[i].matindex+1 == i) {
		for(k=0; k<mtlfile->materials;k++) {
			if(!strcmp(mtlfile->mtl[k].name, mdl->faces[i].mat)){
				//printf("====== material name %s =====\n", mtlfile->mtl[k].name);
				ambient[0] = mtlfile->mtl[k].ka[0];
				ambient[1] = mtlfile->mtl[k].ka[1];
				ambient[2] = mtlfile->mtl[k].ka[2];
				ambient[3] = mtlfile->mtl[k].ni;

				diffuse[0] = mtlfile->mtl[k].kd[0];
				diffuse[1] = mtlfile->mtl[k].kd[1];
				diffuse[2] = mtlfile->mtl[k].kd[2];
				diffuse[3] = mtlfile->mtl[k].ni;

				specular[0] = mtlfile->mtl[k].ks[0];
				specular[1] = mtlfile->mtl[k].ks[1];
				specular[2] = mtlfile->mtl[k].ks[2];
				specular[3] = mtlfile->mtl[k].ni;

				shininess[0] = mtlfile->mtl[k].d;
				glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
				glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

			}
		}
	  }
		

	//}
        
    glBegin (mdl->faces[i].type);
	for (j = 0; j < mdl->faces[i].num_elems; ++j)
	{
		/*printf("test4 %d\n", mdl->texCoords[mdl->faces[i].uvw_indices[j]].uvw);
		if (i==100) {
			printf("test4 %d\n", mdl->texCoords[mdl->faces[i].uvw_indices[j]].uvw);

		}*/
		/*printf("i=%d j=%d %d/", i, j, mdl->faces[i].vert_indices[j]);
		printf("%d/", mdl->faces[i].uvw_indices[j]);
		printf("%d\n", mdl->faces[i].norm_indices[j]);*/

	  if (mdl->has_texCoords) {
		  //printf("test2 %d\n", i);
		  //printf("test3 %d\n", mdl->texCoords[mdl->faces[i].uvw_indices[j]].uvw);
	      glTexCoord3fv (mdl->texCoords[mdl->faces[i].uvw_indices[j]].uvw);
		  //glTexCoord3fv (mdl->texCoords[mdl->faces[219].uvw_indices[3]].uvw);
      }
        if (mdl->has_normals){
		  //glNormal3fv (mdl->normals[mdl->faces[219].norm_indices[3]].ijk);
	      glNormal3fv (mdl->normals[mdl->faces[i].norm_indices[j]].ijk);
        
        }
	    glVertex4fv (mdl->vertices [mdl->faces[i].vert_indices[j]].xyzw);
	}
	glEnd();
    }
	glDisable(GL_COLOR_MATERIAL);

	//if (hasname) {
	  glDisable(GL_LIGHTING);
    //}
  /*printf ("mtl results 222222222222: read\n");
  printf ("   * %s name\n", mtlfile->mtl[0].name);
  printf ("   * %f ns\n", mtlfile->mtl[0].ns);
  printf ("   * %f %f %f ka\n", mtlfile->mtl[0].ka[0], mtlfile->mtl[0].ka[1], mtlfile->mtl[0].ka[2]);
  printf ("   * %f %f %f kd\n", mtlfile->mtl[0].kd[0], mtlfile->mtl[0].kd[1], mtlfile->mtl[0].kd[2]);
  printf ("   * %f %f %f ks\n", mtlfile->mtl[0].ks[0], mtlfile->mtl[0].ks[1], mtlfile->mtl[0].ks[2]);
  printf ("   * %f ni\n", mtlfile->mtl[0].ni);*/
}

void
init (const char *filename, obj_model_t *objfile)
{
  GLfloat lightpos[] = { 5.0f, 10.0f, 0.0f, 1.0f };

  /* Initialize OpenGL context */
  glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  glLightfv (GL_LIGHT0, GL_POSITION, lightpos);

  /* Load OBJ model file */
  if (!ReadOBJModel (filename, objfile))
	 
    exit (EXIT_FAILURE);
}

void
cleanup (obj_model_t objfile)
{
  FreeModel (&objfile);
}

/*void
reshape (int w, int h)
{
  if (h == 0)
    h = 1;

  glViewport (0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0, w/(GLdouble)h, 0.1, 1000.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glutPostRedisplay ();
}*/

/*void
display ()
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  glTranslatef (0.0f, 0.0f, -10.0f);

  /* Draw the model */
  /*RenderOBJModel (&objfile);

  glutSwapBuffers ();
}*/

//void
/*keyboard (unsigned char key, int x, int y)
{
  // Escape 
  if (key == 27)
    exit (0);
}*/

/*int
main (int argc, char *argv[])
{
  if (argc < 2)
    {
      fprintf (stderr, "usage: %s <filename.obj>\n", argv[0]);
      return -1;
    }

  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize (640, 480);
  glutCreateWindow ("OBJ Model");

  atexit (cleanup);
  init ("palmTree.obj");

  glutReshapeFunc (reshape);
  glutDisplayFunc (display);
  glutKeyboardFunc (keyboard);

  glutMainLoop ();

  return 0;
}*/

