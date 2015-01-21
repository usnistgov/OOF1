// -*- C++ -*-
// $RCSfile: isoelement.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-22 15:15:25 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// isotropic element

// this isn't derived from the GTElement to avoid unnecessary
// computational overhead.

#ifndef ISOELEMENT_H
#define ISOELEMENT_H

class IsoElement;

#include "trielement.h"
#include "node.h"
#include "typeregistry.h"
#include "vec.h"
#include <stdio.h>


// Isotropic Element class

class IsoElement : public TriElement {
protected:
  double young, poisson, t_coeff;
  static double young_dflt, poisson_dflt, t_coeff_dflt;
  static TrueFalse planestrain_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  virtual bool same_type(const Element*) const;
private:
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  IsoElement() {}
  IsoElement(Grid*);
  virtual ~IsoElement() {}
  
  static int Nnodes;
  
  virtual const CharString &tag() const { return nameplate; }
  static void AddArgs(CommandM *cmd) {
    TriElement::AddArgs(cmd);
    AddArgument(cmd, "planestrain", planestrain_dflt);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
  }
  virtual void OptionalArgHandler() {
    TriElement::OptionalArgHandler();
  }
  
  virtual CharString parameters() const;
};

#endif
