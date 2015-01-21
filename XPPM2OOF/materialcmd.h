// -*- C++ -*-
// $RCSfile: materialcmd.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2001-04-03 14:28:07 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef MATERIALCMD_H
#define MATERIALCMD_H

#include "menuDef.h"

template <class TYPE>
class MaterialCmd :  public CommandM, public TYPE {
public:
  MaterialCmd();
  virtual void func();
  virtual CommandM *clone() const;
};

template <class TYPE>
class MaterialTriCmd : public CommandM, public TYPE {
public:
  MaterialTriCmd();
  virtual void func();
  virtual CommandM *clone() const;
};

#include "materialcmd.C"

#endif
