// -*- C++ -*-
// $RCSfile: elementfunction.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-12-05 21:30:32 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef ELEMENTFUNCTION_H
#define ELEMENTFUNCTION_H

// The ElementFunction classes define things that can be computed on
// an Element. They are derived from this templated base class, which
// takes care of things like putting the functions in the menus, and
// making them accessible to the rest of the program.  The derived
// classes must look like this:

//  class StressXX : public ElementFunction<StressXX> {
//  public:
//    StressXX() {}
//    static CharString id() { return CharString("StressXX"); }
//    static double order() { return 1.0; }
//    virtual double operator()(Element &el) const {
//      return el.stresstensor()(0,0);
//    }
//  };
//  template class ElementFunction<StressXX>;

// Notes:

// Although the ElementFunctions don't contain any non-static data, a
// single instance of each function must be created, so that the
// constructor can put each function in the menus, etc.  In order to
// do this in the base class, the type of the derived class is passed
// to the base class (ElementFunction<>) as a template argument.  That
// means that the ElementFunction classes are all different types, so
// the template ElementFunction<> is in turn derived from
// ElementFunctionBase.  ElementFunctionBase maintains the list of all
// the ElementFunctions<>.

// Because instances of these classes are created and used at startup
// to construct the menus, they can't rely on static data for
// initialization (there is no way to make sure that the
// initialization data is created before the
// ElementFunction). Therefore, instead of having a "static const
// CharString name" data member, the classes have a function (id)
// which returns the name. This function can't return a CharString&,
// because to do that it would either have to return a reference to a
// temporary, or the CharString would have to be a static member, and
// we can't guarantee that *that* member would be constructed before
// it's used. So, the derived class must return a plain old
// CharString, which is inefficient. To relieve the inefficiency,
// ElementFunction::name makes and stores a copy when needed, and returns a
// reference to it.

// The functions id() and order() must be static, because they are
// used in the constructor of the ElementFunction<> base class.

// The value returned by order() determines where this ElementFunction
// will appear in menus. Lower orders come first. 

// Not all compilers catch the fact that these templates are actually
// used, since they're not explicitly used anywhere: the only instance
// of an object of the ElementFunction<StressXX> class is the static
// instance declared within ElementFunction<StressXX>.  The line
// "template class ElementFunction<StressXX>" forces instantiation of
// the template.

// The SGI compiler can't seem to generate a default null constructor
// and issues a warning if one isn't provided.  Hence the line
// "StressXX() {}" in the example above.

#include "colordrawer.h"
#include "elementfunctionbase.h"
class CharString;

template <class EF>
class ElementFunction : public ElementFunctionBase {
public:
  virtual ~ElementFunction();
  virtual const CharString &name() const;
  virtual double operator()(Element&) const = 0;
  static const EF &instance();
  virtual double order_() const; // order in menus
protected:
  ElementFunction();
private:
  static const EF instance_;	// the single instance
};

#include "elementfunction.C"

#endif // ELEMENTFUNCTION_H
