// -*- C++ -*-
// $RCSfile: eulerangle.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:30 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef EULERANGLE_H
#define EULERANGLE_H

#include "mvmd.h"
#include <iostream.h>

class EulerAngle {
private:
  double alpha_, beta_, gamma_;
public:
  EulerAngle() : alpha_(0.), beta_(0.), gamma_(0.) {}
  EulerAngle(const double a, const double b, const double g)
    : alpha_(a), beta_(b), gamma_(g) {}
  
  EulerAngle(const EulerAngle&);
  EulerAngle& operator=(const EulerAngle&);
  
  // convert to a rotation matrix
  MV_ColMat_double rotation() const;
  operator MV_ColMat_double() const { return rotation(); }
  
  // return individual components
  double alpha() const { return alpha_; }
  double beta() const { return beta_; }
  double gamma() const { return gamma_; }
  
  // set
  void set(double a, double b, double g) {
    alpha_ = a; beta_ = b; gamma_ = g;
  }
  EulerAngle &operator=(double b) {
    set(0.0, b, 0.0);
    return *this;
  }
  // invert
  EulerAngle operator-() const {
    return EulerAngle(-alpha_, -gamma_, -beta_);
  }
  
  friend ostream &operator<<(ostream &, const EulerAngle&);
  friend istream &operator>>(istream &, EulerAngle&);
  
  friend int operator==(const EulerAngle &om1, const EulerAngle &om2);
  
};

#endif
