// -*- C++ -*-
// $RCSfile: twissel.C,v $
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

#include "charstring.h"
#include "createelement.h"
#include "element.h"
#include "grid.h"
#include "inputflags.h"
#include "twissel.h"
#include "node.h"
#include "readbinary.h"
#include "mvvd.h"
#include <iostream.h>

// static default data
CharString TwisElement::nameplate("twissel");
double TwisElement::young_dflt(1.0);
double TwisElement::poisson_dflt(0.0);
double TwisElement::t_coeff_dflt(0.0);
double TwisElement::fictT_coeff_dflt(0.0);
TrueFalse TwisElement::planestrain_dflt(0);

MV_Vector_double TwisElement::sfs_dflt(6, 0.0);

int TwisElement::Nnodes(3);

ElementTypeRegistration TwisElement::reg(nameplate, TwisElement::binaryread);

// ------------ Constructor for Twistropic Element. ----------------//

TwisElement::TwisElement(Grid *g)
  : TriElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    fictT_coeff(fictT_coeff_dflt)
{
  planestrain = planestrain_dflt;
}

Element *TwisElement::binaryread(FILE *file, TrueFalse &ok) {
  TriElement::binaryread(file, ok);
  if(!ok) return 0;
  
  float psn, yng, tc, ftc;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;	// float to double
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;	// float to double
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; }  // alpha (float)
  t_coeff_dflt = tc;
  if(!readbinary(file, ftc)) { ok = TF_FALSE; return 0; }  // alpha (float)
  fictT_coeff_dflt = ftc;
  
  if(!readbinary(file, inputformatflag)) { ok = TF_FALSE; return 0; }
  // flag (char)
  planestrain_dflt = inputformatflag & PLANESTRAIN;
  
  if(inputformatflag & STIFFNESS) {
    if(!K_dflt.binaryread(file)) {		// stiffness matrix
      ok = TF_FALSE;
      return 0;
    }
    for(int i=0; i<6; i++)
      if(!readbinary(file, thermal_dflt(i))) {	// thermal forces
	ok = TF_FALSE;
	return 0;
      }
    for(int i=0; i<6; i++)	// stress free strain forces
      if(!readbinary(file, sfs_dflt(i))) {
	ok = TF_FALSE;
	return 0;
      }
  }
  else if(inputformatflag & DUPLICATE) {
    if(!readbinary(file, cloneindex)) { ok = TF_FALSE; return 0; }
  }
  return create_element((TwisElement*)0);
}


//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

CharString TwisElement::parameters() const {
  return TriElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
    + " vegard=" + to_charstring(fictT_coeff)
    + " planestrain=" + to_charstring(planestrain);
}

bool TwisElement::same_type(const Element *el) const {
  const TwisElement *other = dynamic_cast<const TwisElement*>(el);
  return other && TriElement::same_type(el)
    && poisson == other->poisson
    && young == other->young
    && t_coeff == other->t_coeff
    && fictT_coeff == other->fictT_coeff
    && planestrain == other->planestrain;
}
