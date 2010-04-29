#include "collision.h"
#include "types.h"
#include <math.h>


/* returns true if two spheres collide, false otherwise*/
bool spherecollide(struct sphere sphere1, struct sphere sphere2) {
  double distance = 0;
  /*compute distance*/
  distance = sqrt(pow(sphere1.pos.x()-sphere2.pos.x(), 2) 
    + pow(sphere1.pos.y()-sphere2.pos.y(), 2));

  /*compare*/
  if (distance < sphere1.radius + sphere2.radius) {
    return true;
  }
  return false;
}

/* returns true if sphere collides with point, false otherwise*/
bool sphereptcollide(struct sphere sphere1, coord2d_t pt) {
  double distance = 0;
  /*compute distance*/
  distance = sqrt(pow(sphere1.pos.x()-pt.x(), 2) 
    + pow(sphere1.pos.y()-pt.y(), 2));
    
  /*compare*/
  if (distance < sphere1.radius) {
    return true;
  }  
  return false;

}

/* returns true if two AABB's collide, false otherwise */
bool AABBcollide(struct AABB box1, struct AABB box2) {
  
  /* test corners */  
  if (box1.mind.x() < box2.mind.x() &&  box2.mind.x() < box1.maxd.x() &&
      box1.mind.y() < box2.mind.y() &&  box2.mind.y() < box1.maxd.y() ) {

      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.mind.x() < box2.maxd.x() &&  box2.maxd.x() < box1.maxd.x() &&
      box1.mind.y() < box2.maxd.y() &&  box2.maxd.y() < box1.maxd.y() ) {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  /* test if box is fully within box */
  if (box1.mind.x() > box2.maxd.x() ||
      box2.mind.x() > box1.maxd.x() )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.mind.y() > box2.maxd.y() ||
      box2.mind.y() > box1.maxd.y() )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  return false;
}

bool AABBptcollide(struct AABB box1, coord2d_t pt) {

  /* test pt */  
  if (box1.mind.x() < pt.x() &&  pt.x() < box1.maxd.x() &&
      box1.mind.y() < pt.y() &&  pt.y() < box1.maxd.y() ) {

	/* Means collision */
    return true;
  }
  return false;

}

bool sphereAABBcollide(struct sphere sph, struct AABB box){
	coord2d_t pt(box.mind.x(),box.mind.y());
	if(sphereptcollide(sph,pt)) return true;
	pt = coord2d_t(box.mind.x(),box.maxd.y());
	if(sphereptcollide(sph,pt)) return true;
	pt = coord2d_t(box.maxd.x(),box.mind.y());
	if(sphereptcollide(sph,pt)) return true;
	pt = coord2d_t(box.maxd.x(),box.maxd.y());
	if(sphereptcollide(sph,pt)) return true;
	return AABBptcollide(box,sph.pos);
}