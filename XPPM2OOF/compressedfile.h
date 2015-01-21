// -*- C++ -*-
// $RCSfile: compressedfile.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2002-10-15 18:31:30 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef COMPRESSEDFILE_H
#define COMPRESSEDFILE_H

#include <iostream.h>
#include <fstream.h>
class CharString;

class CompressedFile {
private:
  istream *stream_;
  int ok_;
public:
  CompressedFile(const CharString &name);
  ~CompressedFile() { delete stream_; }
  int ok() const { return ok_ && *stream_; }
  istream &stream() { return *stream_; }
};

#endif
