// -*- C++ -*-
// $RCSfile: damisoelement.h,v $
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


// damaged isotropic element

#ifndef DAMISOELEMENT_H
#define DAMISOELEMENT_H

class DamIsoElement;

#include "trielement.h"
#include "node.h"
#include "symmmatrix.h"
#include "typeregistry.h"
#include "vec.h"
#include <stdio.h>


class DamIsoElement : public TriElement {
protected:
  double young, poisson, t_coeff;
  TrueFalse planestrain;
  static double young_dflt, poisson_dflt, t_coeff_dflt;
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

  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};

#endif
