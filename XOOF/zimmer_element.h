// -*- C++ -*-
// $RCSfile: zimmer_element.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2004-10-19 21:29:20 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* Zimmer element */

#ifndef ZIMMERELEMENT_H
#define ZIMMERELEMENT_H

class ZimmerElement;
class ZimmerElement2;

#include "genelement.h"
#include "mutableelement.h"

class ZimmerElement : public GTElement, public MutableElement {
protected:
  double c11, c12, c13, c33, c44;
  static double c11_dflt, c12_dflt, c13_dflt, c33_dflt, c44_dflt;
  double alpha11, alpha33;
  static double alpha11_dflt, alpha33_dflt;
#ifdef THERMAL
  double kappa11, kappa33;
  static double kappa11_dflt, kappa33_dflt;
#endif
  double  knockdown1, knockdown2, gamma;
  static double knockdown1_dflt, knockdown2_dflt, gamma_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
  Cijkl virgin_cijkl() const;
  virtual bool same_type(const Element*) const;
public:
  ZimmerElement() {}
  ZimmerElement(Grid*);
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
#ifdef THERMAL
    AddArgument(cmd, "kappa11", kappa11_dflt);
    AddArgument(cmd, "kappa33", kappa33_dflt);
#endif
    AddArgument(cmd, "gamma", gamma_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    MutableElement::AddArgs(cmd);
  }
  
  virtual void binarywrite(FILE*, char) const;
  virtual std::vector<CharString> *print_properties(ostream&) const;
  virtual CharString parameters() const;

  virtual Cijkl setCijkl() const;
  virtual SymmMatrix setAlpha() const;
#ifdef THERMAL
  virtual Kij setKij() const;
#endif

  virtual int mutate();
  virtual void revert();

  friend class Debug;
};

class ZimmerElement2 : public ZimmerElement {
private:
  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  ZimmerElement2() {}
  ZimmerElement2(Grid *g) : ZimmerElement(g) {}
  virtual const CharString &tag() const { return nameplate; }
  virtual int mutate();
};

#endif
