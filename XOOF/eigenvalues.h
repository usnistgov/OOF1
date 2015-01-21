// -*- C++ -*-
// $RCSfile: eigenvalues.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:54 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// simply a class used to store and sort three numbers...

#ifndef EIGENVALUES_H
#define EIGENVALUES_H

#include <iostream.h>

class EigenValues {
private:
  double max_, mid_, min_;
public:
  EigenValues(double e1, double e2, double e3);
  EigenValues() : max_(0.), mid_(0.), min_(0.) {}
  double max() const { return max_; }
  double mid() const { return mid_; }
  double min() const { return min_; }
  friend ostream &operator<<(ostream&, const EigenValues&);
};

#endif
