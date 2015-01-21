// -*- C++ -*-
// $RCSfile: orthoexpand.C,v $
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



// Orthotropic Expanding element, based on GTElement

#include "orthoexpand.h"
#include "readbinary.h"
#include "createelement.h"

// static default data
CharString OrthoExElement::nameplate("eds_el");
double OrthoExElement::young_dflt(1.0);
double OrthoExElement::poisson_dflt(0.0);
double OrthoExElement::a1_dflt(0.0);
double OrthoExElement::a2_dflt(0.0);
double OrthoExElement::a3_dflt(0.0);
int OrthoExElement::Nnodes(3);

ElementTypeRegistration OrthoExElement::reg(nameplate,
					    OrthoExElement::binaryread);

// Constructor

OrthoExElement::OrthoExElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    a1(a1_dflt),
    a2(a2_dflt),
    a3(a3_dflt)
{
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *OrthoExElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, aa1, aa2, aa3;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, aa1)) { ok = TF_FALSE; return 0; } // alpha (float)
  a1_dflt = aa1;
  if(!readbinary(file, aa2)) { ok = TF_FALSE; return 0; } // alpha (float)
  a2_dflt = aa2;
  if(!readbinary(file, aa3)) { ok = TF_FALSE; return 0; } // alpha (float)
  a3_dflt = aa3;
  // create the element
  return create_element((OrthoExElement*) 0);
}

CharString OrthoExElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" +  to_charstring(young)
    + " a1=" +  to_charstring(a1)
    + " a2=" +  to_charstring(a2)
    + " a3=" +  to_charstring(a3);
}

bool OrthoExElement::same_type(const Element *el) const {
  const OrthoExElement *other = dynamic_cast<const OrthoExElement*>(el);
  return other && GTElement::same_type(el)
    && poisson == other->poisson
    && young == other->young
    && a1 == other->a1
    && a2 == other->a2
    && a3 == other->a3;
}
