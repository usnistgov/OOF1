// -*- C++ -*-
// $RCSfile: genelement.C,v $
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


// General Triangular element

#include "genelement.h"
#include "grid.h"
#include "inputflags.h"
#include "readbinary.h"
#include <iomanip.h>

// default values of arguments
EulerAngle GTElement::orientation_dflt;
TrueFalse GTElement::planestrain_dflt(0);

// ----------------------------------------------------------//

GTElement::GTElement(Grid *g)
  : TriElement(g),
    orientation(orientation_dflt)
{
  // for oof2abaqus, planestrain has to be in the Element base class,
  // since the abaqus element type depends on it.
  Element::planestrain = planestrain_dflt;
}

// ----------------------------------------------------------//

// binaryread is called by the derived class's binaryread. That
// function should *first* call this one.

Element *GTElement::binaryread(FILE *file, TrueFalse &ok) {
  TriElement::binaryread(file, ok);
  if(!ok) return 0;

  float a, b, c;
  if(!readbinary(file, a)) {			// euler a  (float)
    ok = TF_FALSE;
    return 0;
  }
  if(!readbinary(file, b)) {			// euler b  (float)
    ok = TF_FALSE;
    return 0;
  }
  if(!readbinary(file, c)) {			// euler c  (float)
    ok = TF_FALSE;
    return 0;
  }
  orientation_dflt = EulerAngle(a, b, c);


  if(!readbinary(file, inputformatflag)) {         // flag (char)
    ok = TF_FALSE;
    return 0;
  }
  if((inputformatflag & STIFFNESS) && (inputformatflag&DUPLICATE))
    garcon()->msout << ms_error << "stiffness and duplicate?"
		    << endl << ms_normal;
  planestrain_dflt = inputformatflag & PLANESTRAIN;


  if(inputformatflag & STIFFNESS) {
    if(!K_dflt.binaryread(file)) {   // stiffness matrix
      ok = TF_FALSE;
      return 0;
    }

   for(int i=0; i<6; i++)        // thermal forces
      if(!readbinary(file, thermal_dflt(i))) {
	ok = TF_FALSE;
	return 0;
      }
  }
  else if(inputformatflag & DUPLICATE) {
    if(!readbinary(file, cloneindex)) {
      ok = TF_FALSE;
      return 0;
    }

  }
  return 0;
}


//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

CharString GTElement::parameters() const {
  return TriElement::parameters()
    + " orientation=" + to_charstring(orientation)
    + " planestrain=" + to_charstring(planestrain);
}

bool GTElement::same_type(const Element *el) const {
  const GTElement *other = dynamic_cast<const GTElement*>(el);
  return other && TriElement::same_type(el)
    && orientation == other->orientation
    && planestrain == other->planestrain;
}
