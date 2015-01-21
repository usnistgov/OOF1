// -*- C++ -*-
// $RCSfile: cell_coordinate.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:47 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "cell_coordinate.h"
#include "lookfor.h"


int operator<(const Cell_coordinate &cell1, const Cell_coordinate &cell2) {
  if(cell1.x < cell2.x) return 1;
  if(cell1.x > cell2.x) return 0;
  if(cell1.y < cell2.y) return 1;
  return 0;
}

ostream &operator<<(ostream &os, const Cell_coordinate &c) {
    os << "[" << c.x << ", " << c.y << "]";
    return os;
}

istream &operator>>(istream &is, Cell_coordinate &cell) {
  if(!is)
    return is;
  if(!lookfor('[', is)) return is;
  is >> cell.x;
  if(!lookfor(',', is)) return is;
  is >> cell.y;
  (void) lookfor(']', is);
  if(!is)
    cerr << "Error reading Cell_coordinate!" << endl;
  return is;
}
