// -*- C++ -*-
// $RCSfile: kij.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-10-13 20:02:51 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef KIJ_H
#define KIJ_H

// thermal conductivity or dielectric tensor

#include "mvvd.h"
#include "mvmd.h"
#include "symmmatrix.h"

class Kij {
private:
  SymmMatrix c;
public:
  Kij() : c(3) {}
  Kij(const SymmMatrix &cc) : c(cc) {}
  double &operator()(int i, int j) { return c(i, j); }
  double operator()(int i, int j) const { return c(i, j); }

  void clear() { c.clear(); }

  Kij transform(const MV_ColMat_double &rot) const { return c.transform(rot); }

  MV_ColMat_double planestress() const;
  MV_Vector_double psi_vector() const;

  void binarywrite(FILE *f) const { c.binarywrite(f); }
  int binaryread(FILE *f) { return c.binaryread(f); }
  friend ostream &operator<<(ostream&, const Kij&);
  friend MV_Vector_double operator*(const Kij&, const MV_Vector_double&);
};

#endif
