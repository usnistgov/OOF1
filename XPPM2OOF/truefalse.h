// -*- C++ -*-
// $RCSfile: truefalse.h,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2000-09-06 20:11:50 $



/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef TRUEFALSE_H
#define TRUEFALSE_H

#include <iostream.h>

class TrueFalse {
private:
  bool flag;
public:
  TrueFalse() : flag(0) {}
  TrueFalse(const int i) {
    flag = ( i == 0? 0 : 1 );
  }

  //  operator int() const { return flag; }
  operator bool() const { return flag; }

  const TrueFalse &operator|=(const TrueFalse &b) {
    flag |= b.flag;
    return *this;
  }

  const TrueFalse &operator&=(const TrueFalse &b) {
    flag &= b.flag;
    return *this;
  }

  friend int operator==(const TrueFalse &a, const TrueFalse &b) {
    return (a.flag == b.flag);
  }

  friend int operator!=(const TrueFalse &a, const TrueFalse &b) {
    return (a.flag != b.flag);
  }

  friend ostream& operator<<(ostream&, const TrueFalse&);
  friend istream& operator>>(istream&, TrueFalse&);
};

extern const TrueFalse TF_TRUE;
extern const TrueFalse TF_FALSE;

#endif
