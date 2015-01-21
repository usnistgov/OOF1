// -*- C++ -*-
// $RCSfile: hexelement.C,v $
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


// Hexagonal element, based on GTElement

#include "hexelement.h"
#include "readbinary.h"
#include "createelement.h"

// static default data
CharString HexElement::nameplate("hexagonal");
double HexElement::c11_dflt(1.0);
double HexElement::c12_dflt(0.0);
double HexElement::c13_dflt(0.0);
double HexElement::c33_dflt(1.0);
double HexElement::c44_dflt(0.5);
double HexElement::alpha11_dflt(1.0);
double HexElement::alpha33_dflt(1.0);
int HexElement::Nnodes(3);

ElementTypeRegistration HexElement::reg(nameplate, HexElement::binaryread);

// Constructor

HexElement::HexElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    alpha11(alpha11_dflt),
    alpha33(alpha33_dflt)
{
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *HexElement::binaryread(FILE *file, TrueFalse &ok) {

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
#ifdef THERMAL
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa11 (float)
  kappa11_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa33 (float)
  kappa33_dflt = c;  // float to double 
#endif // THERMAL
  // create the element
  return create_element((HexElement*) 0);
}

CharString HexElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c33=" + to_charstring(c33)
    + " c44=" + to_charstring(c44)
    + " alpha11=" + to_charstring(alpha11)
    + " alpha33=" + to_charstring(alpha33);
}

bool HexElement::same_type(const Element *el) const {
  const HexElement *other = dynamic_cast<const HexElement*>(el);
  return other && GTElement::same_type(other)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c33 == other->c33
    && c44 == other->c44
    && alpha11 == other->alpha11
    && alpha33 == other->alpha33;
}
