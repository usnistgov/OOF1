// -*- C++ -*-
// $RCSfile: eulerangle.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-22 14:11:09 $


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

class EulerAngle;
class MV_Vector_double;

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
  
  // return individual components
  double alpha() const { return alpha_; }
  double beta() const { return beta_; }
  double gamma() const { return gamma_; }
  
  EulerAngle &operator=(double b) {
    alpha_ = 0.0;
    beta_ = b;
    gamma_ = 0.0;
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

