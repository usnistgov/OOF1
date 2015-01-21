// -*- C++ -*-
// $RCSfile: orthorhombic.h,v $
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


/* An orthorhombic element */

#ifndef ORTHOELEMENT_H
#define ORTHOELEMENT_H

class OrthoElement;

#include "genelement.h"

class OrthoElement : public GTElement {
protected:
  double c11, c12, c13, c22, c23, c33, c44, c55, c66;
  static double c11_dflt, c12_dflt, c13_dflt, c22_dflt, c23_dflt,
    c33_dflt, c44_dflt, c55_dflt, c66_dflt;
  double alpha11, alpha22, alpha33;
  static double alpha11_dflt, alpha22_dflt, alpha33_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  OrthoElement() {}
  OrthoElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c22", c22_dflt);
    AddArgument(cmd, "c23", c23_dflt);
    AddArgument(cmd, "c33", c33_dflt);
    AddArgument(cmd, "c44", c44_dflt);
    AddArgument(cmd, "c55", c55_dflt);
    AddArgument(cmd, "c66", c66_dflt);
    AddArgument(cmd, "alpha11", alpha11_dflt);
    AddArgument(cmd, "alpha22", alpha22_dflt);
    AddArgument(cmd, "alpha33", alpha33_dflt);
  }
  
  virtual CharString parameters() const;
  virtual bool same_type(const Element *) const;
};


#endif
