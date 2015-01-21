// -*- C++ -*-
// $RCSfile: zimmer_element.C,v $
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



// Andrez Zimmermann's  Griffith element with Hexagonal symmetry,
// based on GTElement with energy release criterion

 //note that the thermal coefficients are not knocked down

#include "createelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "zimmer_element.h"
#include <math.h>

// static default data
CharString ZimmerElement::nameplate("zimmer");
CharString ZimmerElement2::nameplate("zimmer2");

double ZimmerElement::c11_dflt(1.0);
double ZimmerElement::c12_dflt(0.0);
double ZimmerElement::c13_dflt(0.0);
double ZimmerElement::c33_dflt(1.0);
double ZimmerElement::c44_dflt(0.5);
double ZimmerElement::alpha11_dflt(1.0);
double ZimmerElement::alpha33_dflt(1.0);
double ZimmerElement::knockdown1_dflt(0.1);
double ZimmerElement::knockdown2_dflt(0.2);
double ZimmerElement::gamma_dflt(0.1);
int ZimmerElement::Nnodes(3);

ElementTypeRegistration ZimmerElement::reg(nameplate,
					   ZimmerElement::binaryread);
ElementTypeRegistration ZimmerElement2::reg(nameplate,
					   ZimmerElement2::binaryread);

// Constructor

ZimmerElement::ZimmerElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    alpha11(alpha11_dflt),
    alpha33(alpha33_dflt),
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    gamma(gamma_dflt)
{
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *ZimmerElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.


  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c11 (float)
  c11_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c12 (float)
  c12_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c13 (float)
  c13_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c33 (float)
  c33_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c44 (float)
  c44_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha11 (float)
  alpha11_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha33 (float)
  alpha33_dflt = c;  // float to double 

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
  return create_element((ZimmerElement*) 0);
}

Element *ZimmerElement2::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.


  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c11 (float)
  c11_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c12 (float)
  c12_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c13 (float)
  c13_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c33 (float)
  c33_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c44 (float)
  c44_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha11 (float)
  alpha11_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha33 (float)
  alpha33_dflt = c;  // float to double 
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
  return create_element((ZimmerElement2*) 0);
}

CharString ZimmerElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c33=" + to_charstring(c33)
    + " c44=" + to_charstring(c44)
    + " alpha11=" + to_charstring(alpha11)
    + " alpha33=" + to_charstring(alpha33)
    + " gamma=" + to_charstring(gamma)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

bool ZimmerElement::same_type(const Element *el) const {
  const ZimmerElement *other = dynamic_cast<const ZimmerElement*>(el);
  return other && GTElement::same_type(el)
    && MutableElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c33 == other->c33
    && c44 == other->c44
    && alpha11 == other->alpha11
    && alpha33 == other->alpha33
    && gamma == other->gamma
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2;
}

bool ZimmerElement2::same_type(const Element *el) const {
  const ZimmerElement2 *other = dynamic_cast<const ZimmerElement2*>(el);
  return other && ZimmerElement::same_type(el);
}


