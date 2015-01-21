// -*- C++ -*-
// $RCSfile: materialT.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2004-10-22 13:51:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "materialT.h"
#include "inputflags.h"
#include "readbinary.h"

#ifdef THERMAL
float MaterialT::t0_dflt(0.0);
Enum<ThermalPlanarity> MaterialT::thermal_planarity_dflt(TP_IN_PLANE_FLUX);
#endif // THERMAL

void MaterialT::output(FILE *file, unsigned char &flag,
		       int n1, int n2, int n3) const
{
  Material::output(file, flag, n1, n2, n3);
#ifdef THERMAL
  if(thermal_planarity == TP_IN_PLANE_GRADIENTS)
    flag |= PLANE_TEMP_GRAD;
  writebinary(file, t0);
#endif // THERMAL
}

void MaterialT::output(FILE *file, unsigned char &flag,
		       int n1, int n2, int n3, int nnn) const
{
  Material::output(file, flag, n1, n2, n3, nnn);
#ifdef THERMAL
  if(thermal_planarity == TP_IN_PLANE_GRADIENTS)
    flag |= PLANE_TEMP_GRAD;
  writebinary(file, t0);
#endif // THERMAL
}

int MaterialT::compare(const Material *other) const {
  if(!Material::compare(other)) return 0;
#ifdef THERMAL
  MaterialT *m2 = (MaterialT*) other;
  if(thermal_planarity != m2->thermal_planarity) return 0;
  if(t0 != m2->t0) return 0;
#endif // THERMAL
  return 1;
}

#include "enumio.C"
