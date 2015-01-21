// -*- C++ -*-
// $RCSfile: griffith_element.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-26 14:36:48 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Griffith element, based on GTElement with energy release criterion

#include "createelement.h"
#include "griffith_element.h"
#include "inputflags.h"
#include "readbinary.h"
#include <math.h>

// static default data
CharString GriffithElement::nameplate("griffith");
CharString GriffithElement2::nameplate("griffith2");

double GriffithElement::young_dflt(1.0);
double GriffithElement::poisson_dflt(0.0);
double GriffithElement::t_coeff_dflt(0.0);
double GriffithElement::knockdown1_dflt(0.1);
double GriffithElement::knockdown2_dflt(0.2);
double GriffithElement::gamma_dflt(0.1);
int GriffithElement::Nnodes(3);

ElementTypeRegistration GriffithElement::reg(nameplate,
					     GriffithElement::binaryread);
ElementTypeRegistration GriffithElement2::reg(nameplate,
					      GriffithElement2::binaryread);

// Constructor

GriffithElement::GriffithElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    gamma(gamma_dflt)
{
  orientation = 0;
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *GriffithElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, kd1, kd2, gma;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = tc;
  if(!readbinary(file, gma)) { ok = TF_FALSE; return 0; } // gamma (float)
  gamma_dflt = gma;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;

  // create the element
  return create_element((GriffithElement*) 0);
}

Element *GriffithElement2::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, kd1, kd2, gma;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = tc;
  if(!readbinary(file, gma)) { ok = TF_FALSE; return 0; } // gamma (float)
  gamma_dflt = gma;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;

  // create the element
  return create_element((GriffithElement2*) 0);
}

CharString GriffithElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
    + " gamma=" + to_charstring(gamma)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

bool GriffithElement::same_type(const Element *el) const {
  const GriffithElement *other = dynamic_cast<const GriffithElement*>(el);
  return other && GTElement::same_type(el)
    && MutableElement::same_type(el)
    && poisson == other->poisson
    && young == other->young
    && t_coeff == other->t_coeff
    && gamma == other->gamma
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2;
}

bool GriffithElement2::same_type(const Element *el) const {
  const GriffithElement2 *other = dynamic_cast<const GriffithElement2*>(el);
  return other && GriffithElement::same_type(el);
}
