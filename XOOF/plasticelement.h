// -*- C++ -*-
// $RCSfile: plasticelement.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:10 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* Plastic element */

#ifndef PLASTICELEMENT_H
#define PLASTICELEMENT_H

class PlasticElement;

#include "genelement.h"
#include "mutableelement.h"

class PlasticElement : public GTElement, public MutableElement {
protected:
  double young, poisson, t_coeff, stress_limit, hardening_coefficient;
  static double young_dflt, poisson_dflt, t_coeff_dflt,
    stress_limit_dflt, hardening_coefficient_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
  Cijkl virgin_cijkl() const;
public:
  PlasticElement() {}
  PlasticElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
    AddArgument(cmd, "plastic stress limit", stress_limit_dflt);
    AddArgument(cmd, "Softening Coefficient", hardening_coefficient_dflt);
    MutableElement::AddArgs(cmd);
  }
  
  virtual void binarywrite(FILE*, char) const;

  virtual CharString parameters() const;

  virtual Cijkl setCijkl() const;
  virtual SymmMatrix setAlpha() const;

  virtual int adjust_young(const float);
  virtual int mutate();
  virtual void revert();

  friend class Debug;
};


#endif





