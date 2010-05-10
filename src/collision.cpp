#include "collision.h"
#include "types.h"
#include <math.h>

bbody::bbody(){
	val[0] = val[1] = val[2] = val[3] = 0;
	_type = 0;
}

//basic fill in the blank constructor
bbody::bbody(const double ina, const double inb, const double inc, const double ind, const int intyp){
	val[0] = ina; val[1] = inb; val[2] = inc; val[3] = ind; _type = intyp;}
	//this is meant for AABBs and lines
bbody::bbody(coord2d_t ina, coord2d_t inb, int intyp){
	if(intyp == BB_AABB){
		val[VMINX] = ina.x();
		val[VMINZ] = ina.y();
		val[VMAXX] = inb.x();
		val[VMAXZ] = inb.y();
	}
	else if(intyp == BB_CIRC){
		val[VCENX] = ina.x();
		val[VCENZ] = ina.y();
		val[VRAD] = inb.x();
		val[3] = 0;
	}
	else{
		double tempa = ina.y() - inb.y();
		double tempb = inb.x() - ina.x();
		val[VACO] = tempa/hypot(tempb,tempa);
		val[VBCO] = tempb/hypot(tempb,tempa);
		val[VCCO] = -(ina.x()*val[VACO] + ina.y()*val[VBCO]);//eq of line: ax+by+c=0
		val[VDCO] = -(ina.x()*val[VBCO] - ina.y()*val[VACO]);//eq of perp line: bx-ay+d=0
	}
	_type = intyp;
}
	//these are meant for circles
bbody::bbody(coord2d_t incor, const double inr, int intyp) {
	val[0]=incor.x(); val[1]=incor.y(); val[2]=inr; val[3]=0; _type=intyp;}
bbody::bbody(vec3d_t invec, const double inr, int intyp) {
	val[0]=invec.x(); val[1]=invec.z(); val[2]=inr; val[3]=0; _type=intyp;}
	//all constructors work for all types

/* returns true if two circles collide, false otherwise*/
bool circlecollide(bbody circle1, bbody circle2) {
  double distance = 0;
  /*compute distance*/
  distance = sqrt(pow(circle1.val[VCENX]-circle2.val[VCENX], 2) 
    + pow(circle1.val[VCENZ]-circle2.val[VCENZ], 2));

  /*compare*/
  if (distance < circle1.val[VRAD] + circle2.val[VRAD]) {
    return true;
  }
  return false;
}

/* returns true if circle collides with point, false otherwise*/
bool circleptcollide(bbody circle1, coord2d_t pt) {
  double distance = 0;
  /*compute distance*/
  distance = sqrt(pow(circle1.val[VCENX]-pt.x(), 2) 
    + pow(circle1.val[VCENZ]-pt.y(), 2));
    
  /*compare*/
  if (distance < circle1.val[VRAD]) {
    return true;
  }  
  return false;

}

/* returns true if two AABB's collide, false otherwise */
bool AABBcollide(bbody box1, bbody box2) {
  
  /* test corners */  
  if (box1.val[VMINX] < box2.val[VMINX] &&  box2.val[VMINX] < box1.val[VMAXX] &&
      box1.val[VMINZ] < box2.val[VMINZ] &&  box2.val[VMINZ] < box1.val[VMAXZ] ) {

      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.val[VMINX] < box2.val[VMAXX] &&  box2.val[VMAXX] < box1.val[VMAXX] &&
      box1.val[VMINZ] < box2.val[VMAXZ] &&  box2.val[VMAXZ] < box1.val[VMAXZ] ) {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  /* test if box is fully within box */
  if (box1.val[VMINX] > box2.val[VMAXX] ||
      box2.val[VMINX] > box1.val[VMAXX] )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.val[VMINZ] > box2.val[VMAXZ] ||
      box2.val[VMINZ] > box1.val[VMAXZ] )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  return false;
}

bool AABBptcollide(bbody box1, coord2d_t pt) {

  /* test pt */  
  if (box1.val[VMINX] < pt.x() &&  pt.x() < box1.val[VMAXX] &&
      box1.val[VMINZ] < pt.y() &&  pt.y() < box1.val[VMAXZ] ) {

	/* Means collision */
    return true;
  }
  return false;

}

bool circleAABBcollide(bbody cir, bbody box){
	coord2d_t pt(box.val[VMINX],box.val[VMINZ]);
	if(circleptcollide(cir,pt)) return true;
	pt = coord2d_t(box.val[VMINX],box.val[VMAXZ]);
	if(circleptcollide(cir,pt)) return true;
	pt = coord2d_t(box.val[VMAXX],box.val[VMINZ]);
	if(circleptcollide(cir,pt)) return true;
	pt = coord2d_t(box.val[VMAXX],box.val[VMAXZ]);
	if(circleptcollide(cir,pt)) return true;
	pt = coord2d_t(cir.val[VCENX],cir.val[VCENZ]);
	return AABBptcollide(box,pt);
}

double ptonline(coord2d_t pt, double la, double lb, double lc){
	return pt.x()*la + pt.y()*lb + lc;
}

bool lineptcollide(bbody lin, coord2d_t pt){
	double d = ptonline(pt,lin.val[VACO],lin.val[VBCO],lin.val[VCCO]);
	if(d>-.001&&d<.001){
		d = ptonline(pt,lin.val[VBCO],-lin.val[VACO],lin.val[VDCO]);
		return (d>0&&d<LINELEN); //checks that pt is within specific range and in right direction
	}
	return false;
}

bool lineAABBcollide(bbody lin, bbody box){
	bool ean;
	bool thrubox = false; //does line pass through box, true if at least one corner is on opp side of line from others
	bool inrange = false; //is box in within correct range, true if at least one corner is
	double d;
	coord2d_t pt(box.val[VMINX],box.val[VMINZ]);
	ean = (ptonline(pt,lin.val[VACO],lin.val[VBCO],lin.val[VCCO])>0);
	d = ptonline(pt,lin.val[VBCO],-lin.val[VACO],lin.val[VDCO]);
	inrange |= (d>0&&d<LINELEN);
	pt = coord2d_t(box.val[VMINX],box.val[VMAXZ]);
	thrubox = (ean != (ptonline(pt,lin.val[VACO],lin.val[VBCO],lin.val[VCCO])>0));
	d = ptonline(pt,lin.val[VBCO],-lin.val[VACO],lin.val[VDCO]);
	inrange |= (d>0&&d<LINELEN);
	if(thrubox&&inrange) return true;
	pt = coord2d_t(box.val[VMAXX],box.val[VMINZ]);
	thrubox |= (ean != (ptonline(pt,lin.val[VACO],lin.val[VBCO],lin.val[VCCO])>0));
	d = ptonline(pt,lin.val[VBCO],-lin.val[VACO],lin.val[VDCO]);
	inrange |= (d>0&&d<LINELEN);
	if(thrubox&&inrange) return true;
	pt = coord2d_t(box.val[VMAXX],box.val[VMAXZ]);
	thrubox |= (ean != (ptonline(pt,lin.val[VACO],lin.val[VBCO],lin.val[VCCO])>0));
	d = ptonline(pt,lin.val[VBCO],-lin.val[VACO],lin.val[VDCO]);
	inrange |= (d>0&&d<LINELEN);
	return thrubox && inrange;
}

bool linecirclecollide(bbody lin, bbody cir){
	coord2d_t pt = coord2d_t(cir.val[VCENX],cir.val[VCENZ]);
	double d = ptonline(pt,lin.val[VACO],lin.val[VBCO],lin.val[VCCO]);
	if(d<cir.val[VRAD]||d>-cir.val[VRAD]){
		d = ptonline(pt,lin.val[VBCO],-lin.val[VACO],lin.val[VDCO]);
		return d<(cir.val[VRAD]+LINELEN) || d>-cir.val[VRAD];
	}
	return false;
}

bool collide(bbody bod1, bbody bod2){
	if(bod1._type==BB_AABB){
		if(bod2._type==BB_AABB) return AABBcollide(bod1,bod2);
		else if(bod2._type==BB_CIRC) {
			if(bod2.val[3] == -1) return false;
			return circleAABBcollide(bod2,bod1);
		}
		else return lineAABBcollide(bod2,bod1);
	}
	else if(bod1._type==BB_CIRC){
		if(bod1.val[3] == -1) return false;
		else if(bod2._type==BB_AABB) return circleAABBcollide(bod1,bod2);
		else if(bod2._type==BB_CIRC) {
			if(bod2.val[3] == -1) return false;
			return circlecollide(bod1,bod2);
		}
		else return linecirclecollide(bod2,bod1);
	}
	else{
		if(bod2._type==BB_AABB) return lineAABBcollide(bod1,bod2);
		else if(bod2._type==BB_CIRC) {
			if(bod2.val[3] == -1) return false;
			return linecirclecollide(bod1,bod2);
		}
		else return true; //don't have a function for line to line collision. Will write one if we need one.
	}
}

bool ptcollide(bbody bod, coord2d_t pt){
	if(bod._type==BB_AABB) return AABBptcollide(bod,pt);
	else if(bod._type==BB_CIRC) {
		if(bod.val[3] == -1) return false;
		else return circleptcollide(bod,pt);
	}
	else return lineptcollide(bod,pt);
}