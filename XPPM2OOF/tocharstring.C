// -*- C++ -*-
// $RCSfile: tocharstring.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2003-10-30 21:33:05 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef TOCHARSTRING_C
#define TOCHARSTRING_C

#include "charstring.h"
#include "tocharstring.h"
#include <iostream>
#ifdef DONT_HAVE_STRSTREAM_H
#include <strstream>
using namespace std;
#else
#include <strstream.h>
#endif

// convert anything to a CharString
template <class TYPE> 
CharString to_charstring(const TYPE &x) {
    ostrstream os;
    os << x << ends;
    char *buf = os.str();
    CharString cs(buf);
    delete buf;
    return cs;
}

#endif
