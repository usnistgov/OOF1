// -*- C++ -*-
// $RCSfile: elementfunction.C,v $
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


// ******** See copious comments in elementfunction.h. ********


#ifndef ELEMENTFUNCTION_C
#define ELEMENTFUNCTION_C

#include "elementfunction.h"
#include "stressdrawer.h"

//  The single instance of an elementfunction
template <class EF>
const EF ElementFunction<EF>::instance_;

template <class EF>
ElementFunction<EF>::ElementFunction() {
    static DrawerRegistration *reg = 0;
    if(!reg) {
      // Create a DrawerRegistration, so that this function can be
      // displayed in a graphics window.
      reg = new DrawerRegistration(EF::id(),
		     Color_Drawer<EF>::create,
		     EF::order());
    }
}

// The SGI compiler doesn't like inline functions in the
// ElementFunction template, for some reason, so the following aren't
// inline.

template <class EF>
ElementFunction<EF>::~ElementFunction() {}

template <class EF>
const EF &ElementFunction<EF>::instance() { return instance_; }

template <class EF>
double ElementFunction<EF>::order_() const { return EF::order(); }

template <class EF>
const CharString &ElementFunction<EF>::name() const {
  static CharString name_;
  if(!name_)
    name_ = EF::id();		// make a copy, just once
  return name_;
}

// static function that's called to create a drawer
template <class EF>
Drawer *Color_Drawer<EF>::create(Grid *g, FormDrawer *fd) {
  return new Color_Drawer<EF>(g, fd);
}

#endif
