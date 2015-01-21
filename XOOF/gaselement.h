// -*- C++ -*-
// $RCSfile: gaselement.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2005-02-15 20:26:02 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// gas element -- thermal conductivity but no elasticity

// this isn't derived from the GTElement to avoid unnecessary
// computational overhead.

#ifndef GASELEMENT_H
#define GASELEMENT_H

class GasElement;

#include "debug.h"
#include "trielement.h"
#include "node.h"
#include "coord_double.h"
#include "symmmatrix.h"
#include "twotwo.h"
#include "typeregistry.h"
#include "vec.h"
#include <stdio.h>

#ifdef THERMAL

class GasElement : public TriElement {
protected:
  double kappa;
  static double kappa_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static StrainStress nostress;
  virtual bool same_type(const Element*) const;
private:
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  GasElement() {}
  GasElement(Grid*);
  virtual ~GasElement() {}
  
  static int Nnodes;
  
  virtual const CharString &tag() const { return nameplate; }
  static void AddArgs(CommandM *cmd) {
    TriElement::AddArgs(cmd);
    AddArgument(cmd, "kappa", kappa_dflt);
  }
  virtual void OptionalArgHandler() {
    TriElement::OptionalArgHandler();
  }
  
  virtual void binarywrite(FILE*, char) const;
  virtual std::vector<CharString> *print_properties(ostream&) const;
  
  virtual CharString parameters() const;
  virtual StrainStress total_straintensor() { return nostress; }
  virtual const StrainStress &stresstensor() { return nostress; }
  virtual const MV_Vector_double &gradient_temperature_field();
  virtual const MV_Vector_double &heat_flux_field();
  double average_T() const;
  virtual MeshCoord force(const Node*) const;

  virtual int mutate() { return 0; }
  
protected:
  // local stiffness matrix
  virtual void setstiffness();
  virtual void setstiffnessT();
  virtual void loadinterior(SparseLinkMat&) const {}
  virtual void loadboundary(SparseLinkMat&,
			    SparseLinkMat&, SparseLinkMat&) const {}
  virtual void loadinterior_rhs(MV_Vector_double&) const {}
  virtual void loadboundary_rhs(MV_Vector_double&) const {}

  virtual bool isempty() const { return true; }
  
  friend class Debug;
};

#endif // THERMAL

#endif // GASELEMENT_H
