#include "crate.h"
#include "util.h"

#include <GL/glut.h>

void goCrate::draw(){
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
	//glBindTexture(GL_TEXTURE_2D, ((crate*)crates[i])->texture);
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
	//glBindTexture(GL_TEXTURE_2D, ((crate*)&crates[i])->texture);
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
	//glBindTexture(GL_TEXTURE_2D, ((crate*)&crates[i])->texture);
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