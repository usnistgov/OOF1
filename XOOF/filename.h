// -*- C++ -*-
// $RCSfile: filename.h,v $
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


#ifndef FILENAME_H
#define FILENAME_H

#include "charstring.h"
#include "menuDef.h"

class FileName : public CharString {
public:
  FileName() {}
  FileName(const CharString &n) : CharString(n) {}
};

#ifdef MENUGFX
#include "varobjects.h"
template <>
VarObject *Var<FileName>::create_varobject(Variable *var, Form *form,
  					   VarObjectType type,
  					   int x, int y, int w, int h);
#endif // MENUGFX

template <>
int Var<FileName>::fromstring(const CharString &str, Waiter&) const;

template <>
void Var<FileName>::fromstring(TextWaiter &w) const;

#endif
