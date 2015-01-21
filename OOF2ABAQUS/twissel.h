// -*- C++ -*-
// $RCSfile: twissel.h,v $
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


// twissel element

// this isn't derived from the GTElement to avoid unnecessary
// computational overhead.

#ifndef TWISELEMENT_H
#define TWISELEMENT_H

class TwisElement;

#include "trielement.h"
#include "mvvd.h"
#include "node.h"
#include "typeregistry.h"
#include "vec.h"
#include <stdio.h>


// Twistropic Element class

class TwisElement : public TriElement {
protected:
  double young, poisson, t_coeff, fictT_coeff;
  static double young_dflt, poisson_dflt, t_coeff_dflt, fictT_coeff_dflt;
  static TrueFalse planestrain_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static MV_Vector_double sfs_dflt;
private:
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  TwisElement() {}
  TwisElement(Grid*);
  virtual ~TwisElement() {}
  
  static int Nnodes;
  
  virtual const CharString &tag() const { return nameplate; }
  static void AddArgs(CommandM *cmd) {
    TriElement::AddArgs(cmd);
    AddArgument(cmd, "planestrain", planestrain_dflt);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
    AddArgument(cmd, "vegard", fictT_coeff_dflt);
  }
  virtual void OptionalArgHandler() {
    TriElement::OptionalArgHandler();
  }
  
  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};

#endif
