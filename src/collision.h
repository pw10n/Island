#pragma once

#include "types.h"
#include <math.h>

#define BB_AABB 0
#define VMINX vala
#define VMINZ valb
#define VMAXX valc
#define VMAXZ vald

#define BB_CIRC 1
#define VCENX vala
#define VCENZ valb
#define VRAD valc
//circle bbodies don't use val[3] so it has no define
//instead, if val[3] of a circle = -1 then don't collide with anything
#define DONTCOLLIDE bbody(0,0,0,-1,BB_CIRC)

#define BB_LINE 2
#define VACO vala
#define VBCO valb
#define VCCO valc
#define VDCO vald
#define LINELEN .05 //this is assuming lines will have uniform length

//longer line than BB_LINE. use the same V* defines
#define BB_LLINE 3
#define LLINELEN 6.0

typedef struct bbody {
	double vala, valb, valc, vald; //uses the V* defines above for naming, based on the type
	uint8_t _type; //uses the BB_* defines above

	bbody();
	bbody(const double, const double, const double, const double, const int);
	bbody(coord2d_t, coord2d_t, int);
	bbody(vec3d_t, vec3d_t, int);
	bbody(coord2d_t, const double, int);
	bbody(vec3d_t, const double, int);
} bbody;

/* returns true if two circles collide, false otherwise*/
bool circlecollide(bbody, bbody);

/* returns true if circle collides with point, false otherwise*/
bool circleptcollide(bbody, coord2d_t);

/* returns true if two AABB's collide, false otherwise */
bool AABBcollide(bbody, bbody);

bool AABBptcollide(bbody, coord2d_t);

bool circleAABBcollide(bbody, bbody);

/* plugs the point into the line eq (doubles) and returns sum*/
double ptonline(coord2d_t, double, double, double);

bool lineptcollide(bbody, coord2d_t);

bool lineAABBcollide(bbody, bbody);

bool linecirclecollide(bbody, bbody);

/*master collide functions: main calls these, these call the above.*/
bool collide(bbody, bbody);

bool ptcollide(bbody, coord2d_t);
