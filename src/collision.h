#include "types.h"
#include <math.h>

/*axis alligned bounding box*/
struct AABB {
  vec3d_t min;
  vec3d_t max;
  AABB(const double in_x, const double in_y, const double in_z, 
    const double in_x2, const double in_y2, const double in_z2) 
    : min(in_x, in_y, in_z), max(in_x2, in_y2, in_z2){}
  AABB() : min(0, 0, 0), max(0, 0, 0){}
  
};

struct sphere {
  vec3d_t pos; /*position of center*/
  double radius;
  sphere(float in_radius, const double in_x, const double in_y, const double in_z)
    : radius(in_radius), pos(in_x, in_y, in_z){}
  sphere() : radius(0), pos(0, 0, 0){}
};

/* returns true if two spheres collide, false otherwise*/
bool spherecollide(struct sphere sphere1, struct sphere sphere2) {
  double distance = 0;
  /*compute distance*/
  distance = sqrt(pow(sphere1.pos.x()-sphere2.pos.x(), 2) 
    + pow(sphere1.pos.y()-sphere2.pos.y(), 2) 
    + pow(sphere1.pos.z()-sphere2.pos.z(), 2));

  /*compare*/
  if (distance < sphere1.radius + sphere2.radius) {
    return true;
  }
  return false;
}

/* returns true if sphere collides with point, false otherwise*/
bool sphereptcollide(struct sphere sphere1, vec3d_t pt) {
  double distance = 0;
  /*compute distance*/
  distance = sqrt(pow(sphere1.pos.x()-pt.x(), 2) 
    + pow(sphere1.pos.y()-pt.y(), 2) 
    + pow(sphere1.pos.z()-pt.z(), 2));
    
  /*compare*/
  if (distance < sphere1.radius) {
    return true;
  }  
  return false;

}

/* returns true if two AABB's collide, false otherwise */
bool AABBcollide(struct AABB box1, struct AABB box2) {
  
  /* test corners */  
  if (box1.min.x() < box2.min.x() &&  box2.min.x() < box1.max.x() &&
      box1.min.y() < box2.min.y() &&  box2.min.y() < box1.max.y() &&
      box1.min.z() < box2.min.z() &&  box2.min.z() < box1.max.z() ) {

      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.min.x() < box2.max.x() &&  box2.max.x() < box1.max.x() &&
      box1.min.y() < box2.max.y() &&  box2.max.y() < box1.max.y() &&
      box1.min.z() < box2.max.z() &&  box2.max.z() < box1.max.z() ) {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  /* test if box is fully within box */
  if (box1.min.x() > box2.max.x() ||
      box2.min.x() > box1.max.x() )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.min.y() > box2.max.y() ||
      box2.min.y() > box1.max.y() )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  if (box1.min.z() > box2.max.z() ||
      box2.min.z() > box1.max.z() )  {
      /* do nothing (check next case) */
  }
  else {
    return true;
  }

  return false;
}

bool AABBptcollide(struct AABB box1, vec3d_t pt) {

  /* test pt */  
  if (box1.min.x() < pt.x() &&  pt.x() < box1.max.x() &&
      box1.min.y() < pt.y() &&  pt.y() < box1.max.y() &&
      box1.min.z() < pt.z() &&  pt.z() < box1.max.z() ) {

      /* do nothing (check next case) */
  }
  else {
	/* Means collision */
    return true;
  }
  return false;

}


