// -*- C++ -*-
// $RCSfile: elementfunctionbase.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:55 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// See copious comments in elementfunction.h.


#include "elementfunctionbase.h"
#include "stdlib.h"

// The list of all ElementFunctions is returned by a static function,
// guaranteeing that it is created before it is used.  This is
// important because the ElementFunctions themselves are static
// objects, so we don't have control over when they're created.

Vec<ElementFunctionBase*> &ElementFunctionBase::allElementFunctions() {	//static
  static Vec<ElementFunctionBase*> v;
  return v;
}

ElementFunctionBase::ElementFunctionBase() {
  allElementFunctions().grow(1, this);
}

static int ordering(const void *ef1, const void *ef2) {
  double o1 = (*reinterpret_cast<ElementFunctionBase*const*>(ef1))->order_();
  double o2 = (*reinterpret_cast<ElementFunctionBase*const*>(ef2))->order_();
  if(o1 < o2) return -1;
  if(o1 > o2) return 1;
  return 0;
}

void ElementFunctionBase::sort() { // static
  qsort(&(allElementFunctions()[0]), allElementFunctions().capacity(),
	sizeof(ElementFunctionBase*), ordering);
}
