// -*- C++ -*-
// $RCSfile: timer.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



#include "timer.h"
#include <iostream.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>

// constructor -- store current user and system time.

Timer::Timer() {
    rusage info;
    getrusage(RUSAGE_SELF, &info);
    starttime_u = info.ru_utime;
    starttime_s = info.ru_stime;
    startclock=time(0);
}

// subtract two timevals

static inline double timediff(const timeval &a, const timeval &b) {
    return a.tv_sec - b.tv_sec + .000001*(a.tv_usec - b.tv_usec);
}

// report elapsed time

void Timer::operator()(double &u, double &s, int &c) const {
    rusage info;
    getrusage(RUSAGE_SELF, &info);
    u = timediff(info.ru_utime, starttime_u);
    s = timediff(info.ru_stime, starttime_s);
    c = time(0)-startclock;
}

ostream &operator<<(ostream &os, const Timer &timer) {
    double utime, stime;
    int clock;
    timer(utime, stime, clock);
    os << "user=" << utime << " "
       << "system=" << stime << " "
       << "clock=" << clock;
    return os;
}
