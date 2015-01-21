// -*- C++ -*-
// $RCSfile: meshcoord.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-30 18:32:42 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "cell_coordinate.h"
#include "meshcoord.h"
#include "lookfor.h"
#include <math.h>
#include <iostream.h>

bool operator<(const MeshCoord &a, const MeshCoord &b) {
  if(a.x == b.x)
    return a.y < b.y;
  return a.x < b.x;
}

bool operator>(const MeshCoord &a, const MeshCoord &b) {
  if(a.x == b.x)
    return a.y > b.y;
  return a.x > b.x;
}

ostream &operator<<(ostream &os, const MeshCoord &coord) {
  os << "(" << coord.x << ", " << coord.y << ")";
  return os;
}

istream &operator>>(istream &is, MeshCoord &coord) {
  if(!is)
    return is;
  if(!lookfor('(', is)) return is;
  is >> coord.x;
  if(!lookfor(',', is)) return is;
  is >> coord.y;
  (void) lookfor(')', is);
  if(!is)
    cerr << "Error reading MeshCoord!" << endl;
  return is;
}

Cell_coordinate mesh2cell_dn(const MeshCoord &m) {	// rounds down
  return Cell_coordinate(int(floor(m.x)), int(floor(m.y)));
}

Cell_coordinate mesh2cell_up(const MeshCoord &m) {	// rounds up
  return Cell_coordinate(int(ceil(m.x)), int(ceil(m.y)));
}

MeshCoord cell2mesh(const Cell_coordinate &c) {
  return MeshCoord(c.x + 0.5, c.y + 0.5);
}
