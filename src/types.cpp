#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "types.h"


using namespace std;

coord2i_t coord2i_t::operator=( const coord2i_t& c1) {
   this->_x = c1.x();
   this->_y = c1.y();
   return(*this );
}

coord2d_t coord2d_t::operator=( const coord2d_t& c1) {
   this->_x = c1.x();
   this->_y = c1.y();
   return(*this );
}

coord2d_t coord2d_t::operator*( const coord2d_t& c1) {
   this->_x *= c1.x();
   this->_y *= c1.y();
   return(*this );
}


/* vec3d_t constructors */
/* default */
vec3d_t::vec3d_t( void) {
    p_x = p_y = p_z = 0;
}

vec3d_t::vec3d_t(vec4d_t v4){
   
      p_x = v4.x();
      p_y = v4.y();
      p_z = v4.z();
   };
   

vec3d_t::vec3d_t( vec3d_t *v) {
   p_x = v->p_x; 
   p_y= v->p_y; 
   p_z= v->p_z;
}

void vec3d_t::set(double x, double y, double z) {
   this->p_x = x; 
   this->p_y = y; 
   this->p_z = z;
}

void vec3d_t::set(vec3d_t& v) {
   this->p_x = v.p_x; 
   this->p_y = v.p_y; 
   this->p_z = v.p_z;
}

vec3d_t vec3d_t::operator=( const vec3d_t& v1) {
   this->p_x = v1.x();
   this->p_y = v1.y();
   this->p_z = v1.z();
   return(*this );
}

void vec3d_t::cross(const vec3d_t in, vec3d_t *out) {
   out->p_x = (p_y*in.p_z - p_z*in.p_y); 
   out->p_y= -(p_x*in.p_z - p_z*in.p_x); 
   out->p_z= (p_x*in.p_y - p_y*in.p_x);
}

double vec3d_t::dot(const vec3d_t v2) {
   return(p_x*v2.p_x + p_y*v2.p_y + p_z*v2.p_z);
}

double vec3d_t::length(){
   return sqrt(p_x*p_x + p_y*p_y + p_z*p_z);
}

double vec3d_t::norm() {
   double leng = length();
   p_x = p_x/leng; 
   p_y= p_y/leng; 
   p_z= p_z/leng;
   return(leng);
}

void vec3d_t::from_vector4( vec4d_t v4 ){
   p_x = v4.x();
   p_y = v4.y();
   p_z = v4.z();
};

ostream& operator<<( ostream& os, const vec3d_t& v )
{
    cout << "vector x: " << v.p_x ;
    cout << " y: " << v.p_y ;
    cout << " z: " << v.p_z << endl;
    return(os);
}

/* destructor */
vec3d_t::~vec3d_t() {}

/* 4D vec3d_t constructors */
/* default */
vec4d_t::vec4d_t( void ) {
    p_x = p_y = p_z = inter = 0;
    }    

vec4d_t::vec4d_t( vec4d_t* v) {
    p_x = v->p_x;
    p_y= v->p_y;
    p_z= v->p_z;
    inter= v->inter;
}

void vec4d_t::set(double x, double y, double z, double t) {
    p_x = x;
    p_y= y;
    p_z= z;
   inter = t;
}

ostream& operator<<( ostream& os, const vec4d_t& v )
{
    cout << "vector x: " << v.p_x ;
    cout << " y: " << v.p_y ;
    cout << " z: " << v.p_z ;
    cout << " t: " << v.inter << endl;
    return(os);
}

void vec4d_t::from_vector3( vec3d_t v3 ){
   p_x = v3.x();
   p_y = v3.y();
   p_z = v3.z();
   inter = 1.0;
};

/* destructor */
vec4d_t::~vec4d_t( void ) {}

