// -*- C++ -*-
// $RCSfile: twotwo.h,v $
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


// 2x2 matrix

#ifndef TWOTWO_H
#define TWOTWO_H

class MeshCoord;
#include <iostream.h>
#include <stdio.h>

#include "beginheader.h"

class TwoTwo {
    public:
	double m00, m01, m10, m11;  // data is all public!
	
	TwoTwo() : m00(0.0), m01(0.0), m10(0.0), m11(0.0) {}
	TwoTwo(double a00, double a01, double a10, double a11)
	    : m00(a00), m01(a01), m10(a10), m11(a11) {}
	TwoTwo inverse() const;
	double &operator()(const int, const int);
	const double &operator()(const int, const int) const;
	const TwoTwo &operator+=(const TwoTwo &a);
    friend TwoTwo operator+(const TwoTwo&, const TwoTwo&);
    friend TwoTwo operator*(const TwoTwo&, const TwoTwo&);
    friend MeshCoord operator*(const TwoTwo&, const MeshCoord&);
    friend TwoTwo operator*(const TwoTwo&, const double);
    friend TwoTwo operator*(const double, const TwoTwo&);
    friend ostream& operator<<(ostream&, const TwoTwo&);
  friend istream &operator>>(istream&, TwoTwo&);
  void print(FILE *) const;
};

EXTERN TwoTwo I2x2 DFLT(TwoTwo(1.0, 0.0, 0.0, 1.0));

#include "endheader.h"

#endif

