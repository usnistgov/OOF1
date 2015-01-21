// -*- C++ -*-
// $RCSfile: output.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:43 $

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
#include "truefalse.h"
#include <stdio.h>

#include "beginheader.h"

EXTERN int use_stdout DFLT(0);
EXTERN double physical_width DFLT(1.0);

class OutputCmd : public CommandM {
private:
  FileName filename;
public:
  OutputCmd() : CommandM("entire", "write an oof file"),
    filename("out.goof")
  {
    AddArgument(this, "filename", filename);
  }
  virtual CommandFn func();
  virtual CommandM *clone() const {
    OutputCmd *oc = new OutputCmd;
    oc->filename = filename;
    return oc;
  }
};

class SubRegionOutputCmd : public CommandM {
private:
  FileName filename;
  Cell_coordinate c1, c2;
public:
  SubRegionOutputCmd();
  virtual CommandFn func();
  virtual CommandM *clone() const {
    SubRegionOutputCmd *oc = new SubRegionOutputCmd;
    oc->filename = filename;
    oc->c1 = c1;
    oc->c2 = c2;
    return oc;
  }
};

EXTERN TrueFalse bluebox DFLT(1);  // recycle elements?

class OutputFlags {
public:
  static const int end_marker;
  static const int xy_node;
  static const char no_displacement;
};

void endlist(FILE*);

#include "endheader.h"

#endif
