// -*- C++ -*-
// $RCSfile: fname.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:58 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */
#ifndef FNAME_H
#define FNAME_H

/* For portability between SGI and HP when calling fortran from C.
 * fortran names used in C need an underscore appended on SGI but not HP.
 */

#ifdef __hpux
#define FNAME(x) x
#else
#define FNAME(x) x##_
#endif

#endif
