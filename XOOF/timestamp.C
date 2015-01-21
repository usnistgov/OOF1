// -*- C++ -*-
// $RCSfile: timestamp.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:17 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "timestamp.h"

unsigned long TimeStamp::globaltime(0);

void TimeStamp::operator++() {
  globaltime++;
  localtime = globaltime;
}

int operator<(const TimeStamp &t1, const TimeStamp &t2) {
  return t1.localtime < t2.localtime;
}

int operator>(const TimeStamp &t1, const TimeStamp &t2) {
  return t1.localtime > t2.localtime;
}

int operator<=(const TimeStamp &t1, const TimeStamp &t2) {
  return t1.localtime <= t2.localtime;
}

int operator>=(const TimeStamp &t1, const TimeStamp &t2) {
  return t1.localtime >= t2.localtime;
}

ostream &operator<<(ostream &os, const TimeStamp &ts) {
  return os << ts.localtime;
}
