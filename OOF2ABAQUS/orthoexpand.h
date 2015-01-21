// -*- C++ -*-
// $RCSfile: orthoexpand.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-26 14:36:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* An isotropic element that can expand anisotropically */

#ifndef ORTHOEXELEMENT_H
#define ORTHOEXELEMENT_H

class OrthoExElement;

#include "genelement.h"

class OrthoExElement : public GTElement {
protected:
  double young, poisson, a1, a2, a3;
  static double young_dflt, poisson_dflt, a1_dflt, a2_dflt, a3_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  OrthoExElement() {}
  OrthoExElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "a1", a1_dflt);
    AddArgument(cmd, "a2", a2_dflt);
    AddArgument(cmd, "a3", a3_dflt);
  }
  
  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};


#endif
