// -*- C++ -*-
// $RCSfile: readbinary.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:45 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef READBINARY_C
#define READBINARY_C

#include "readbinary.h"
#include "menuDef.h"
#include "myfread.h"

#if defined(__sun) && !defined(__SVR4)
#define CAST (char*)
#else
#define CAST /**/
#endif

template <class TYPE>
int readbinary(FILE *file, TYPE &x, const int n) {
  int nread = myfread(CAST &x, sizeof(TYPE), n, file);
  if(nread != n) {
    garcon()->msout << ms_error << "readbinary: read " << nread
		    << ", expected " << n << endl << ms_normal;
    return 0;
  }
  return 1;
}

template <class TYPE>
void writebinary(FILE *file, const TYPE &x, const int n) {
    myfwrite(CAST &x, sizeof(TYPE), n, file);
}

#endif
