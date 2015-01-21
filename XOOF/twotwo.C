// -*- C++ -*-
// $RCSfile: twotwo.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:59 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// 2x2 matrix stuff

#include "fatalerror.h"
#include "lookfor.h"
#include "meshcoord.h"
#include "stdlib.h"
#include "twotwo.h"

TwoTwo TwoTwo::inverse() const {
    TwoTwo inv;
    double det = m00*m11 - m01*m10;
    // should check for zero determinant
    det = 1./det;
    inv.m00 = m11*det;
    inv.m11 = m00*det;
    inv.m01 = -m01*det;
    inv.m10 = -m10*det;
    return inv;
}

TwoTwo operator+(const TwoTwo &a, const TwoTwo &b) {
    TwoTwo sum(a);
    sum.m00 += b.m00;
    sum.m01 += b.m01;
    sum.m10 += b.m10;
    sum.m11 += b.m11;
    return sum;
}

const TwoTwo &TwoTwo::operator+=(const TwoTwo &a) {
    m00 += a.m00;
    m01 += a.m01;
    m10 += a.m10;
    m11 += a.m11;
    return *this;
}

TwoTwo operator*(const TwoTwo &a, const TwoTwo &b) {
    TwoTwo prod;
    prod.m00 = a.m00*b.m00 + a.m01*b.m10;
    prod.m01 = a.m00*b.m01 + a.m01*b.m11;
    prod.m10 = a.m10*b.m00 + a.m11*b.m10;
    prod.m11 = a.m10*b.m01 + a.m11*b.m11;
    return prod;
}

MeshCoord operator*(const TwoTwo &a, const MeshCoord &z) {
    return MeshCoord(a.m00*z.x + a.m01*z.y, a.m10*z.x + a.m11*z.y);
}

TwoTwo operator*(const TwoTwo &a, const double x) {
    TwoTwo prod(a);
    prod.m00 *= x;
    prod.m01 *= x;
    prod.m10 *= x;
    prod.m11 *= x;
    return prod;
}

TwoTwo operator*(const double x, const TwoTwo &a) {
    TwoTwo prod(a);
    prod.m00 *= x;
    prod.m01 *= x;
    prod.m10 *= x;
    prod.m11 *= x;
    return prod;
}

const double &TwoTwo::operator()(const int i, const int j) const {
    // would be faster without checking for valid indices
    if(i == 0) {
	if(j == 0)
	    return m00;
	else if(j == 1)
	    return m01;
    }
    else if (i == 1) {
	if(j == 0)
	    return m10;
	else
	    return m11;
    }
    cerr << "TwoTwo index out of bounds!" << endl;
    fatalerror();
    /*NOTREACHED*/
}

double &TwoTwo::operator()(const int i, const int j) {
    // would be faster without checking for valid indices
    if(i == 0) {
	if(j == 0)
	    return m00;
	else if(j == 1)
	    return m01;
    }
    else if (i == 1) {
	if(j == 0)
	    return m10;
	else
	    return m11;
    }
    cerr << "TwoTwo index out of bounds!" << endl;
    fatalerror();
    /*NOTREACHED*/
}

ostream &operator<<(ostream &os, const TwoTwo &a) {
  os << "((" << a.m00 << ", " << a.m01 << "), ("
     << a.m10 << ", " << a.m11 << "))";
  return os;
}

istream &operator>>(istream &is, TwoTwo &a) {
  if(!is) return is;
  if(!lookfor('(', is)) return is;
  if(!lookfor('(', is)) return is;
  is >> a.m00;
  if(!lookfor(',', is)) return is;
  is >> a.m01;
  if(!lookfor(')', is)) return is;
  if(!lookfor(',', is)) return is;
  if(!lookfor('(', is)) return is;
  is >> a.m10;
  if(!lookfor(',', is)) return is;
  is >> a.m11;
  if(!lookfor(')', is)) return is;
  lookfor(')', is);
  return is;
}

void TwoTwo::print(FILE *file) const {
  fprintf(file, "((%e, %e), (%e, %e))", m00, m01, m10, m11);
}
