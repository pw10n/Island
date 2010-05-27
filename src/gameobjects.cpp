#include "gameobjects.h"
#include "util.h"
#include "objloader.h"

#include <GL/glut.h>

void goCrate::draw(){
	cerr <<" DEBUG: Drawing a crate..." << endl;
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// it'll be 2x2x2 for now
	glBindTexture(GL_TEXTURE_2D, _tid);
	// "front"
	// if(cull(_pos)) continue;
	glPushMatrix();
	glTranslatef(_pos.x(),0,-_pos.y());
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (-.5, 0.0, 0.5);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (0.5, 0.0, 0.5);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (0.5, 1.0, 0.5);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (-.5, 1.0, 0.5);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, _tid);
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (0.5, 0.0, -.5);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (0.5, 0.0, 0.5);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (0.5, 1.0, 0.5);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (0.5, 1.0, -.5);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, ((goCrate*)crates[i])->texture);
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (-.5, 1.0, -.5);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (0.5, 1.0, -.5);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (0.5, 1.0, 0.5);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (-.5, 1.0, 0.5);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, ((goCrate*)&crates[i])->texture);
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (-.5, 0.0, 0.5);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (-.5, 0.0, -.5);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (-.5, 1.0, -.5);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (-.5, 1.0, 0.5);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, ((goCrate*)&crates[i])->texture);
	//back
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0);
	glVertex3f (-.5, 0.0, -0.5);
	glTexCoord2f (1.0, 0.0);
	glVertex3f (0.5, 0.0, -0.5);
	glTexCoord2f (1.0, 1.0);
	glVertex3f (0.5, 1.0, -0.5);
	glTexCoord2f (0.0, 1.0);
	glVertex3f (-.5, 1.0, -0.5);
	glEnd();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Hut::draw(){
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, _tid);

	glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());
		glScalef(.04, .04, .04);
		RenderOBJModel (mdl);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void palmTree::draw(){
	glDisable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glBindTexture(GL_TEXTURE_2D, _tid);

	glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());
		glScalef(1, 1, 1);
		RenderOBJModel (mdl);
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void rock::draw(){
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, _tid);

	glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());
		glScalef(.009, .009, .009);
		RenderOBJModel (mdl);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void rock2::draw(){
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, _tid);

	glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());
		glScalef(.009, .009, .009);
		RenderOBJModel (mdl);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

