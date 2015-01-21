// -*- C++ -*-
// $RCSfile: emptyelement.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-22 14:11:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Empty Element
 

#include "charstring.h"
#include "createelement.h"
#include "element.h"
#include "emptyelement.h"
#include "grid.h"
#include "node.h"
#include "readbinary.h"
#include <iostream.h>

// static and default data
int EmptyElement::Nnodes(3);
CharString EmptyElement::nameplate("empty");
ElementTypeRegistration EmptyElement::reg(nameplate, EmptyElement::binaryread);

EmptyElement::EmptyElement(Grid *g) : TriElement(g) {
}

Element *EmptyElement::binaryread(FILE *file, TrueFalse &ok) {
  TriElement::binaryread(file, ok);
  if(!ok) return 0;
  return create_element((EmptyElement*)0);
}
