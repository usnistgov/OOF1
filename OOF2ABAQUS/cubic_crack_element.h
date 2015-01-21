// -*- C++ -*-
// $RCSfile: cubic_crack_element.h,v $
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


/* Cubic element with Griffith2 mutation */

#ifndef CUBICCRACK_H
#define CUBICCRACK_H

class CubicCrackElement;

#include "genelement.h"
#include "mutableelement.h"

class CubicCrackElement : public GTElement, public MutableElement {
protected:
  double young, poisson, t_coeff, anisotropy;
  static double young_dflt, poisson_dflt, t_coeff_dflt, anisotropy_dflt;
  double  knockdown1, knockdown2, gamma;
  static double knockdown1_dflt, knockdown2_dflt, gamma_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  CubicCrackElement() {}
  CubicCrackElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
    AddArgument(cmd, "anisotropy", anisotropy_dflt);
    AddArgument(cmd, "gamma", gamma_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    MutableElement::AddArgs(cmd);
  }
  
  virtual CharString parameters() const;

  virtual bool same_type(const Element*) const;
};

#endif
