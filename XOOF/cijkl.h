// -*- C++ -*-
// $RCSfile: cijkl.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:48 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef CIJKL_H
#define CIJKL_H

#include "mvmd.h"
#include "symmmatrix.h"
class StrainStress;

class Cijkl {
private:
  SymmMatrix c;
public:
  Cijkl() : c(6) {}
	
  double &operator()(const int a, const int b) {
    return c(a, b);
  }
  double operator()(const int a, const int b) const {
    return c(a, b);
  }

  void clear() { c.clear(); }

  double &operator()(const int, const int, const int, const int);
  double operator()(const int, const int, const int, const int) const;
	
  Cijkl transform(const MV_ColMat_double &/*3x3 matrix*/ ) const;
	
  MV_ColMat_double planestrain() const;
  MV_ColMat_double planestress() const;
  MV_ColMat_double psi_matrix() const;

  // solve for strain, given stress
  StrainStress solve(const StrainStress&) const;


  void binarywrite(FILE *f) const { c.binarywrite(f); }
  int binaryread(FILE *f) { return c.binaryread(f); }
  friend ostream &operator<<(ostream&, const Cijkl&);

  friend StrainStress operator*(const Cijkl&, const SymmMatrix&);
};

// ----------------------------------------------------------- //

/* Voigt minus 1           fortran version
 *  00 -> 0                    11 -> 1
 *  11 -> 1                    22 -> 2
 *  22 -> 2                    33 -> 3
 *  12 -> 3                    23 -> 4
 *  02 -> 4                    13 -> 5
 *  01 -> 5                    12 -> 6
 */
 
inline int voigt(const int i, const int j) {
  if(i == j) return i;
  return 6 - i - j;
}

#endif
