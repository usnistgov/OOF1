// -*- C++ -*-
// $RCSfile: genelement.h,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2004-10-19 02:24:35 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// general triangular element

#ifndef GTELEMENT_H
#define GTELEMENT_H

class GTElement;

#include "cijkl.h"
#include "kij.h"
#include "debug.h"
#include "enum.h"
#include "eulerangle.h"
#include "trielementT.h"
#include "node.h"
#include "coord_double.h"
#include "planarity.h"
#include "strainstress.h"
#include "symmmatrix.h"
#include "twotwo.h"
#include "typeregistry.h"
#include "vec.h"
#include <stdio.h>


class GTElement : public TriElementT {
protected:
  EulerAngle orientation;
  TrueFalse planestrain;
  static TrueFalse planestrain_dflt;
  static EulerAngle orientation_dflt;
#ifdef THERMAL
  Enum<ThermalPlanarity> thermal_planarity;
  static Enum<ThermalPlanarity> thermal_planarity_dflt;
#endif // THERMAL
  static TrueFalse Kset;  // has stiffness been set yet?
  static Element *binaryread(FILE*, TrueFalse&);
  virtual bool same_type(const Element*) const;
public:
  GTElement() {}
  GTElement(Grid*);
  virtual ~GTElement() {}
  
  virtual const CharString &tag() const = 0;
  
  static void AddArgs(CommandM *cmd) {
    TriElementT::AddArgs(cmd);
    AddArgument(cmd, "planestrain", planestrain_dflt);
#ifdef THERMAL
    AddArgument(cmd, "T_planarity", thermal_planarity_dflt);
#endif // THERMAL
    AddArgument(cmd, "orientation", orientation_dflt);

  }
  virtual void OptionalArgHandler() {
    TriElement::OptionalArgHandler();
  }
  
  virtual void binarywrite(FILE*, char) const;
  std::vector<CharString> *print_properties(ostream&) const;
  
  virtual CharString parameters() const;

  virtual const StrainStress &elastic_straintensor();
  virtual StrainStress total_straintensor();
  virtual const StrainStress &stresstensor();
#ifdef THERMAL
  virtual const MV_Vector_double &gradient_temperature_field();
  virtual const MV_Vector_double &heat_flux_field();
#endif // THERMAL
  virtual EulerAngle query_orientation() const {return orientation;}

  virtual bool defines_planestrain() const { return true; }
  virtual bool get_planestrain() const { return planestrain; }
#ifdef THERMAL
  virtual bool defines_Tplanarity() const { return true; }
  virtual bool get_Tplanarity() const { return thermal_planarity; }
#endif // THERMAL
  
protected:
  // local stiffness matrix
  virtual void setstiffness();
  void setstiffness(MV_ColMat_double*);
  virtual Cijkl setCijkl() const = 0;
#ifdef THERMAL
  virtual void setstiffnessT();
  virtual Kij setKij() const = 0;
#endif // THERMAL
  // thermal expansion coefficients
  virtual SymmMatrix setAlpha() const = 0;
  
  friend class Debug;
};

#endif







