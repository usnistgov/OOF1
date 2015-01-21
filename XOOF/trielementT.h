// -*- C++ -*-
// $RCSfile: trielementT.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2005-02-17 21:24:48 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef TRIELEMENTT_H
#define TRIELEMENTT_H

// Linear triangular element with a temperature0 parameter

// For !THERMAL, this is a useless class, but it's easiest to keep it
// here rather than having the class heirarchy depend on THERMAL.

#include "trielement.h"

class TriElementT : public TriElement {
protected:
#ifdef THERMAL
  double temperature0;
  static double temperature0_dflt;
#endif // THERMAL
  static Element *binaryread(FILE*, TrueFalse&);
  virtual bool same_type(const Element*) const;
public:
  TriElementT() {}
  TriElementT(Grid*);
  virtual ~TriElementT() {}

  static void AddArgs(CommandM *cmd) {
    TriElement::AddArgs(cmd);
#ifdef THERMAL
    AddArgument(cmd, "T0", temperature0_dflt);
#endif // THERMAL
  }

  virtual void binarywrite(FILE*, char) const;
  virtual std::vector<CharString> *print_properties(ostream&) const;
  virtual CharString parameters() const;

  virtual MeshCoord force(const Node*) const;
  virtual void loadinterior_rhs(MV_Vector_double&) const;
  virtual void loadboundary_rhs(MV_Vector_double&) const;

#ifdef THERMAL
  virtual double average_T() const;
#endif // THERMAL
  virtual double deltaT() const;
};

#endif
