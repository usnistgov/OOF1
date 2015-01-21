// -*- C++ -*-
// $RCSfile: amtriangleiterator.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:23 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef AMTRIANGLEITERATOR_H
#define AMTRIANGLEITERATOR_H

class AMNode;
class AMTriangle;
#include "cell_coordinate.h"

// Iterator to loop over pixels underlying a triangle
class AMTriangleIterator {
private:
  double lslope_btm, rslope_btm; // slopes of sides of bottom half of triangle
  double lslope_top, rslope_top; // slopes of sides ot top half of triangle

  // x range for rows of pixels at given y in top and bottom halves
  double xmin_top(double) const;
  double xmax_top(double) const;
  double xmin_btm(double) const;
  double xmax_btm(double) const;

  int btm_done, top_done;	// has each half been looped over yet?
  int current_xmax;
  int maxx, maxy;		// absolute max values, determined by goof
  int midright;			// is the middle node on the right or left side?

  const AMNode *topnode;
  const AMNode *midnode;
  const AMNode *btmnode;
  Cell_coordinate current;	// current pixel
public:
  AMTriangleIterator(const AMTriangle &);
  void operator++();
  int end() { return top_done && btm_done; }
  friend class AMTriangle;
};

#endif
