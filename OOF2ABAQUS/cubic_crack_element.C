// -*- C++ -*-
// $RCSfile: cubic_crack_element.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 21:24:30 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



// Element with Cubic symmetry,
// based on GTElement with energy release criterion

 //note that the thermal coefficients are not knocked down

#include "createelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "cubic_crack_element.h"
#include <math.h>

// static default data
CharString CubicCrackElement::nameplate("cubic_crack");

double CubicCrackElement::young_dflt(1.0);
double CubicCrackElement::poisson_dflt(0.0);
double CubicCrackElement::t_coeff_dflt(0.0);
double CubicCrackElement::anisotropy_dflt(1.0);
double CubicCrackElement::knockdown1_dflt(0.1);
double CubicCrackElement::knockdown2_dflt(0.2);
double CubicCrackElement::gamma_dflt(0.1);
int CubicCrackElement::Nnodes(3);

ElementTypeRegistration CubicCrackElement::reg(nameplate,
					       CubicCrackElement::binaryread);

CubicCrackElement::CubicCrackElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    anisotropy(anisotropy_dflt),
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    gamma(gamma_dflt)
{
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *CubicCrackElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.
  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // anisotropy (float)
  anisotropy_dflt = c;  // float to double 

  float  kd1, kd2, gma;

  if(!readbinary(file, gma)) { ok = TF_FALSE; return 0; } // maxstress (float)
  gamma_dflt = gma;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;

  // create the element
  return create_element((CubicCrackElement*) 0);
}


CharString CubicCrackElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
    + " anisotropy=" + to_charstring(anisotropy)
    + " gamma=" + to_charstring(gamma)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

bool CubicCrackElement::same_type(const Element *el) const {
  const CubicCrackElement *other = dynamic_cast<const CubicCrackElement*>(el);
  return other && GTElement::same_type(el)
    && MutableElement::same_type(el)
    && poisson == other->poisson
    && young == other->young
    && t_coeff == other->t_coeff
    && anisotropy == other->anisotropy
    && gamma == other->gamma
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2;
}
