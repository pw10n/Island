#pragma once

#include "types.h"
#include <math.h>

/*axis alligned bounding box*/
struct AABB {
  coord2d_t mind;
  coord2d_t maxd;
  AABB(const double in_x, const double in_y, const double in_x2, const double in_y2) 
    : mind(in_x, in_y), maxd(in_x2, in_y2){}
  AABB(coord2d_t cen) : mind(cen.x()-.5,cen.y()-.5), maxd(cen.x()+.5,cen.y()+.5){}
  AABB() : mind(0,0), maxd(0,0){}
};

struct sphere {
  coord2d_t pos; /*position of center*/
  double radius;
  sphere(float in_radius, const double in_x, const double in_y)
    : radius(in_radius), pos(in_x, in_y){}
  sphere() : radius(0), pos(){}
};
/* returns true if two spheres collide, false otherwise*/
bool spherecollide(struct sphere sphere1, struct sphere sphere2);

/* returns true if sphere collides with point, false otherwise*/
bool sphereptcollide(struct sphere sphere1, coord2d_t pt);

/* returns true if two AABB's collide, false otherwise */
bool AABBcollide(struct AABB box1, struct AABB box2);

bool AABBptcollide(struct AABB box1, coord2d_t pt);

bool sphereAABBcollide(struct sphere sph, struct AABB box);
