// -*- C++ -*-
// $RCSfile: planarity.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2005-02-17 22:26:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "enum.h"
#include "planarity.h"
#include <iostream.h>

void init_planarity() {
  Enum<ThermalPlanarity>::name(TP_IN_PLANE_FLUX, "plane_flux");
  Enum<ThermalPlanarity>::name(TP_IN_PLANE_GRADIENTS, "plane_gradient");
  Enum<ElasticPlanarity>::name(EP_IN_PLANE_FLUX, "plane_stress");
  Enum<ElasticPlanarity>::name(EP_IN_PLANE_GRADIENTS, "plane_strain");
}

template<>
VarObject* Var<Enum<ThermalPlanarity> >::create_varobject(Variable *var,
							  Form *form,
							  VarObjectType t,
							  int x, int y,
							  int w, int h)
{
  return new EnumVarObject<ThermalPlanarity>(var, form, t, x, y, w, h);
}

template<>
VarObject* Var<Enum<ElasticPlanarity> >::create_varobject(Variable *var,
							  Form *form,
							  VarObjectType t,
							  int x, int y,
							  int w, int h)
{
  return new EnumVarObject<ElasticPlanarity>(var, form, t, x, y, w, h);
}
