// -*- C++ -*-
// $RCSfile: griffithmaterial.h,v $
// $Revision: 1.5 $
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


#ifndef GRIFFITHMATERIAL_H
#define GRIFFITHMATERIAL_H

#include "materialT.h"
#include "menuDef.h"
#include "eulerangle.h"

class GriffithMaterial : public MaterialT {
private:
  static float young_dflt, poisson_dflt, t_coeff_dflt,
    knockdown1_dflt, knockdown2_dflt, gamma_dflt;
  float young, poisson, t_coeff, knockdown1, knockdown2, gamma;
  static TrueFalse onlyonce_dflt;
  TrueFalse onlyonce;
#ifdef THERMAL
  static float kappa_dflt;
  float kappa;
#endif // THERMAL
  
  static MaterialTypeRegistration reg;
  static CharString type;

public:
  GriffithMaterial() :
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    knockdown1(knockdown1_dflt), knockdown2(knockdown2_dflt),
    gamma(gamma_dflt), onlyonce(onlyonce_dflt)
  {}

  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3) const;

  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3, int same_element) const;

  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa", kappa_dflt);
#endif // THERMAL
    AddArgument(cmd, "gamma", gamma_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    AddArgument(cmd, "only_once", onlyonce_dflt);
  }
  
  virtual CharString &tag() const { return type; }
  virtual MaterialTypeRegistration *get_reg() const { return &reg; }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

// As far as ppm2oof is concerned, the two griffith materials differ
// only by name. They have the same parameter lists.

class GriffithMaterial2 : public GriffithMaterial {
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  virtual MaterialTypeRegistration *get_reg() const { return &reg; }
  virtual CharString &tag() const { return type; }
};

#endif
