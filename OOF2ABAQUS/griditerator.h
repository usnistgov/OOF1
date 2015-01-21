// -*- C++ -*-
// $RCSfile: griditerator.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:10 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef GRIDITERATOR_H
#define GRIDITERATOR_H

enum EItype {
  EI_ALL,
  EI_SELECTED,
  EI_NONEMPTY,
  EI_EMPTY,
#ifdef THERMAL
  EI_NONEMPTY_PHI,
#endif
  EI_INTERIOR,
  EI_BOUNDARY,
  EI_MUTATED,
  EI_SORTED
};

class EIterator;

#include "charstring.h"
#include "element.h"
#include "iterator.h"
#include <iostream.h>

class ElementIterator : public Iterator<Element> {
private:
  EIterator *the_iterator;
public:
  ElementIterator(const EItype, Grid*);
  ElementIterator(const CharString&, Grid*);
  virtual ~ElementIterator();
  virtual Element *operator()();
  friend ostream &operator<<(ostream&, const ElementIterator&);
};


#endif
