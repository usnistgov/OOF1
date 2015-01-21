// -*- C++ -*-
// $RCSfile: stencil.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Stencil applied when modifying images
// stencil(0, 0) is the center pixel
// stencil(-1, 0) is the pixel to the left, etc.

#ifndef STENCIL_H
#define STENCIL_H

class Stencil {
private:
  double *val;
  int size;
  double factor;
  int offset, yfact;		// used in operator()(int, int)
public:
  // create a symmetric 3x3 stencil. There are only 
  Stencil(double center, double side, double diagonal, double f)
    : val(new double[9]),
      size(3),
      factor(f),
      offset(4), yfact(3)
    {
      val[4] = center;		// center entry
      val[0] = val[2] = val[6] = val[8] = diagonal; // diagonal entries
      val[1] = val[3] = val[5] = val[7] = side; // side entries
    }
  ~Stencil() { delete val; }

  double operator()(int x, int y) const {
    return val[yfact*y + x + offset];
  }
  double scale() const { return factor; }
};

#endif
