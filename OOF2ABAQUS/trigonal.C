// -*- C++ -*-
// $RCSfile: trigonal.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 21:24:33 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Trigonal element

#include "createelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "trigonal.h"

// static default data
CharString TrigonalAElement::nameplate("trigonalA");
double TrigonalAElement::c11_dflt(1.0);
double TrigonalAElement::c12_dflt(0.0);
double TrigonalAElement::c13_dflt(0.0);
double TrigonalAElement::c14_dflt(0.0);
double TrigonalAElement::c15_dflt(0.0);
double TrigonalAElement::c33_dflt(1.0);
double TrigonalAElement::c44_dflt(0.5);
double TrigonalAElement::alpha11_dflt(1.0);
double TrigonalAElement::alpha33_dflt(1.0);
int TrigonalAElement::Nnodes(3);

ElementTypeRegistration TrigonalAElement::reg(nameplate,
					     TrigonalAElement::binaryread);

// Constructor

TrigonalAElement::TrigonalAElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c14(c14_dflt),
    c15(c15_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    alpha11(alpha11_dflt),
    alpha33(alpha33_dflt)
{
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *TrigonalAElement::binaryread(FILE *file, TrueFalse &ok) {

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
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c14 (float)
  c14_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c15 (float)
  c15_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c33 (float)
  c33_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c44 (float)
  c44_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha11 (float)
  alpha11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha33 (float)
  alpha33_dflt = c;		// float to double

  // create the element
  return create_element((TrigonalAElement*) 0);
}

CharString TrigonalAElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c14=" + to_charstring(c14)
    + " c15=" + to_charstring(c15)
    + " c33=" + to_charstring(c33)
    + " c44=" + to_charstring(c44)
    + " alpha11=" + to_charstring(alpha11)
    + " alpha33=" + to_charstring(alpha33);
}

bool TrigonalAElement::same_type(const Element *el) const {
  const TrigonalAElement *other = dynamic_cast<const TrigonalAElement*>(other);
  return other && GTElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c14 == other->c14
    && c15 == other->c15
    && c33 == other->c33
    && c44 == other->c44
    && alpha11 == other->alpha11
    && alpha33 == other->alpha33;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// static default data
CharString TrigonalBElement::nameplate("trigonalB");
double TrigonalBElement::c11_dflt(1.0);
double TrigonalBElement::c12_dflt(0.0);
double TrigonalBElement::c13_dflt(0.0);
double TrigonalBElement::c14_dflt(0.0);
double TrigonalBElement::c33_dflt(1.0);
double TrigonalBElement::c44_dflt(0.5);
double TrigonalBElement::alpha11_dflt(1.0);
double TrigonalBElement::alpha33_dflt(1.0);
int TrigonalBElement::Nnodes(3);

ElementTypeRegistration TrigonalBElement::reg(nameplate,
					     TrigonalBElement::binaryread);

// Constructor

TrigonalBElement::TrigonalBElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c14(c14_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    alpha11(alpha11_dflt),
    alpha33(alpha33_dflt)
{
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *TrigonalBElement::binaryread(FILE *file, TrueFalse &ok) {

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
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c14 (float)
  c14_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c33 (float)
  c33_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c44 (float)
  c44_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha11 (float)
  alpha11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha33 (float)
  alpha33_dflt = c;		// float to double
  // create the element
  return create_element((TrigonalBElement*) 0);
}

CharString TrigonalBElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c14=" + to_charstring(c14)
    + " c33=" + to_charstring(c33)
    + " c44=" + to_charstring(c44)
    + " alpha11=" + to_charstring(alpha11)
    + " alpha33=" + to_charstring(alpha33);
}

bool TrigonalBElement::same_type(const Element *el) const {
  const TrigonalBElement *other = dynamic_cast<const TrigonalBElement*>(el);
  return other && GTElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c14 == other->c14
    && c33 == other->c33
    && c44 == other->c44
    && alpha11 == other->alpha11
    && alpha33 == other->alpha33;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

double TrigonalCrack::knockdown1_dflt(0.1);
double TrigonalCrack::knockdown2_dflt(0.2);
double TrigonalCrack::gamma_dflt(0.1);
CharString TrigonalCrack::nameplate("trigonalCrack");
ElementTypeRegistration TrigonalCrack::reg(TrigonalCrack::nameplate,
					      TrigonalCrack::binaryread);

TrigonalCrack::TrigonalCrack(Grid *g)
  : TrigonalAElement(g),
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    gamma(gamma_dflt)
{
}

Element *TrigonalCrack::binaryread(FILE *file, TrueFalse &ok) {
  // kludge: We can't just use TrigonalAElement::binaryread() because
  // that calls create_element(). The concrete element classes weren't
  // meant to be used as base classes!

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables
  // that will be used by the constructor.

  // This section is copied from TrigonalAElement::binaryread().
  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c11 (float)
  c11_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c12 (float)
  c12_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c13 (float)
  c13_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c14 (float)
  c14_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c15 (float)
  c15_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c33 (float)
  c33_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c44 (float)
  c44_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha11 (float)
  alpha11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha33 (float)
  alpha33_dflt = c;		// float to double

  float kd1, kd2, gma;
  if(!readbinary(file, gma)) { ok = TF_FALSE; return 0; } // gamma (float)
  gamma_dflt = gma;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;
  if(!MutableElement::binaryread(file))
    return 0;
  return create_element((TrigonalCrack*) 0);
}

CharString TrigonalCrack::parameters() const {
  return TrigonalAElement::parameters() 
    + " gamma=" + to_charstring(gamma)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

bool TrigonalCrack::same_type(const Element *el) const {
  const TrigonalCrack *other = dynamic_cast<const TrigonalCrack*>(el);
  return el && TrigonalAElement::same_type(el)
    && MutableElement::same_type(el)
    && gamma == other->gamma
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2;
}
