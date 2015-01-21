// -*- C++ -*-
// $RCSfile: cubicmaterial.h,v $
// $Revision: 1.6 $
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

#ifndef CUBICMATERIAL_H
#define CUBICMATERIAL_H

#include "materialT.h"
#include "menuDef.h"
#include "eulerangle.h"


class CubicMaterial : public MaterialT {
protected:
  static float young_dflt, poisson_dflt, t_coeff_dflt, anisotropy_dflt;
  static EulerAngle orientation_dflt;
  float young, poisson, t_coeff, anisotropy;
  EulerAngle orientation;
#ifdef THERMAL
  static float kappa_dflt;
  float kappa;
#endif // THERMAL
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  
  CubicMaterial() :
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    anisotropy(anisotropy_dflt),
    orientation(orientation_dflt)
  {}
  
  virtual void output(FILE*, unsigned char&, int n1, int n2, int n3) const;
  
  virtual void output(FILE*, unsigned char&, int n1, int n2, int n3,
		      int same_element) const;
  
  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "orientation", orientation_dflt);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
    AddArgument(cmd, "anisotropy", anisotropy_dflt);
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
