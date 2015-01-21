// -*- C++ -*-
// $RCSfile: strainstress.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:16 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef STRAINSTRESS_H
#define STRAINSTRESS_H

class EigenValues;
class StrainStress;

#include "symmmatrix.h"
class SphereCoord;

class EigenVector {
public:
  EigenVector() {}
  EigenVector(double v, double a0, double a1, double a2)
    : eigenvalue(v)
  {
    eigenvector[0] = a0;
    eigenvector[1] = a1;
    eigenvector[2] = a2;
  }
  double eigenvalue;
  double eigenvector[3];
  SphereCoord orientation() const;
  friend ostream &operator<<(ostream&, const EigenVector&);
};

class StrainStress : public SymmMatrix {
public:
  StrainStress() : SymmMatrix(3) {}
  StrainStress(const SymmMatrix &s) : SymmMatrix(s) {}
  double invariant1() const;
  double invariant2() const;
  double invariant3() const;
  double principal1() const;
  double principal2() const;
  double principal3() const;
  double shear() const;
  double shear2d() const;

  ostream &fancy_output(ostream&, const char*) const;

  const StrainStress &operator*=(double);
  friend StrainStress operator*(const StrainStress&, double);
  const StrainStress &operator/=(double);
  friend StrainStress operator/(const StrainStress&, double);
  friend double operator*(const StrainStress&, const StrainStress&);

  //  const StrainStress &operator+=(const StrainStress&);
  //  const StrainStress &operator-=(const StrainStress&);
  friend StrainStress operator+(const StrainStress&, const StrainStress&);
  friend StrainStress operator-(const StrainStress&, const StrainStress&);
};


#endif



