// -*- C++ -*-
// $RCSfile: isomaterial.h,v $
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

#ifndef ISOMATERIAL_H
#define ISOMATERIAL_H

#include "material.h"
#include "menuDef.h"

// This isn't derived from MaterialT, because thermal_planarity is
// irrelevant for isotropic materials.  So this material has to
// include its own t0, which is otherwise in MaterialT.

class IsoMaterial : public Material {
protected:
  static float young_dflt, poisson_dflt, t_coeff_dflt;
  float young, poisson, t_coeff;
#ifdef THERMAL
  static float t0_dflt, kappa_dflt;
  float kappa, t0;
#endif // THERMAL
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
    
  IsoMaterial() :
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt)
#ifdef THERMAL
    ,
    kappa(kappa_dflt),
    t0(t0_dflt)
#endif // THERMAL
  {}

  virtual void output(FILE*, unsigned char &flag,
		      int n1, int n2, int n3) const;

  virtual void output(FILE*, unsigned char &flag,
		      int n1, int n2, int n3, int same_element) const;
  virtual void AddArgs(CommandM *cmd) {
    Material::AddArgs(cmd);
#ifdef THERMAL
    AddArgument(cmd, "T0", t0_dflt);
#endif // THERMAL
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa", kappa_dflt);
#endif // THERMAL
  }
	
  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;
  virtual std::vector<CharString> *print_properties(ostream&) const;
};
    
#endif
