#include "collision.h"
#include "types.h"
#include <math.h>

bbody::bbody(){
	vala = valb = valc = vald = 0;
	_type = 0;
}

//basic fill in the blank constructor
bbody::bbody(const double ina, const double inb, const double inc, const double ind, const int intyp){
	vala = ina; valb = inb; valc = inc; vald = ind; _type = intyp;}
	//these are meant for AABBs and lines
bbody::bbody(coord2d_t ina, coord2d_t inb, int intyp){
	if(intyp == BB_AABB){
		VMINX = ina.x();
		VMINZ = ina.y();
		VMAXX = inb.x();
		VMAXZ = inb.y();
	}
	else if(intyp == BB_CIRC){
		VCENX = ina.x();
		VCENZ = ina.y();
		VRAD = inb.x();
		vald = 0;
	}
	else{
		double tempa = inb.y() - ina.y();//ina.y() - inb.y();
		double tempb = inb.x() - ina.x();
		VACO = tempa/hypot(tempb,tempa);
		VBCO = tempb/hypot(tempb,tempa);
		VCCO = -(ina.x()*VACO - ina.y()*VBCO);//eq of line: ax+by+c=0  +
		VDCO = -(ina.x()*VBCO + ina.y()*VACO);//eq of perp line: bx-ay+d=0   -
	}
	_type = intyp;
}
bbody::bbody(vec3d_t ina, vec3d_t inb, int intyp){
	if(intyp == BB_AABB){
		VMINX = ina.x();
		VMINZ = ina.z();
		VMAXX = inb.x();
		VMAXZ = inb.z();
	}
	else if(intyp == BB_CIRC){
		VCENX = ina.x();
		VCENZ = ina.z();
		VRAD = inb.x();
		vald = 0;
	}
	else{
		double tempa = ina.z() - inb.z();
		double tempb = inb.x() - ina.x();
		VACO = tempa/hypot(tempb,tempa);
		VBCO = tempb/hypot(tempb,tempa);
		VCCO = -(ina.x()*VACO + ina.z()*VBCO);//eq of line: ax+by+c=0
		VDCO = -(ina.x()*VBCO - ina.z()*VACO);//eq of perp line: bx-ay+d=0
	}
	_type = intyp;
}
	//these are meant for circles
bbody::bbody(coord2d_t incor, const double inr, int intyp){
	vala=incor.x(); valb=incor.y(); valc=inr; vald=0; _type=intyp;}
bbody::bbody(vec3d_t invec, const double inr, int intyp){
	vala=invec.x(); valb=invec.z(); valc=inr; vald=0; _type=intyp;}
	//all constructors work for all types

/* returns true if two circles collide, false otherwise*/
bool circlecollide(bbody circle1, bbody circle2) {
  double distance = 0;
  /*compute distance*/
  distance = sqrt(pow(circle1.VCENX-circle2.VCENX, 2) 
    + pow(circle1.VCENZ-circle2.VCENZ, 2));

  /*compare*/
  if (distance < circle1.VRAD + circle2.VRAD) {
    return true;
  }
  return false;
}

/* returns true if circle collides with point, false otherwise*/
bool circleptcollide(bbody circle1, coord2d_t pt) {
  double distance = 0;
  /*compute distance*/
  distance = sqrt(pow(circle1.VCENX-pt.x(), 2) 
    + pow(circle1.VCENZ-pt.y(), 2));
    
  /*compare*/
  if (distance < circle1.VRAD) {
    return true;
  }  
  return false;

}

/* returns true if two AABB's collide, false otherwise */
bool AABBcollide(bbody box1, bbody box2) {
  
  /* test corners */  
  if (box1.VMINX < box2.VMINX &&  box2.VMINX < box1.VMAXX &&
      box1.VMINZ < box2.VMINZ &&  box2.VMINZ < box1.VMAXZ ) {

      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.VMINX < box2.VMAXX &&  box2.VMAXX < box1.VMAXX &&
      box1.VMINZ < box2.VMAXZ &&  box2.VMAXZ < box1.VMAXZ ) {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  /* test if box is fully within box */
  if (box1.VMINX > box2.VMAXX ||
      box2.VMINX > box1.VMAXX )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.VMINZ > box2.VMAXZ ||
      box2.VMINZ > box1.VMAXZ )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  return false;
}

bool AABBptcollide(bbody box1, coord2d_t pt) {

  /* test pt */  
  if (box1.VMINX < pt.x() &&  pt.x() < box1.VMAXX &&
      box1.VMINZ < pt.y() &&  pt.y() < box1.VMAXZ ) {

	/* Means collision */
    return true;
  }
  return false;

}

bool circleAABBcollide(bbody cir, bbody box){
	coord2d_t pt(box.VMINX,box.VMINZ);
	if(circleptcollide(cir,pt)) return true;
	pt = coord2d_t(box.VMINX,box.VMAXZ);
	if(circleptcollide(cir,pt)) return true;
	pt = coord2d_t(box.VMAXX,box.VMINZ);
	if(circleptcollide(cir,pt)) return true;
	pt = coord2d_t(box.VMAXX,box.VMAXZ);
	if(circleptcollide(cir,pt)) return true;
	pt = coord2d_t(cir.VCENX,cir.VCENZ);
	return AABBptcollide(box,pt);
}

double ptonline(coord2d_t pt, double la, double lb, double lc){
	return pt.x()*la + pt.y()*lb + lc;
}

bool lineptcollide(bbody lin, coord2d_t pt){
	double d = ptonline(pt,lin.VACO,lin.VBCO,lin.VCCO);
	if(d>-.001&&d<.001){
		d = ptonline(pt,lin.VBCO,-lin.VACO,lin.VDCO);
		if(lin._type==BB_LINE) return (d>0&&d<LINELEN); //checks that pt is within specific range
		else return (d>0&&d<LLINELEN);				// and in right direction
	}
	return false;
}

bool lineAABBcollide(bbody lin, bbody box){
	bool ean;
	bool thrubox = false; //does line pass through box, true if at least one corner is on opp side of line from others
	bool inrange = false; //is box in within correct range, true if at least one corner is
	double d; double rang = (lin._type==BB_LINE)?LINELEN:LLINELEN;
	coord2d_t pt(box.VMINX,box.VMINZ);
	ean = (ptonline(pt,lin.VACO,lin.VBCO,lin.VCCO)>0);
	d = ptonline(pt,lin.VBCO,-lin.VACO,lin.VDCO);
	inrange |= (d>0&&d<rang);
	pt = coord2d_t(box.VMINX,box.VMAXZ);
	thrubox = (ean != (ptonline(pt,lin.VACO,lin.VBCO,lin.VCCO)>0));
	d = ptonline(pt,lin.VBCO,-lin.VACO,lin.VDCO);
	inrange |= (d>0&&d<rang);
	if(thrubox&&inrange) return true;
	pt = coord2d_t(box.VMAXX,box.VMINZ);
	thrubox |= (ean != (ptonline(pt,lin.VACO,lin.VBCO,lin.VCCO)>0));
	d = ptonline(pt,lin.VBCO,-lin.VACO,lin.VDCO);
	inrange |= (d>0&&d<rang);
	if(thrubox&&inrange) return true;
	pt = coord2d_t(box.VMAXX,box.VMAXZ);
	thrubox |= (ean != (ptonline(pt,lin.VACO,lin.VBCO,lin.VCCO)>0));
	d = ptonline(pt,lin.VBCO,-lin.VACO,lin.VDCO);
	inrange |= (d>0&&d<rang);
	return thrubox && inrange;
}

bool linecirclecollide(bbody lin, bbody cir){
	coord2d_t pt = coord2d_t(cir.VCENX,cir.VCENZ);
	double d = ptonline(pt,lin.VACO,lin.VBCO,lin.VCCO);
	if(d<cir.VRAD&&d>-cir.VRAD){
		d = ptonline(pt,lin.VBCO,-lin.VACO,lin.VDCO);
		if(lin._type==BB_LINE) return (d<(cir.VRAD+LINELEN)) && (d>-cir.VRAD);
		else return (d<(cir.VRAD+LLINELEN)) && (d>-cir.VRAD);
	}
	return false;
}

bool collide(bbody bod1, bbody bod2){
	if(bod1._type==BB_AABB){
		if(bod2._type==BB_AABB) return AABBcollide(bod1,bod2);
		else if(bod2._type==BB_CIRC) {
			if(bod2.vald == -1) return false;
			return circleAABBcollide(bod2,bod1);
		}
		else return lineAABBcollide(bod2,bod1);
	}
	else if(bod1._type==BB_CIRC){
		if(bod1.vald == -1) return false;
		else if(bod2._type==BB_AABB) return circleAABBcollide(bod1,bod2);
		else if(bod2._type==BB_CIRC) {
			if(bod2.vald == -1) return false;
			return circlecollide(bod1,bod2);
		}
		else return linecirclecollide(bod2,bod1);
	}
	else{
		if(bod2._type==BB_AABB) return lineAABBcollide(bod1,bod2);
		else if(bod2._type==BB_CIRC) {
			if(bod2.vald == -1) return false;
			return linecirclecollide(bod1,bod2);
		}
		else return true; //don't have a function for line to line collision. Will write one if we need one.
	}
}

bool ptcollide(bbody bod, coord2d_t pt){
	if(bod._type==BB_AABB) return AABBptcollide(bod,pt);
	else if(bod._type==BB_CIRC) {
		if(bod.vald == -1) return false;
		else return circleptcollide(bod,pt);
	}
	else return lineptcollide(bod,pt);
}