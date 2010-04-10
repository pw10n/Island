#pragma once

class coord2i_t{
   int _x;
   int _y;

   public:
   int x() const { return _x; }
   int& x() { return _x; }
   int y() const { return _y; }
   int& y() { return _y; }
};

class coord2d_t{
   double _x;
   double _y;

   public:
   double x() const { return _x; }
   double& x() { return _x; }
   double y() const { return _y; }
   double& y() { return _y; }
};



