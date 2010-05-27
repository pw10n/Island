#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "texture.h"

#include <windows.h>
#include  <GL/glut.h>

using namespace std;


unsigned int BindTextureBMP(const char *fn, bool tile){
	FILE* file;
	char temp;
	long i;
	unsigned int texid;

	BITMAPINFOHEADER* infoheader = (BITMAPINFOHEADER*) malloc(sizeof(BITMAPINFOHEADER));
	char* data;
	
	cerr << "Loading texture '" << fn << "'..." << endl;

	if( !(file = fopen(fn, "rb")) ){
		cerr << "ERROR: file does not exist." << endl;
		return BindFailSafe(tile);
		//exit(-2);
	}

	fseek(file, 14, SEEK_CUR);
	fread(infoheader, sizeof(BITMAPINFOHEADER), 1, file);
	cerr << "biWidth: " << infoheader->biWidth << endl;
	cerr << "biHeight: " << infoheader->biHeight << endl;
	cerr << "biBitCount: " << infoheader->biBitCount << endl;

	if (infoheader->biBitCount != 24){
		cerr << "ERROR: Bpp is not 24." << endl;
		return BindFailSafe(tile);
		exit(-3);
	}
	
	if(!(data = (char*) malloc(infoheader->biWidth * infoheader->biHeight * 3))){
		cerr << "ERROR: malloc failed.";
		exit(-4);
	}

	if(!(i = fread(data, infoheader->biWidth * infoheader->biHeight * 3, 1, file))){
		cerr << "ERROR: can not read texture.";
		exit(-5);
	}

	//bgr -> rgb
	for(i=0; i<(infoheader->biWidth * infoheader->biHeight * 3); i+=3){
		temp = data[i];
		data[i] = data[i+2];
		data[i+2] = temp;
	}
	
	fclose(file);
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	if (tile == true) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, infoheader->biWidth, infoheader->biHeight,GL_RGB, GL_UNSIGNED_BYTE, data);

	//glTexImage2D(GL_TEXTURE_2D, 0, 3, infoheader->biWidth, infoheader->biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

//	free(infoheader);
//	free(data);

	return texid;
}


unsigned int BindFailSafe(bool tile){
	FILE* file;
	char temp;
	long i;
	unsigned int texid;
	char *fn = "textures\oops.bmp";

	BITMAPINFOHEADER* infoheader = (BITMAPINFOHEADER*) malloc(sizeof(BITMAPINFOHEADER));
	char* data;
	
	cerr << "Loading safety texture '" << fn << "'..." << endl;

	if( !(file = fopen(fn, "rb")) ){
		cerr << "ERROR: file does not exist." << endl;
		exit(-2);
	}

	fseek(file, 14, SEEK_CUR);
	fread(infoheader, sizeof(BITMAPINFOHEADER), 1, file);
	cerr << "biWidth: " << infoheader->biWidth << endl;
	cerr << "biHeight: " << infoheader->biHeight << endl;
	cerr << "biBitCount: " << infoheader->biBitCount << endl;

	if (infoheader->biBitCount != 24){
		cerr << "ERROR: Bpp is not 24." << endl;
		exit(-3);
	}
	
	if(!(data = (char*) malloc(infoheader->biWidth * infoheader->biHeight * 3))){
		cerr << "ERROR: malloc failed.";
		exit(-4);
	}

	if(!(i = fread(data, infoheader->biWidth * infoheader->biHeight * 3, 1, file))){
		cerr << "ERROR: can not read texture.";
		exit(-5);
	}

	//bgr -> rgb
	for(i=0; i<(infoheader->biWidth * infoheader->biHeight * 3); i+=3){
		temp = data[i];
		data[i] = data[i+2];
		data[i+2] = temp;
	}
	
	fclose(file);
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	if (tile == true) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, infoheader->biWidth, infoheader->biHeight,GL_RGB, GL_UNSIGNED_BYTE, data);

	//glTexImage2D(GL_TEXTURE_2D, 0, 3, infoheader->biWidth, infoheader->biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	free(infoheader);
	free(data);

	return texid;
}