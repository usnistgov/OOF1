// -*- C++ -*-
// $RCSfile: trigonal.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 21:24:33 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modified
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

/* trigonal element */

#ifndef TRIGONAL_H
#define TRIGONAL_H

class TrigonalAElement;
class TrigonalBElement;

#include "genelement.h"
#include "mutableelement.h"

class TrigonalAElement : public GTElement {
protected:
  double c11, c12, c13, c14, c15, c33, c44;
  static double c11_dflt, c12_dflt, c13_dflt, c14_dflt, c15_dflt,
    c33_dflt, c44_dflt;
  double alpha11, alpha33;
  static double alpha11_dflt, alpha33_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  TrigonalAElement() {}
  TrigonalAElement(Grid*);
  virtual ~TrigonalAElement() {}
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c14", c14_dflt);
    AddArgument(cmd, "c15", c15_dflt);
    AddArgument(cmd, "c33", c33_dflt);
    AddArgument(cmd, "c44", c44_dflt);
    AddArgument(cmd, "alpha11", alpha11_dflt);
    AddArgument(cmd, "alpha33", alpha33_dflt);
  }

  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};

class TrigonalBElement : public GTElement {
protected:
  double c11, c12, c13, c33, c44, c14;
  static double c11_dflt, c12_dflt, c13_dflt, c14_dflt, c33_dflt, c44_dflt;
  double alpha11, alpha33;
  static double alpha11_dflt, alpha33_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  TrigonalBElement() {}
  TrigonalBElement(Grid*);
  virtual ~TrigonalBElement() {}
  static int Nnodes;
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    GTElement::AddArgs(cmd);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c33", c33_dflt);
    AddArgument(cmd, "c44", c44_dflt);
    AddArgument(cmd, "c14", c14_dflt);
    AddArgument(cmd, "alpha11", alpha11_dflt);
    AddArgument(cmd, "alpha33", alpha33_dflt);
  }

  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};

class TrigonalCrack : public TrigonalAElement, public MutableElement {
protected:
  double knockdown1, knockdown2, gamma;
  static double knockdown1_dflt, knockdown2_dflt, gamma_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  TrigonalCrack() {}
  TrigonalCrack(Grid*);
  virtual ~TrigonalCrack() {}
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd) {
    TrigonalAElement::AddArgs(cmd);
    AddArgument(cmd, "gamma", gamma_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    MutableElement::AddArgs(cmd);
  }

  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};

#endif
