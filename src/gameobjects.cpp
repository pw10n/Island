#include "gameobjects.h"
#include "util.h"
#include "objloader.h"
#include <cstdlib>

#include <GL/glut.h>

void goCrate::draw(){ 
	float arr[4] = {1.0, 1.0, 1.0, 1.0};
	float arr2[1] = {1.0};
	//cerr <<" DEBUG: Drawing a crate..." << endl;
	glPushMatrix();
		  glMaterialfv(GL_FRONT, GL_AMBIENT, arr);
	  glMaterialfv(GL_FRONT, GL_DIFFUSE, arr);
	  glMaterialfv(GL_FRONT, GL_SPECULAR, arr);
	  glMaterialfv(GL_FRONT, GL_SHININESS, arr2);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
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
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	//glDisable(GL_LIGHTING);
	//glDisable(GL_TEXTURE_2D);
}

void Hut::draw(){
	/*glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, _tid);*/

	//glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());
		//glScalef(.04, .04, .04);
		//RenderOBJModel (mdl);
		glCallList(HUTLIST);
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
}

void palmTree::draw(){
	//glDisable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glBindTexture(GL_TEXTURE_2D, _tid);

	glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());
		//glScalef(1, 1, 1);
		//RenderOBJModel (mdl);
		glCallList(TREELIST);
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
}

void rock::draw(){
	/*glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, _tid);*/

	//glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());
		//glScalef(.009, .009, .009);
		glRotatef(angle, 0, 1, 0);
		//RenderOBJModel (mdl);
		glCallList(ROCKLIST);
	glPopMatrix();
	/*glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);*/
}

void rock2::draw(){
	/*glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, _tid);*/

	//glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());
		//glScalef(.009, .009, .009);
		glRotatef(angle, 0, 1, 0);
		//RenderOBJModel (mdl);
		glCallList(ROCK2LIST);
	glPopMatrix();
	/*glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);*/
}

void veg::draw(){

	glPushMatrix();
		glTranslatef(_pos.x(),0,-_pos.y());

		glRotatef(angle, 0, 1, 0);

		glCallList(VEGLIST);
	glPopMatrix();

}

