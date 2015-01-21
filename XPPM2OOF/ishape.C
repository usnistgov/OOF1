// -*- C++ -*-
// $RCSfile: ishape.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-09-06 20:11:37 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Shapes with integer coordinates (shapes defined on an image)

#include "cell_coordinate.h"
#include "ishape.h"
#include <math.h>

ImageRect::ImageRect(const Cell_coordinate &c1, const Cell_coordinate &c2) {
  if(c1.x < c2.x) {
    xmin = c1.x;
    xmax = c2.x;
  }
  else {
    xmin = c2.x;
    xmax = c1.x;
  }
  if(c1.y < c2.y) {
    ymin = c1.y;
    ymax = c2.y;
  }
  else {
    ymin = c2.y;
    ymax = c1.y;
  }
}

int ImageRect::contains(const Cell_coordinate &c) const {
  return (c.x >= xmin && c.x < xmax && c.y >= ymin && c.y < ymax);
}

static int min(int a, int b) { return a < b ? a : b; }
static int max(int a, int b) { return a > b ? a : b; }

ImageRect operator&&(const ImageRect &r1, const ImageRect &r2) {
  return ImageRect(Cell_coordinate(max(r1.xmin, r2.xmin),
				   max(r1.ymin, r2.ymin)),
		   Cell_coordinate(min(r1.xmax, r2.xmax),
				   min(r1.ymax, r2.ymax)));
}


ostream &operator<<(ostream &os, const ImageRect &rect) {
  os << "{{" << rect.xmin << ", " << rect.ymin << "}, {"
     << rect.xmax << ", " << rect.ymax << "}}";
  return os;
}
