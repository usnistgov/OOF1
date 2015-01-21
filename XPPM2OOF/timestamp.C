// -*- C++ -*-
// $RCSfile: timestamp.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2001-01-30 22:20:10 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "timestamp.h"
#include <limits.h>

unsigned long TimeStamp::globaltime(0);
unsigned long TimeStamp::globalepoch(0);

static const unsigned long MAXTIME = ULONG_MAX;

void TimeStamp::operator++() {
  globaltime++;
  if(globaltime == MAXTIME) {
    globaltime = 0;
    globalepoch++;
  }
  localtime = globaltime;
  localepoch = globalepoch;
}

int operator<(const TimeStamp &t1, const TimeStamp &t2) {
  if(t1.localepoch == t2.localepoch)
    return t1.localtime < t2.localtime;
  return t1.localepoch < t2.localepoch;
}

int operator>(const TimeStamp &t1, const TimeStamp &t2) {
  if(t1.localepoch == t2.localepoch)
    return t1.localtime > t2.localtime;
  return t1.localepoch > t2.localepoch;
}
