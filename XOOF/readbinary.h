// -*- C++ -*-
// $RCSfile: readbinary.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:13 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef READBINARY_H
#define READBINARY_H

#include <stdio.h>

template <class TYPE>
int readbinary(FILE*, TYPE&, const int n = 1);

template <class TYPE>
void writebinary(FILE*, const TYPE&, const int n = 1);

#include "readbinary.C"

#endif // READBINARY_H
