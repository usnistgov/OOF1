// -*- C++ -*-
// $RCSfile: output.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:10 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef OUTPUT_H
#define OUTPUT_H

#include "menuDef.h"
#include "filename.h"

#include "beginheader.h"

// base class for commands taking a file argument

class FileCmd : public CommandM {
protected:
  FileName filename;
public:
  FileCmd(const CharString &n,  const CharString &hs, const CharString &fname)
    : CommandM(n, hs), filename(fname)
  {
    AddArgument(this, "filename", filename);
  }
  virtual CommandFn func() = 0;
  virtual CommandM *clone() const {
    FileCmd *fc = subclone();
    fc->filename = filename;
    return fc;
  }
  virtual FileCmd *subclone() const = 0;
};



#include "endheader.h"



#endif
