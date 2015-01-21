// -*- C++ -*-
// $RCSfile: meshcoord.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-21 22:16:55 $


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
#include <stdio.h>

class MeshCoord;

class MeshCoord {
public:
  double x;
  double y;
  MeshCoord() : x(0.0), y(0.0) {}
  MeshCoord(double X, double Y) : x(X), y(Y) {}
};


inline int operator==(const MeshCoord &c1, const MeshCoord &c2)
{
  return c1.x == c2.x && c1.y == c2.y;
}

ostream &operator<<(ostream &, const MeshCoord&);
istream &operator>>(istream &, MeshCoord&);

#endif /* MESHCOORD */

