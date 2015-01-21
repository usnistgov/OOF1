// -*- C++ -*-
// $RCSfile: expandsel.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:31 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef EXPANDSEL_H
#define EXPANDSEL_H

#include "menuDef.h"

class ExpandSelectionCmd : public CommandM {
private:
  double range;
public:
  ExpandSelectionCmd() 
    : CommandM("expand", "expand selected region"),
      range(1.5)
    {
      AddArgument(this, "range", range);      
    }
  CommandFn func();
  CommandM *clone() const;
};

class ShrinkSelectionCmd : public CommandM {
private:
  double range;
public:
  ShrinkSelectionCmd() 
    : CommandM("shrink", "shrink selected region"),
      range(1.5)
    {
      AddArgument(this, "range", range);      
    }
  CommandFn func();
  CommandM *clone() const;
};

#endif
