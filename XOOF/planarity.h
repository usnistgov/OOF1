// -*- C++ -*-
// $RCSfile: planarity.h,v $
// $Revision: 1.3 $
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


#ifndef PLANARITY_H
#define PLANARITY_H

#include <iostream.h>


// It would be nice to make typedefs for Enum<ThermalPlanarity> and
// Enum<ElasticPlanarity>, but the Enum templates don't seem to work
// if they're typedefed.

enum ThermalPlanarity {
  TP_IN_PLANE_FLUX,
  TP_IN_PLANE_GRADIENTS
};

enum ElasticPlanarity {
  EP_IN_PLANE_FLUX,
  EP_IN_PLANE_GRADIENTS
};

void init_planarity();		// initializes Enum templates

#include "varobjects.h"
template <>
VarObject*
Var<Enum<ThermalPlanarity> >::create_varobject(Variable *var, Form *form,
                                               VarObjectType t,
                                               int x, int  y, int w, int h);
#endif
