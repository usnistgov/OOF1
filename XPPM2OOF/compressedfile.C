// -*- C++ -*-
// $RCSfile: compressedfile.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2002-10-15 21:08:27 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "compressedfile.h"
#include "charstring.h"
#include "menuDef.h"
#ifdef NEED_FDSTREAM
#include "fdstream.hpp"
#endif

CompressedFile::CompressedFile(const CharString &name)
  : stream_(0), ok_(0)
{
  FILE *file;
  if(name.tail(3) == ".gz")
    file = popen("gunzip -c " + name, "r");
  else
    file = fopen(name, "r");
  if(!file) return;
  fflush(file);
#ifdef NEED_FDSTREAM
  stream_ = new boost::fdistream(fileno(file));
#else
  stream_ = new ifstream(fileno(file));
#endif
  ok_ = 1;
}
