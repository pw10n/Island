#pragma once

#include <math.h>
#include <float.h>
#include <iostream>
#include <exception>
#include "stdint.h"


using namespace std;

union varies{
   uint32_t i32;
   uint16_t i16;
   uint8_t  i08;
};
   

class coord2i_t{
   int32_t _x;
   int32_t _y;

   public:
	   coord2i_t(): _x(0), _y(0) {};
	   coord2i_t(int32_t x, int32_t y): _x(x), _y(y) {};

	   coord2i_t operator=( const coord2i_t& );

	   int32_t x() const { return _x; }
	   int32_t& x() { return _x; }
	   int32_t y() const { return _y; }
	   int32_t& y() { return _y; }
};

class coord2d_t{
   double _x;
   double _y;

   public:
	   coord2d_t(): _x(0.0), _y(0.0) {};
	   coord2d_t(double x, double y): _x(x), _y(y) {};

	   coord2d_t operator=( const coord2d_t& );

	   double x() const { return _x; }
	   double& x() { return _x; }
	   double y() const { return _y; }
	   double& y() { return _y; }
};


///////////// ADAPTED FROM: Dr. Wood's Vector Library /////////////

class vec4d_t;

// taken from Zoe Wood's util.h 
/* A vector class for 3D vec3d_ts */
class vec3d_t{
public:
   vec3d_t( void );           // default constructor
   vec3d_t( vec3d_t* );       //alternative constructors 
    vec3d_t( const double x, const double y, const double z )
       : p_x( x ), p_y( y ), p_z( z ) {}
   vec3d_t( vec4d_t v4 );
   ~vec3d_t( void );          // *always* have a destructor

     /*assesors*/
     double  x( void ) const { return p_x; }
     double& x( void )       { return p_x; }
     double  y( void ) const { return p_y; }
     double& y( void )       { return p_y; }
     double  z( void ) const { return p_z; }
     double& z( void )       { return p_z; }
    void set(double x, double y, double z);
    void set(vec3d_t& v);

     /*operator overload for math simplicity */ 
     vec3d_t operator=( const vec3d_t& );
   
    bool operator==(const vec3d_t &v) const {
       if ( fabs(p_x - v.x()) < DBL_EPSILON ){
          if ( fabs(p_y - v.y()) < DBL_EPSILON ){
             if ( fabs(p_z - v.z()) < DBL_EPSILON ){
                return true;
             }
          }
       }
       return false;
    }

     vec3d_t operator+( const vec3d_t& v )
       { return vec3d_t( p_x + v.p_x, p_y + v.p_y, p_z + v.p_z); }
   
    vec3d_t operator-( const vec3d_t& v )
       { return vec3d_t( p_x - v.p_x, p_y - v.p_y, p_z - v.p_z); }
     
    double operator*( const vec3d_t& v )
       { return ( p_x*v.p_x + p_y*v.p_y + p_z*v.p_z); }
   
    vec3d_t operator*(const double f) 
       { return vec3d_t( p_x*f , p_y*f , p_z*f); }
    
   bool operator<( const vec3d_t& v) const{
      return (p_x < v.p_x && p_y < v.p_y && p_z < v.p_z);
   }
   
    /*useful to be able to print out using stream operators*/
    friend ostream& operator<<( ostream&, const vec3d_t& );
    friend istream& operator>>( istream&, vec3d_t& );

   void cross(const vec3d_t in, vec3d_t *out);
   double dot(const vec3d_t v2);
   double length();
   double norm();
   
   void from_vector4( vec4d_t v4 );
   

private:
     double p_x, p_y, p_z;
};

class vec4d_t{
public:
    vec4d_t( void );           // default constructor
    vec4d_t( vec4d_t* );
     vec4d_t( const double x, const double y, const double z , const double t)
         : p_x( x ), p_y( y ), p_z( z ), inter( t) {}
   vec4d_t( vec3d_t v3 ){
      p_x = v3.x();
      p_y = v3.y();
      p_z = v3.z();
      inter = 1.0;
   };
    ~vec4d_t( void );          // *always* have a destructor

     /*assesors*/
     double  x( void ) const { return p_x; }
     double& x( void )       { return p_x; }
     double  y( void ) const { return p_y; }
     double& y( void )       { return p_y; }
     double  z( void ) const { return p_z; }
     double& z( void )       { return p_z; }
     double  t( void ) const { return inter; }
     double& t( void )       { return inter; }
   
   void from_vector3( vec3d_t v3 );

     void set(double x, double y, double z, double t);
    
    /*useful to be able to print out using stream operators*/
     friend ostream& operator<<( ostream&, const vec4d_t& );

private:
     double p_x, p_y, p_z, inter;
};


/////////////////////////////////////////////////////////////////


class gException : public exception{
	string _msg;

public:
	gException(string msg) throw() : exception(){
		_msg = msg;
	}

	virtual const char* what() const throw()
	{
		return _msg.c_str();
	}
		
};

