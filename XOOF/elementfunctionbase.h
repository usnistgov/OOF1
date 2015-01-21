// -*- C++ -*-
// $RCSfile: elementfunctionbase.h,v $
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


#ifndef ELEMENTFUNCTIONBASE_H
#define ELEMENTFUNCTIONBASE_H

#include "vec.h"
class CharString;
class Element;

class ElementFunctionBase {
public:
  virtual const CharString &name() const = 0;
  virtual double operator()(Element&) const = 0;
  virtual double order_() const = 0; // for sorting in menus
  ElementFunctionBase();
  static Vec<ElementFunctionBase*> &allElementFunctions();
  static void sort();
};


#endif
