// -*- C++ -*-
// $RCSfile: damisoelement.h,v $
// $Revision: 1.5 $
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


// damaged isotropic element

#ifndef DAMISOELEMENT_H
#define DAMISOELEMENT_H

class DamIsoElement;

#include "debug.h"
#include "trielementT.h"
#include "node.h"
#include "coord_double.h"
#include "symmmatrix.h"
#include "twotwo.h"
#include "typeregistry.h"
#include "vec.h"
#include <stdio.h>


class DamIsoElement : public TriElementT {
protected:
  double young, poisson, t_coeff;
  TrueFalse planestrain;
  static double young_dflt, poisson_dflt, t_coeff_dflt;
#ifdef THERMAL
  double kappa;
  static double kappa_dflt;
#endif // THERMAL
  static TrueFalse planestrain_dflt;
  double maxstress, minstress, maxknockdown, minknockdown;
  static double maxstress_dflt, minstress_dflt;
  static double maxknockdown_dflt, minknockdown_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  
  double total_knockdown;	// product of all applied knockdowns
  static double total_knockdown_dflt;

  // used to save the unmutated state
  float oldgray;
  static float oldgray_dflt;

  virtual bool same_type(const Element*) const;

private:
  static ElementTypeRegistration reg;
  static CharString nameplate;
public:
  DamIsoElement() {}
  DamIsoElement(Grid*);
  virtual ~DamIsoElement() {}
  
  static int Nnodes;
  
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd);

  virtual void binarywrite(FILE*, char) const;
  virtual std::vector<CharString> *print_properties(ostream &os) const;
  
  virtual CharString parameters() const;
  virtual const StrainStress &elastic_straintensor();
  virtual StrainStress total_straintensor();
  virtual const StrainStress &stresstensor();
#ifdef THERMAL
  virtual const MV_Vector_double &gradient_temperature_field();
  virtual const MV_Vector_double &heat_flux_field();
#endif // THERMAL
  
  virtual int mutate();
  virtual void revert();
  
protected:
  // local stiffness matrix
  virtual void setstiffness();
  void setstiffness(MV_ColMat_double*);
#ifdef THERMAL
  void setstiffnessT();
#endif // THERMAL
  void knockdown(double);
  
  friend class Debug;
};

#endif
