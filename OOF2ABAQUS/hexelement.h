// -*- C++ -*-
// $RCSfile: hexelement.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 21:24:32 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* An element with hexagonal symmetry */

#ifndef HEXELEMENT_H
#define HEXELEMENT_H

class HexElement;

#include "genelement.h"

class HexElement : public GTElement {
protected:
  double c11, c12, c13, c33, c44;
  static double c11_dflt, c12_dflt, c13_dflt, c33_dflt, c44_dflt;
  double alpha11, alpha33;
  static double alpha11_dflt, alpha33_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  HexElement() {}
  HexElement(Grid*);
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c33", c33_dflt);
    AddArgument(cmd, "c44", c44_dflt);
    AddArgument(cmd, "alpha11", alpha11_dflt);
    AddArgument(cmd, "alpha33", alpha33_dflt);
  }
  
  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};


#endif
