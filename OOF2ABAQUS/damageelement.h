// -*- C++ -*-
// $RCSfile: damageelement.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 21:24:31 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* Damage element */

#ifndef DAMAGEELEMENT_H
#define DAMAGEELEMENT_H

class DamageElement;

#include "genelement.h"
#include "mutableelement.h"

class DamageElement : public GTElement, public MutableElement {
protected:
  double young, poisson, t_coeff, knockdown1, knockdown2, maxstress;
  static double young_dflt, poisson_dflt, t_coeff_dflt,
    knockdown1_dflt, knockdown2_dflt, maxstress_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  DamageElement() {}
  DamageElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
    AddArgument(cmd, "maxstress", maxstress_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    MutableElement::AddArgs(cmd);
  }
  

  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};


#endif
