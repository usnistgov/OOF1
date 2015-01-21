// -*- C++ -*-
// $RCSfile: eigenvalues.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:54 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "eigenvalues.h"

EigenValues::EigenValues(double e1, double e2, double e3) {
  // sort
  if(e1 > e2 && e1 > e3) {
    max_ = e1;
    if(e2 > e3) {
      mid_ = e2;
      min_ = e3;
    }
    else {
      mid_ = e3;
      min_ = e2;
    }
  }
  else if(e2 > e1 && e2 > e3) {
    max_ = e2;
    if(e1 > e3) {
      mid_ = e1;
      min_ = e3;
    }
    else {
      mid_ = e3;
      min_ = e1;
    }
  }
  else {
    max_ = e3;
    if(e1 > e2) {
      mid_ = e1;
      min_ = e2;
    }
    else {
      mid_ = e2;
      min_ = e1;
    }
  }
}

ostream &operator<<(ostream &os, const EigenValues &ev)
{
  return os << ev.max_ << " " << ev.mid_ << " " << ev.min_;
}

