// -*- C++ -*-
// $RCSfile: normtype.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2001-10-26 01:24:46 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef NORMTYPE_H
#define NORMTYPE_H
#include "beginheader.h"

#include "enum.h"

enum NormType {L2_NORM, LINF_NORM};
EXTERN Enum<NormType> normtype DFLT(L2_NORM);

void init_normtype();

#include "varobjects.h"
template <>
VarObject *
Var<Enum<NormType> >::create_varobject(Variable *var, Form *form,
                                                 VarObjectType t,
                                                 int x, int y, int w, int h);


#include "endheader.h"
#endif
