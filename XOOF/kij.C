// -*- C++ -*-
// $RCSfile: kij.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2001-06-04 19:13:03 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "kij.h"

// Extract the 2x2 submatrix for plane stress calculations.

MV_Vector_double Kij::psi_vector() const {
  MV_Vector_double psi(2, 0.0);
  if(c(2,2) != 0.0) {
    double c22inv = 1./c(2, 2);
    psi[0] = c22inv*c(0, 2);
    psi[1] = c22inv*c(1, 2);
  }
  return psi;
}

MV_ColMat_double Kij::planestress() const {
  MV_ColMat_double D(2, 2);
  MV_Vector_double psi(psi_vector());
  D(0, 0) = c(0, 0) - c(0, 2)*psi[0];
  D(0, 1) = c(0, 1) - c(0, 2)*psi[1];
  D(1, 0) = c(1, 0) - c(1, 2)*psi[0];
  D(1, 1) = c(1, 1) - c(1, 2)*psi[1];
  return D;
}

MV_Vector_double operator*(const Kij &kij, const MV_Vector_double &v) {
  return kij.c * v;
}

ostream &operator<<(ostream &os, const Kij &kij) {
  return os << kij.c;
}
