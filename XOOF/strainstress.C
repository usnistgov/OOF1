// -*- C++ -*-
// $RCSfile: strainstress.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-31 19:15:14 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "eigenvalues.h"
#include "eulerangle.h"
#include "strainstress.h"
#include "symeig3.h"
#include <math.h>
#include <iomanip.h>

double StrainStress::invariant1() const { // trace
  return operator()(0,0) + operator()(1,1) + operator()(2,2);
}

double StrainStress::invariant3() const { // determinant
  double m00 = operator()(0,0);
  double m11 = operator()(1,1);
  double m22 = operator()(2,2);
  double m01 = operator()(0,1);
  double m12 = operator()(1,2);
  double m02 = operator()(0,2);
  return m00*(m11*m22 - m12*m12)
    - m01*(m01*m22 - m12*m02)
    + m02*(m01*m12 - m11*m02);
}

double StrainStress::invariant2() const {
  double m00 = operator()(0,0);
  double m11 = operator()(1,1);
  double m22 = operator()(2,2);
  double m01 = operator()(0,1);
  double m12 = operator()(1,2);
  double m02 = operator()(0,2);
  return m00*m22 + m00*m11 + m11*m22 - m01*m01 - m02*m02 - m12*m12;
    
}

double StrainStress::principal1() const {
  EigenValues evs;
  eigenvalues(*this, evs);
  return evs.max();
}

double StrainStress::principal2() const {
  EigenValues evs;
  eigenvalues(*this, evs);
  return evs.mid();
}

double StrainStress::principal3() const {
  EigenValues evs;
  eigenvalues(*this, evs);
  return evs.min();
}

double StrainStress::shear() const {
  EigenValues evs;
  eigenvalues(*this, evs);
  return 0.5*(evs.max() - evs.min());
}

double StrainStress::shear2d() const {
  // assume that one eigenvalue is nearly zero, and return the
  // difference of the other two.
  EigenValues evs;
  eigenvalues(*this, evs);
  double a1 = fabs(evs.max());
  double a2 = fabs(evs.mid());
  double a3 = fabs(evs.min());
  if(a1 < a2 && a1 < a3)	// a1 is nearly zero
    return 0.5*(evs.mid() - evs.min());
  if(a2 < a1 && a2 < a3)	// a2 is nearly zero
    return 0.5*(evs.max() - evs.min());
  return 0.5*(evs.max() - evs.mid()); // a3 is nearly zero
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

const StrainStress &StrainStress::operator*=(double a) {
  SymmMatrix::operator*=(a);
  return *this;
}

StrainStress operator*(const StrainStress &A, double x) {
  return StrainStress(A)*=x;
}

const StrainStress &StrainStress::operator/=(double a) {
  return operator*=(1./a);
}

StrainStress operator/(const StrainStress &A, double x) {
  return A*(1./x);
}

double operator*(const StrainStress &A, const StrainStress &B) {
  double result = 0;
  for(int i=0; i<3; i++)
    for(int j=0; j<3; j++)
      result += A(i,j)*B(i,j);
  return result;
}

StrainStress operator+(const StrainStress &A, const StrainStress &B) {
  StrainStress result(A);
  result += B;			// defined in SymmMatrix base class
  return result;
}

StrainStress operator-(const StrainStress &A, const StrainStress &B) {
  StrainStress result(A);
  result -= B;			// defined in SymmMatrix base class
  return result;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

ostream &StrainStress::fancy_output(ostream &os, const char *prefix) const {
  os << prefix << "              X             Y             Z" << endl;
  os << prefix << "X ";
  os << setw(13) << operator()(0,0) << " "
     << setw(13) << operator()(0,1) << " "
     << setw(13) << operator()(0,2) << endl;
  os << prefix << "Y               ";
  os << setw(13) << operator()(1,1) << " "
     << setw(13) << operator()(1,2) << endl;
  os << prefix << "Z                             ";
  os << setw(13) << operator()(2,2);
  return os;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

SphereCoord EigenVector::orientation() const {
  return SphereCoord(&eigenvector[0]);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

ostream &operator<<(ostream &os, const EigenVector &eig) {
  os << "["
     << setw(13) << eig.eigenvector[0] << ", "
     << setw(13) << eig.eigenvector[1] << ", "
     << setw(13) << eig.eigenvector[2] << "]    "
     << setw(13) << eig.eigenvalue;
  return os;
}
