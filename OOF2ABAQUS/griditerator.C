// -*- C++ -*-
// $RCSfile: griditerator.C,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-22 15:15:25 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// iterate over elements

#include "griditerator.h"
#include "grid.h"

class EIterator {
protected:
  Grid *grid;
public:
  EIterator(Grid *g) : grid(g) {}
  virtual ~EIterator() {}
  virtual Element *operator()() = 0;
  virtual void print(ostream &) const = 0;
};

// --------------------------------------------------------- //

class AllElementIterator : public EIterator {
  // This class is more complicated than it needs to be, because in
  // OOF it has to loop over interiorE and boundaryE, but in
  // OOF2ABAQUS boundaryE doesn't exist.
private:
  int currentindex;
  Vec<Element*> *currentlist;
  AllElementIterator(Grid *grd)
    : EIterator(grd),
      currentindex(-1),
      currentlist(&grid->interiorE)
  {}
  virtual Element *operator()();
public:
  virtual void print(ostream &) const;
  friend class ElementIterator;
  friend class EmptyElementIterator;
  friend class NonEmptyElementIterator;
  friend class NonEmptyPhiElementIterator;
  friend class NameIterator;
  friend class SortedElementIterator;
};

Element *AllElementIterator::operator()() {
  if(!grid) return 0;
  currentindex++;
  if(currentindex == currentlist->capacity()) {
    return 0;
  }
  return (*currentlist)[currentindex];
}

void AllElementIterator::print(ostream &os) const {
  if(currentlist == &grid->interiorE)
    os << "interior " << currentindex;
  else
    os << "boundary " << currentindex;
}

// --------------------------------------------------------- //

class EmptyElementIterator : public EIterator {
private:
  AllElementIterator aei;
  EmptyElementIterator(Grid *g) : EIterator(g), aei(g) {}
  virtual Element *operator()();
  virtual void print(ostream &) const;
  friend class ElementIterator;
};

Element *EmptyElementIterator::operator()() {
  Element *el = aei();
  while(el && !el->isempty())
    el = aei();
  return el;
}

void EmptyElementIterator::print(ostream &os) const {
  aei.print(os);
}

class NonEmptyElementIterator : public EIterator {
private:
  AllElementIterator aei;
  NonEmptyElementIterator(Grid *g) : EIterator(g), aei(g) {}
  virtual Element *operator()();
  virtual void print(ostream &) const;
  friend class ElementIterator;
};

Element *NonEmptyElementIterator::operator()() {
  Element *el = aei();
  while(el && el->isempty())
    el = aei();
  return el;
}

void NonEmptyElementIterator::print(ostream &os) const {
  aei.print(os);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class InteriorElementIterator : public EIterator {
private:
  int currentindex;
  InteriorElementIterator(Grid *g) : EIterator(g), currentindex(-1) {}
  virtual Element *operator()();
  virtual void print(ostream &) const;
  friend class ElementIterator;
};

Element *InteriorElementIterator::operator()() {
  if(!grid) return 0;
  currentindex++;
  if(currentindex >= grid->interiorE.capacity()) return 0;
  return grid->interiorE[currentindex];
}

void InteriorElementIterator::print(ostream &os) const {
  os << "interior " << currentindex;
}

class BoundaryElementIterator : public EIterator {
private:
  int currentindex;
  BoundaryElementIterator(Grid *g) : EIterator(g), currentindex(-1) {}
  virtual Element *operator()();
  virtual void print(ostream &) const;
  friend class ElementIterator;
};

Element *BoundaryElementIterator::operator()() {
  return 0;
}

void BoundaryElementIterator::print(ostream &os) const {
  os << "boundary " << currentindex;
}

// --------------------------------------------------------- //

class NameIterator : public EIterator {
private:
  const CharString name;
  AllElementIterator aei;
  NameIterator(const CharString &nm, Grid *g)
    : EIterator(g), aei(g), name(nm)
  {}
  virtual Element *operator()();
  virtual void print(ostream &) const;
  friend class ElementIterator;
};

Element *NameIterator::operator()() {
  Element *el = aei();
  while(el && el->tag() != name)
    el = aei();
  return el;
}

void NameIterator::print(ostream &os) const {
  aei.print(os);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//


// --------------------------------------------------------- //

ElementIterator::ElementIterator(const EItype type, Grid *g) {
  switch(type) {
  case EI_ALL:
    the_iterator = new AllElementIterator(g);
    break;
  case EI_EMPTY:
    the_iterator = new EmptyElementIterator(g);
    break;
  case EI_NONEMPTY:
    the_iterator = new NonEmptyElementIterator(g);
    break;
  case EI_INTERIOR:
    the_iterator = new InteriorElementIterator(g);
    break;
  case EI_BOUNDARY:
    the_iterator = new BoundaryElementIterator(g);
    break;
  default:
    cerr << "bad iterator" << endl;
    exit(1);
  }
}

ElementIterator::ElementIterator(const CharString &name, Grid *g) {
  the_iterator = new NameIterator(name, g);
}

Element *ElementIterator::operator()() {
  return (*the_iterator)();
}

ElementIterator::~ElementIterator() {
  delete the_iterator;
}

ostream &operator<<(ostream &os, const ElementIterator &iter) {
  iter.the_iterator->print(os);
  return os;
}
