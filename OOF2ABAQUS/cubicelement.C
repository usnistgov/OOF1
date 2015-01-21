// -*- C++ -*-
// $RCSfile: cubicelement.C,v $
// $Revision: 1.2 $
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


// Cubic element, based on GTElement

#include "cubicelement.h"
#include "readbinary.h"
#include "createelement.h"

// static default data
CharString CubicElement::nameplate("cubic");
double CubicElement::young_dflt(1.0);
double CubicElement::poisson_dflt(0.0);
double CubicElement::t_coeff_dflt(0.0);
#ifdef THERMAL
double CubicElement::kappa_dflt(0.0);
#endif // THERMAL
double CubicElement::anisotropy_dflt(1.0);
int CubicElement::Nnodes(3);

ElementTypeRegistration CubicElement::reg(nameplate,
					  CubicElement::binaryread);

// Constructor

CubicElement::CubicElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    anisotropy(anisotropy_dflt)
{
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *CubicElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, aniso;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = tc;
  if(!readbinary(file, aniso)) { ok = TF_FALSE; return 0; } //anisotropy(float)
  anisotropy_dflt = aniso;

  // create the element
  return create_element((CubicElement*) 0);
}

CharString CubicElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
    + " anisotropy=" + to_charstring(anisotropy);
}

bool CubicElement::same_type(const Element *el) const {
  const CubicElement *other = dynamic_cast<const CubicElement*>(el);
  return other && GTElement::same_type(el)
    && poisson == other->poisson
    && young == other->young
    && t_coeff == other->t_coeff
    && anisotropy == other->anisotropy;
}
