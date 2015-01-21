// -*- C++ -*-
// $RCSfile: valeria.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2004-10-19 21:29:20 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */

/* Valeria element: Designed to emulate random fractures using Weibull
parameters to stachastically initiate mutation with element */

#ifndef VALERIA_H
#define VALERIA_H

static int valeria_seed = 0;
class ValeriaElement;


#include "genelement.h"
#include "mutableelement.h"

class ValeriaElement : public GTElement, public MutableElement {
protected:
  double young, poisson, t_coeff, knockdown1, knockdown2,
    weibull_m, weibull_so, weibull_ao;
  static double young_dflt, poisson_dflt, t_coeff_dflt,
    knockdown1_dflt, knockdown2_dflt, weibull_m_dflt,
    weibull_so_dflt, weibull_ao_dflt;
#ifdef THERMAL
  double kappa;
  static double kappa_dflt;
#endif // THERMAL
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
  Cijkl virgin_cijkl() const;
  virtual bool same_type(const Element*) const;
public:
  ValeriaElement() {};
  ValeriaElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
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
    MutableElement::AddArgs(cmd);
  }
  
  virtual void binarywrite(FILE*, char) const;
  virtual std::vector<CharString> *print_properties(ostream&) const;
  virtual CharString parameters() const;

  virtual Cijkl setCijkl() const;
  virtual SymmMatrix setAlpha() const;
#ifdef THERMAL
  virtual Kij setKij() const;
#endif // THERMAL

  virtual int mutate();
  virtual void revert();

  friend class Debug;
};



#endif
