// -*- C++ -*-
// $RCSfile: cubic_crack_material.h,v $
// $Revision: 1.2 $
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


#ifndef CUBICCRACKMATERIAL_H
#define CUBICCRACKMATERIAL_H

#include "materialT.h"
#include "menuDef.h"
#include "eulerangle.h"

class CubicCrackMaterial : public MaterialT {
private:
  static float young_dflt, poisson_dflt, t_coeff_dflt, anisotropy_dflt;
  static float knockdown1_dflt, knockdown2_dflt, gamma_dflt;
  static TrueFalse onlyonce_dflt;
  static EulerAngle orientation_dflt;
  float young, poisson, t_coeff, anisotropy;
#ifdef THERMAL
  static float kappa_dflt;
  float kappa;
#endif // THERMAL
  float knockdown1, knockdown2, gamma;
  EulerAngle orientation;
  TrueFalse onlyonce;

  static MaterialTypeRegistration reg;
  static CharString type;

public:
  CubicCrackMaterial() :
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    anisotropy(anisotropy_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    knockdown1(knockdown1_dflt), knockdown2(knockdown2_dflt),
    gamma(gamma_dflt),
    orientation(orientation_dflt),
    onlyonce(onlyonce_dflt)
    {}

  virtual void output(FILE*, unsigned char&,  int n1, int n2, int n3) const;

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
    AddArgument(cmd, "gamma", gamma_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    AddArgument(cmd, "only_once", onlyonce_dflt);
  }

  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

#endif

