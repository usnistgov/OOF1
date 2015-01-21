// -*- C++ -*-
// $RCSfile: cijkl.C,v $
// $Revision: 1.1 $
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

#include <iostream.h>
#include "cijkl.h"

double &Cijkl::operator()(const int i, const int j, const int k, const int l) {
  return c(voigt(i, j), voigt(k, l));
}

double Cijkl::operator()(const int i, const int j,
				const int k, const int l) const
{
  return c(voigt(i, j), voigt(k, l));
}

ostream &operator<<(ostream &os, const Cijkl &cijkl) {
  return os << cijkl.c;
}

