// -*- C++ -*-
// $RCSfile: arson.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:23 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef ARSON_H
#define ARSON_H

#include "menuDef.h"
#include "pixelgroups.h"
#include "burn.h"

class ArsonCmd : public CommandM {
private:
  CharString fmt; // format string used to construct group names
  CharString fmtname(int) const;  // construct name from fmt
  PixelGroup flammable;
  void find_flammable();
  NormalBurn burner;
public:
  ArsonCmd();
  virtual void func();
  virtual CommandM *clone() const;
};

#endif
