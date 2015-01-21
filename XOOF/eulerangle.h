// -*- C++ -*-
// $RCSfile: eulerangle.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:57 $


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

class SphereCoord;
class EulerAngle;

class MV_Vector_double;
#include "mvmd.h"

class EulerAngle {
private:
  double alpha_, beta_, gamma_;
public:
  EulerAngle() : alpha_(0.), beta_(0.), gamma_(0.) {}
  EulerAngle(const double a, const double b, const double g)
    : alpha_(a), beta_(b), gamma_(g) {}
  EulerAngle(const SphereCoord&);
  
  EulerAngle(const EulerAngle&);
  EulerAngle& operator=(const EulerAngle&);
  
  // convert to a rotation matrix
  MV_ColMat_double rotation() const;
  operator MV_ColMat_double() const { return rotation(); }
  
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

class SphereCoord {
private:
  double theta_;
  double phi_;
public:
  SphereCoord() : theta_(0), phi_(0) {}
  SphereCoord(double t, double p) : theta_(t), phi_(p) {}
  SphereCoord(const double * const v);	// find orientation of a vector
  SphereCoord(const MV_Vector_double &v); // ditto
  MV_ColMat_double rotation() const;
  operator MV_ColMat_double() const { return rotation(); }
  EulerAngle operator-() const { return EulerAngle(-phi_, 0, -theta_); }
  friend ostream &operator<<(ostream&, const SphereCoord&);
  friend class EulerAngle;
  //  MV_ColMat_double rotation() const;
  //  operator MV_ColMat_double() const { return rotation(); }
};

#endif

