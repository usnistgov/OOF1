// -*- C++ -*-
// $RCSfile: materialT.h,v $
// $Revision: 1.4 $
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

#ifndef MATERIALT_H
#define MATERIALT_H

#include "enum.h"
#include "material.h"
#include "planarity.h"

class MaterialT : public Material {
protected:
#ifdef THERMAL
  float t0;
  static float t0_dflt;
  Enum<ThermalPlanarity> thermal_planarity;
  static Enum<ThermalPlanarity> thermal_planarity_dflt;
#endif // THERMAL
public:
  MaterialT()
#ifdef THERMAL
    : t0(t0_dflt),
      thermal_planarity(thermal_planarity_dflt)
#endif // THERMAL
  {}
  virtual ~MaterialT() {}
  virtual void output(FILE*, unsigned char&, int n1, int n2, int n3) const;
  virtual void output(FILE*, unsigned char&, int n1, int n2, int n3, int) const;
  virtual void AddArgs(CommandM *cmd) {
    Material::AddArgs(cmd);
#ifdef THERMAL
    AddArgument(cmd, "T_planarity", thermal_planarity_dflt);
    AddArgument(cmd, "T0", t0_dflt);
#endif // THERMAL
  }
  virtual int compare(const Material*) const;
#ifdef THERMAL
  bool query_Tplanarity() const { return thermal_planarity; }
#endif // THERMAL
};

#endif
