// -*- C++ -*-
// $RCSfile: cubicelement.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-26 14:36:48 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* A cubic element */

#ifndef CUBICELEMENT_H
#define CUBICELEMENT_H

class CubicElement;

#include "genelement.h"

class CubicElement : public GTElement {
protected:
  double young, poisson, t_coeff, anisotropy;
  static double young_dflt, poisson_dflt, t_coeff_dflt, anisotropy_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  CubicElement() {}
  CubicElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
    AddArgument(cmd, "anisotropy", anisotropy_dflt);
  }
  
  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
  friend class Debug;
};


#endif
