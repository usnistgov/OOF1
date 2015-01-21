// -*- C++ -*-
// $RCSfile: cell_coordinate.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:25 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef CELL_COORDINATE
#define CELL_COORDINATE

class Cell_coordinate;

#include <iostream.h>
#include <X11/Xlib.h>

class Cell_coordinate : public XPoint {
public:

  Cell_coordinate(void) {
    x = 0;
    y = 0;
  }

  Cell_coordinate(const int X, const int Y) {
    x = X;
    y = Y;
  }

  int operator==(const Cell_coordinate &cc) const {
     return(cc.x == x && cc.y == y);
  }

  int operator!=(const Cell_coordinate &cc) const
  {
     return(cc.x != x || cc.y != y);
  }
};

int operator<(const Cell_coordinate&, const Cell_coordinate&);

inline Cell_coordinate operator+(const Cell_coordinate &a,
				 const Cell_coordinate &b)
{
  return Cell_coordinate(a.x + b.x, a.y + b.y);
}

inline Cell_coordinate operator-(const Cell_coordinate &a,
				 const Cell_coordinate &b)
{
  return Cell_coordinate(a.x - b.x, a.y - b.y);
}

ostream &operator<<(ostream&, const Cell_coordinate&);
istream &operator>>(istream&, Cell_coordinate&);

#endif








