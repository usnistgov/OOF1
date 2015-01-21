// -*- C++ -*-
// $RCSfile: twissel.h,v $
// $Revision: 1.3 $
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


// twissel element

// this isn't derived from the GTElement to avoid unnecessary
// computational overhead.

#ifndef TWISELEMENT_H
#define TWISELEMENT_H

class TwisElement;

#include "debug.h"
#include "trielementT.h"
#include "node.h"
#include "coord_double.h"
#include "symmmatrix.h"
#include "twotwo.h"
#include "typeregistry.h"
#include "vec.h"
#include <stdio.h>


// Twistropic Element class

class TwisElement : public TriElementT {
protected:
  double young, poisson, t_coeff, fictT_coeff;
  TrueFalse planestrain;
  static double young_dflt, poisson_dflt, t_coeff_dflt, fictT_coeff_dflt;
  static TrueFalse planestrain_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
#ifdef THERMAL
  double kappa;
  static double kappa_dflt;
#endif // THERMAL
  virtual bool same_type(const Element*) const;
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
    TriElementT::AddArgs(cmd);
    AddArgument(cmd, "planestrain", planestrain_dflt);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
    AddArgument(cmd, "vegard", fictT_coeff_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa", kappa_dflt);
#endif // THERMAL
  }
  virtual void OptionalArgHandler() {
    TriElement::OptionalArgHandler();
  }
  
  virtual void binarywrite(FILE*, char) const;
  virtual std::vector<CharString> *print_properties(ostream&) const;
  virtual CharString parameters() const;
  //  virtual StressStrain avgstress();
  //  virtual StressStrain avgstrain();
  virtual const StrainStress &elastic_straintensor();
  virtual StrainStress total_straintensor();
  virtual const StrainStress &stresstensor();
#ifdef THERMAL
  virtual const MV_Vector_double &gradient_temperature_field();
  virtual const MV_Vector_double &heat_flux_field();
#endif // THERMAL

  virtual int mutate() { return 0; }

  // override some TriElement virtual functions. The Twissel versions
  // depend on the locally defined stress free strain.
  virtual void loadinterior_rhs(MV_Vector_double&) const;
  virtual void loadboundary_rhs(MV_Vector_double&) const;
  virtual MeshCoord force(const Node*) const;
  
protected:
  MV_Vector_double sfs_forces;	// stress free strain
  static MV_Vector_double sfs_dflt;

  // local stiffness matrix
  virtual void setstiffness();
  void setstiffness(MV_ColMat_double*);
#ifdef THERMAL
  virtual void setstiffnessT();
#endif // THERMAL
  
  friend class Debug;
};

#endif
