// -*- C++ -*-
// $RCSfile: trielement.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-12-26 19:10:07 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Triangular linear elements

#include "grid.h"
#include "main.h"
#include "node.h"
#include "readbinary.h"
#include "symmmatrix.h"
#include "trielement.h"

SymmMatrix TriElement::K_dflt(6);
MV_Vector_double TriElement::thermal_dflt(6, 0.0);

TriElement::TriElement(Grid *g) : Element(g) {}

Element *TriElement::binaryread(FILE *file, TrueFalse &ok) {
  Element::binaryread(file, ok);
  if(ok) {
    Element::nodenumber_dflt.resize(3);
    for(int j=0; j<3; j++) {
      if(!readbinary(file, Element::nodenumber_dflt[j])) {
	ok = TF_FALSE;
	break;
      }
    }
  }
  return 0;
}

void TriElement::make_edgenodes(Grid *g) {
  edgenode.resize(3);
  for(int i=0; i<corner.capacity(); i++) {
    edgenode[i] = g->get_edgenode(corner[(i+1)%3], corner[(i+2)%3]);
  }
}

bool TriElement::same_type(const Element *other) const {
  return Element::same_type(other);
}

void TriElement::print(ostream &os) const {
  os << index + 1;
  if(!sixnodes) {
    for(int i=0; i<corner.capacity(); i++) {
      os << ", " << corner[i]->index + 1;
    }
  }
  else {
    for(int i=0; i<corner.capacity(); i++) {
      os << ", " << corner[i]->index + 1;
    }
    for(int i=0; i<edgenode.capacity(); i++) {
      os << ", " << edgenode[(i+2)%3]->index + 1;
    }
  }
}
