// -*- C++ -*-
// $RCSfile: myfread.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:10 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* versions of fread and fwrite that swap bytes on little-endian machines */

#ifndef MYFREAD_H
#define MYFREAD_H

#include <stdio.h>

#if LITTLEENDIAN

size_t myfread(void *ptr, size_t size, size_t nitems, FILE *stream);
size_t myfwrite(const void *ptr, size_t size, size_t nitems, FILE *stream);

#else  /* ! LITTLEENDIAN */

#define myfread fread
#define myfwrite fwrite

#endif /* LITTLEENDIAN */

#endif /* MYFREAD_H */
