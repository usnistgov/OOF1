// -*- C++ -*-
// $RCSfile: tetragonal.C,v $
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

// Tetragonal element

#include "createelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "tetragonal.h"

// static default data
CharString TetragonalElement::nameplate("tetragonal");
double TetragonalElement::c11_dflt(1.0);
double TetragonalElement::c12_dflt(0.0);
double TetragonalElement::c13_dflt(0.0);
double TetragonalElement::c16_dflt(0.0);
double TetragonalElement::c33_dflt(1.0);
double TetragonalElement::c44_dflt(1.0);
double TetragonalElement::c66_dflt(1.0);
double TetragonalElement::alpha11_dflt(1.0);
double TetragonalElement::alpha33_dflt(1.0);
int TetragonalElement::Nnodes(3);

ElementTypeRegistration TetragonalElement::reg(nameplate,
					       TetragonalElement::binaryread);

// Constructor

TetragonalElement::TetragonalElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c16(c16_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    c66(c66_dflt),
    alpha11(alpha11_dflt),
    alpha33(alpha33_dflt)
{
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *TetragonalElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables
  // that will be used by the constructor.

  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c11 (float)
  c11_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c12 (float)
  c12_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c13 (float)
  c13_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c16 (float)
  c16_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c33 (float)
  c33_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c44 (float)
  c44_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c66 (float)
  c66_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha11 (float)
  alpha11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha33 (float)
  alpha33_dflt = c;		// float to double

  // create the element
  return create_element((TetragonalElement*) 0);
}

CharString TetragonalElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c16=" + to_charstring(c16)
    + " c33=" + to_charstring(c33)
    + " c44=" + to_charstring(c44)
    + " c66=" + to_charstring(c66)
    + " alpha11=" + to_charstring(alpha11)
    + " alpha33=" + to_charstring(alpha33);
}

bool TetragonalElement::same_type(const Element *el) const {
  const TetragonalElement *other = dynamic_cast<const TetragonalElement*>(el);
  return other && GTElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c16 == other->c16
    && c33 == other->c33
    && c44 == other->c44
    && c66 == other->c66
    && alpha11 == other->alpha11
    && alpha33 == other->alpha33;
}
