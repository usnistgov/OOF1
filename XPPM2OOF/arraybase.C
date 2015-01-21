// -*- C++ -*-
// $RCSfile: arraybase.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-30 19:23:22 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Base class for Array templates, and other non-templated Array stuff

#include "array.h"
#include "cell_coordinate.h"
#include "ishape.h"

void ArrayBase::resize(const Cell_coordinate &size) {
  if(height == size.y && width == size.x) return;
  height = size.y;
  width = size.x;
  free();
  allocate();
}

ArrayIterator::ArrayIterator(const ImageRect &r, const ARI_Type flag)
  : i(r.ymin), j(r.xmin),
    x0(r.xmin), y0(r.ymin),
    x1(r.xmax), y1(r.ymax)
{
  if(flag == ARI_INCLUSIVE) { // limits are inclusive
    x1++;
    y1++;
  }
}

int ArrayIterator::operator()(Cell_coordinate &pxl) {
  if(j == x1) {
    j = x0;
    i++;
    if(i >= y1) return 0;	// ">" is required for 0x0 array!
  }
  pxl.x = j;
  pxl.y = i;
  j++;
  return 1;
}

void ArrayIterator::reset() {
  i = y0;
  j = x0;
}

int ArrayBase::contains(const Cell_coordinate &pxl) const {
  if(pxl.x < 0 || pxl.x >= width) return 0;
  if(pxl.y < 0 || pxl.y >= height) return 0;
  return 1;
}

