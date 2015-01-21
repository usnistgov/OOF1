// -*- C++ -*-
// $RCSfile: element.C,v $
// $Revision: 1.3 $
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


#include "element.h"
#include "grid.h"
#include "node.h"
#include "elementgroups.h"
#include "mutableelement.h"
#include "readbinary.h"

// default values for element creation commands
float Element::gray_dflt = 0.5;
Vec<int> Element::nodenumber_dflt;
int Element::index_dflt;
unsigned char Element::inputformatflag(0);
int Element::cloneindex;

// Constructor for base element.

Element::Element(Grid *grid)
  : index(index_dflt),
    intrinsic_gray(gray_dflt),
    planestrain(0)
{
    int order = nodenumber_dflt.capacity();
    corner.resize(order);
    for(int j=0; j<order; j++) {
	corner[j] = grid->getnode(nodenumber_dflt[j]);
    }
}

Element::~Element() {}

Element *Element::binaryread(FILE *file, TrueFalse &ok) {
  ok = TF_TRUE;
  if(!readbinary(file, gray_dflt)) ok = TF_FALSE;
  return 0;
}

void Element::reset_defaults() {
  // reset static default values used when creating elements
  MutableElement::reset_defaults();
}

ostream& operator<<(ostream& os, const Element& elem) {
  elem.print(os);
  return os;
}

bool Element::same_type(const Element *other) const {
  return intrinsic_gray == other->intrinsic_gray;
}

bool operator==(const Element &el1, const Element &el2) {
  return el1.tag() == el2.tag() && el1.same_type(&el2);
}
