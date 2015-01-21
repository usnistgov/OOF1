// -*- C++ -*-
// $RCSfile: meshcoord.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2001-02-16 14:53:54 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef MESHCOORD_H
#define MESHCOORD_H

#include <iostream.h>
#include <math.h>

class MeshCoord;
class Cell_coordinate;

inline double dot(const MeshCoord&, const MeshCoord&);

class MeshCoord {
public:
  double x;
  double y;
  MeshCoord(void)
    {
      x = 0;
      y = 0;
    }
  MeshCoord(const double X, const double Y)
    : x(X), y(Y)
      {
    }
  MeshCoord &operator*=(double a) {
    x *= a;
    y *= a;
    return *this;
  }
  MeshCoord &operator/=(double a) {
    x /= a;
    y /= a;
    return *this;
  }
  MeshCoord &operator+=(const MeshCoord &a) {
    x += a.x;
    y += a.y;
    return *this;
  }
  MeshCoord &operator-=(const MeshCoord &a) {
    x -= a.x;
    y -= a.y;
    return *this;
  }
  double norm() const {
    return sqrt(dot(*this, *this));
  }
};

inline MeshCoord operator-(const MeshCoord &a, const MeshCoord &b)
{
  MeshCoord result(a);
  return result -= b;
}

inline MeshCoord operator+(const MeshCoord &a, const MeshCoord &b)
{
  MeshCoord result(a);
  return result += b;
}

inline MeshCoord operator*(const double a, const MeshCoord &b)
{
  MeshCoord result(b);
  return result *= a;
}

inline MeshCoord operator*(const MeshCoord &b, const double a)
{
  MeshCoord result(b);
  return result *= a;
}

inline double cross(const MeshCoord &c1, const MeshCoord &c2)
{
  return c1.x*c2.y - c1.y*c2.x;
}

inline double dot(const MeshCoord &c1, const MeshCoord &c2) {
  return c1.x*c2.x + c1.y*c2.y;
}

inline double operator%(const MeshCoord &c1, const MeshCoord &c2)
{
  return(cross(c1,c2));
}

inline double trianglearea(const MeshCoord &v1,
			   const MeshCoord &v2,
			   const MeshCoord &v3)
{
  return 0.5*(v2 - v1) % (v3 - v1);
}

inline int operator==(const MeshCoord &c1, const MeshCoord &c2)
{
  return c1.x == c2.x && c1.y == c2.y;
}

inline double sq_distance(const MeshCoord &c1, const MeshCoord &c2)
{
  double dx = c1.x - c2.x;
  double dy = c1.y - c2.y;
  return(dx*dx + dy*dy);
}

bool operator>(const MeshCoord&, const MeshCoord&);
bool operator<(const MeshCoord&, const MeshCoord&);

Cell_coordinate mesh2cell_dn(const MeshCoord&);	// rounds down
Cell_coordinate mesh2cell_up(const MeshCoord&);	// rounds up
MeshCoord cell2mesh(const Cell_coordinate&);

ostream &operator<<(ostream &, const MeshCoord&);
istream &operator>>(istream &, MeshCoord&);

#endif /* MESHCOORD */

