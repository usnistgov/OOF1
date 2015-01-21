// -*- C++ -*-
// $RCSfile: myfread.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:08 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* versions of fread and fwrite that swap bytes on little-endian machines */

#ifdef LITTLEENDIAN

#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include "myfread.h"

/* copy nitems items of size size from oldbuf to newbuf, reversing the
 * byte order in each item.
 */

static void copyswap(size_t size, size_t nitems, const unsigned char *oldbuf,
	      unsigned char *newbuf)
{
  int i, top, j;
  int total = nitems*size;
  for(i=0; i<total; i+=size) {
    top = i+size-1;
    for(j=0; j<size; j++)
      newbuf[i+j] = oldbuf[top-j];
  }
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

size_t myfread(void *ptr, size_t size, size_t nitems, FILE *stream) {
  unsigned char *buf;
  size_t status;

  if(size == 1)
    buf = (unsigned char*) ptr;	// don't have to use temp space
  else
    buf = new unsigned char[size*nitems]; // temp space

  status = fread(buf, size, nitems, stream);

  if(size != 1) {
    copyswap(size, nitems, buf, (unsigned char*) ptr);
    delete buf;
  }

  return status;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

size_t myfwrite(const void *ptr, size_t size, size_t nitems, FILE *stream) {
  int status;
  unsigned char *buf;

  if(size == 1)
    buf = (unsigned char *) ptr;
  else {
    buf = new unsigned char[size*nitems]; // temp space
    copyswap(size, nitems, (unsigned char*) ptr, buf);
  }
  status = fwrite(buf, size, nitems, stream);
  if(size != 1)
    delete buf;
  return status;
}

#endif
