// -*- C++ -*-
// $RCSfile: normtype.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2005-02-17 21:34:53 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "normtype.h"

void init_normtype() {
  Enum<NormType>::name(L2_NORM, "L_2");
  Enum<NormType>::name(LINF_NORM, "L_infinity");
}

template <>
VarObject* 
Var<Enum<NormType> >::create_varobject(Variable *var, Form *form,
						 VarObjectType t,
						 int x, int y, int w, int h)
{
  return new EnumVarObject<NormType>(var, form, t, x, y, w, h);
}

