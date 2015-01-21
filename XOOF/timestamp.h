// -*- C++ -*-
// $RCSfile: timestamp.h,v $
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

// class to keep track of what's happened when, so that the program
// knows if something is out of date with respect to something else.

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <iostream.h>

class TimeStamp {
private:
  static unsigned long globaltime;
  unsigned long localtime;
public:
  TimeStamp() : localtime(0) {}
  TimeStamp(const TimeStamp &ts) : localtime(ts.localtime) {}
  void operator++();
  void backdate() { localtime = 0; }
  friend int operator<(const TimeStamp&, const TimeStamp&);
  friend int operator>(const TimeStamp&, const TimeStamp&);
  friend int operator<=(const TimeStamp&, const TimeStamp&);
  friend int operator>=(const TimeStamp&, const TimeStamp&);
  friend ostream &operator<<(ostream&, const TimeStamp&);
};

#endif
