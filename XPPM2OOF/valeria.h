// -*- C++ -*-
// $RCSfile: valeria.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2004-10-22 13:51:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef VALERIA_H
#define VALERIA_H

#include "materialT.h"
#include "menuDef.h"
#include "eulerangle.h"

class ValeriaMaterial : public MaterialT {
private:
  static float young_dflt, poisson_dflt, t_coeff_dflt,
    knockdown1_dflt, knockdown2_dflt, weibull_m_dflt,
    weibull_so_dflt, weibull_ao_dflt;
  float young, poisson, t_coeff,
    knockdown1, knockdown2, weibull_m,
    weibull_so, weibull_ao;
  static TrueFalse onlyonce_dflt;
  TrueFalse onlyonce;
#ifdef THERMAL
  static float kappa_dflt;
  float kappa;
#endif // THERMAL

  static MaterialTypeRegistration reg;
  static CharString type;

public:
  ValeriaMaterial() :
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    weibull_m(weibull_m_dflt),
    weibull_so(weibull_so_dflt),
    weibull_ao(weibull_ao_dflt),
    onlyonce(onlyonce_dflt)
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
    AddArgument(cmd, "weibull_m", weibull_m_dflt);
    AddArgument(cmd, "weibull_so", weibull_so_dflt);
    AddArgument(cmd, "weibull_ao", weibull_ao_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    AddArgument(cmd, "only_once", onlyonce_dflt);
  }

  virtual CharString &tag() const { return type; }
  virtual MaterialTypeRegistration *get_reg() const { return &reg; }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

#endif // VALERIA_H
