// -*- C++ -*-
// $RCSfile: debug.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:52 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// everything is a friend of Debug

#ifndef DEBUG_H
#define DEBUG_H

class Debug;
template <class TYPE> void checkindices(TYPE &, char *);

void breakpoint();

#endif
