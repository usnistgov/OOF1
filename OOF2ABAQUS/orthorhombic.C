// -*- C++ -*-
// $RCSfile: orthorhombic.C,v $
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



// Orthorhombic element, based on GTElement

#include "orthorhombic.h"
#include "readbinary.h"
#include "createelement.h"

// static default data
CharString OrthoElement::nameplate("orthorhombic");
double OrthoElement::c11_dflt(1.0);
double OrthoElement::c12_dflt(0.0);
double OrthoElement::c13_dflt(0.0);
double OrthoElement::c22_dflt(1.0);
double OrthoElement::c23_dflt(0.0);
double OrthoElement::c33_dflt(1.0);
double OrthoElement::c44_dflt(0.5);
double OrthoElement::c55_dflt(0.5);
double OrthoElement::c66_dflt(0.5);
double OrthoElement::alpha11_dflt(1.0);
double OrthoElement::alpha22_dflt(1.0);
double OrthoElement::alpha33_dflt(1.0);
int OrthoElement::Nnodes(3);

ElementTypeRegistration OrthoElement::reg(nameplate,
					  OrthoElement::binaryread);

// Constructor

OrthoElement::OrthoElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c22(c22_dflt),
    c23(c23_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    c55(c55_dflt),
    c66(c66_dflt),
    alpha11(alpha11_dflt),
    alpha22(alpha22_dflt),
    alpha33(alpha33_dflt)
{
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *OrthoElement::binaryread(FILE *file, TrueFalse &ok) {

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

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c22 (float)
  c22_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c23 (float)
  c23_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c33 (float)
  c33_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c44 (float)
  c44_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c55 (float)
  c55_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c66 (float)
  c66_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha11 (float)
  alpha11_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha22 (float)
  alpha22_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha33 (float)
  alpha33_dflt = c;  // float to double 

  // create the element
  return create_element((OrthoElement*) 0);
}

CharString OrthoElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c22=" + to_charstring(c22)
    + " c23=" + to_charstring(c23)
    + " c33=" + to_charstring(c33)
    + " c44=" + to_charstring(c44)
    + " c55=" + to_charstring(c55)
    + " c66=" + to_charstring(c66)
    + " alpha11=" + to_charstring(alpha11)
    + " alpha22=" + to_charstring(alpha22)
    + " alpha33=" + to_charstring(alpha33);
}

bool OrthoElement::same_type(const Element *el) const {
  const OrthoElement *other = dynamic_cast<const OrthoElement*>(el);
  return other && GTElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c22 == other->c22
    && c23 == other->c23
    && c33 == other->c33
    && c44 == other->c44
    && c55 == other->c55
    && c66 == other->c66
    && alpha11 == other->alpha11
    && alpha22 == other->alpha33
    && alpha33 == other->alpha22;
}
