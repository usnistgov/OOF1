// -*- C++ -*-
// $RCSfile: damageelement.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 21:24:31 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Damage element, based on GTElement and MutableElement

#include "damageelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "createelement.h"
#include <math.h>

// static default data
CharString DamageElement::nameplate("damage");
double DamageElement::young_dflt(1.0);
double DamageElement::poisson_dflt(0.0);
double DamageElement::t_coeff_dflt(0.0);
double DamageElement::knockdown1_dflt(0.1);
double DamageElement::knockdown2_dflt(0.2);
double DamageElement::maxstress_dflt(0.1);
int DamageElement::Nnodes(3);

ElementTypeRegistration DamageElement::reg(nameplate,
					   DamageElement::binaryread);

// Constructor

DamageElement::DamageElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    maxstress(maxstress_dflt)
{
  orientation = 0;
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *DamageElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, kd1, kd2, mxs;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = tc;
  if(!readbinary(file, mxs)) { ok = TF_FALSE; return 0; } // maxstress (float)
  maxstress_dflt = mxs;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;
  
  // create the element
  return create_element((DamageElement*) 0);
}

CharString DamageElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
    + " maxstress=" + to_charstring(maxstress)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

bool DamageElement::same_type(const Element *el) const {
  const DamageElement *other = dynamic_cast<const DamageElement*>(el);
  return el && GTElement::same_type(el)
    && MutableElement::same_type(el)
    && poisson == other->poisson
    && young == other->young
    && t_coeff == other->t_coeff
    && maxstress == other->maxstress
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2;
}
