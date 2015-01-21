// -*- C++ -*-
// $RCSfile: damagematerial.h,v $
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

#ifndef DAMISOMATERIAL_H
#define DAMISOMATERIAL_H

#include "materialT.h"
#include "menuDef.h"


class DamIsoMaterial : public Material {
protected:
  static float young_dflt, poisson_dflt, t_coeff_dflt;
  static float maxstress_dflt, minstress_dflt;
  static float maxknockdown_dflt, minknockdown_dflt;
  float young, poisson, t_coeff;
  float maxstress, minstress;
  float maxknockdown, minknockdown;
#ifdef THERMAL
  static float kappa_dflt, t0_dflt;
  float kappa, t0;
#endif // THERMAL
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
    
  DamIsoMaterial()
    : young(young_dflt),
      poisson(poisson_dflt),
      t_coeff(t_coeff_dflt),
#ifdef THERMAL
      kappa(kappa_dflt),
      t0(t0_dflt),
#endif // THERMAL
      maxstress(maxstress_dflt), minstress(minstress_dflt),
      maxknockdown(maxknockdown_dflt), minknockdown(minknockdown_dflt)
  {}

  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3) const;

  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3,
		      int same_element) const;   

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
    AddArgument(cmd, "max_stress", maxstress_dflt);
    AddArgument(cmd, "min_stress", minstress_dflt);
    AddArgument(cmd, "max_knockdown", maxknockdown_dflt);
    AddArgument(cmd, "min_knockdown", minknockdown_dflt);
  }
	
  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

//-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/-\-/
    
class Damage : public MaterialT {
protected:
  static float young_dflt, poisson_dflt, t_coeff_dflt;
  static float maxstress_dflt;
  static float knockdown1_dflt,  knockdown2_dflt;
  static float maxstressvar;
  static TrueFalse onlyonce_dflt;
  float young, poisson, t_coeff;
  float maxstress;
  float knockdown1, knockdown2;
  TrueFalse onlyonce;
#ifdef THERMAL
  static float kappa_dflt;
  float kappa;
#endif // THERMAL
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  
  Damage();
  
  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3) const;

  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3,
		      int same_element) const;   

  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa", kappa_dflt);
#endif // THERMAL
    AddArgument(cmd, "max_tension", maxstress_dflt);
    AddArgument(cmd, "variability", maxstressvar);
    AddArgument(cmd, "k.o.tension", knockdown1_dflt);
    AddArgument(cmd, "k.o.shear", knockdown2_dflt);
    AddArgument(cmd, "only_once", onlyonce_dflt);
  }
	
  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

#endif
