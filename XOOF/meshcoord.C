// -*- C++ -*-
// $RCSfile: meshcoord.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:52 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "meshcoord.h"
#include "lookfor.h"

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

void MeshCoord::print(FILE *file) const {
  fprintf(file, "(%e, %e)", x, y);
}
