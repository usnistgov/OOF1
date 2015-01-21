// -*- C++ -*-
// $RCSfile: griditerator.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2004-10-19 02:24:35 $

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
  const Grid *grid;
public:
  EIterator(const Grid *g) : grid(g) {}
  virtual ~EIterator() {}
  virtual Element *operator()() = 0;
  virtual void print(ostream &) const = 0;
};

// --------------------------------------------------------- //

class AllElementIterator : public EIterator {
private:
  int currentindex;
  const Vec<Element*> *currentlist;
  AllElementIterator(const Grid *grd)
    : EIterator(grd),
      currentindex(-1),
      currentlist(&grid->interiorE)
  {}
  virtual Element *operator()();
public:
  virtual void print(ostream &) const;
  friend class ElementIterator;
  friend class SelectedElementIterator;
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
    if(currentlist == &grid->boundaryE) return 0;
    currentlist = &grid->boundaryE;
    if(currentlist->capacity() == 0) return 0;
    currentindex = 0;
    return (*currentlist)[0];
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

class SelectedElementIterator : public EIterator {
private:
  AllElementIterator aei;
  SelectedElementIterator(const Grid *g) : EIterator(g), aei(g) {}
  virtual Element *operator()();
  virtual void print(ostream &) const;
  friend class ElementIterator;
};

Element *SelectedElementIterator::operator()() {
  Element *el = aei();
  while(el && !el->isselected())
    el = aei();
  return el;
}

void SelectedElementIterator::print(ostream &os) const {
  aei.print(os);
}

// --------------------------------------------------------- //

class EmptyElementIterator : public EIterator {
private:
  AllElementIterator aei;
  EmptyElementIterator(const Grid *g) : EIterator(g), aei(g) {}
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
  NonEmptyElementIterator(const Grid *g) : EIterator(g), aei(g) {}
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

#ifdef THERMAL
class NonEmptyPhiElementIterator : public EIterator {
private:
  AllElementIterator aei;
  NonEmptyPhiElementIterator(const Grid *g) : EIterator(g), aei(g) {}
  virtual Element *operator()();
  virtual void print(ostream &) const;
  friend class ElementIterator;
};

Element *NonEmptyPhiElementIterator::operator()() {
  Element *el = aei();
  while(el && el->isempty_phi())
    el = aei();
  return el;
}

void NonEmptyPhiElementIterator::print(ostream &os) const {
  aei.print(os);
}
#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class InteriorElementIterator : public EIterator {
private:
  int currentindex;
  InteriorElementIterator(const Grid *g) : EIterator(g), currentindex(-1) {}
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
  BoundaryElementIterator(const Grid *g) : EIterator(g), currentindex(-1) {}
  virtual Element *operator()();
  virtual void print(ostream &) const;
  friend class ElementIterator;
};

Element *BoundaryElementIterator::operator()() {
  if(!grid) return 0;
  currentindex++;
  if(currentindex >= grid->boundaryE.capacity()) return 0;
  return grid->boundaryE[currentindex];
}

void BoundaryElementIterator::print(ostream &os) const {
  os << "boundary " << currentindex;
}

// --------------------------------------------------------- //

class NameIterator : public EIterator {
private:
  const CharString name;
  AllElementIterator aei;
  NameIterator(const CharString &nm, const Grid *g)
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

class SortedElementIterator : public EIterator {
private:
  int currentindex;
  Vec<Element*> elist;
  SortedElementIterator(const Grid *g)
    : EIterator(g),
      currentindex(-1),
      elist(g->interiorE.capacity() + g->boundaryE.capacity())
  {
    if(!grid) return;
    int i;
    int ni = g->interiorE.capacity();
    for(i=0; i<g->interiorE.capacity(); i++)
      elist[i] = g->interiorE[i];
    for(i=0; i<g->boundaryE.capacity(); i++)
      elist[i+ni] = g->boundaryE[i];
    qsort(&elist[0], elist.capacity(), sizeof(Element*), Element::compare);
  }
  virtual Element *operator()() {
    if(!grid) return 0;
    currentindex++;
    if(currentindex >= elist.capacity()) return 0;
    return elist[currentindex];
  }
  virtual void print(ostream &os) const {
    os << "sorted " << currentindex;
  }
  friend class ElementIterator;
};

// --------------------------------------------------------- //

ElementIterator::ElementIterator(const EItype type, const Grid *g) {
  switch(type) {
  case EI_ALL:
    the_iterator = new AllElementIterator(g);
    break;
  case EI_SELECTED:
    the_iterator = new SelectedElementIterator(g);
    break;
  case EI_EMPTY:
    the_iterator = new EmptyElementIterator(g);
    break;
  case EI_NONEMPTY:
    the_iterator = new NonEmptyElementIterator(g);
    break;
#ifdef THERMAL
  case EI_NONEMPTY_PHI:
    the_iterator = new NonEmptyPhiElementIterator(g);
    break;
#endif
  case EI_INTERIOR:
    the_iterator = new InteriorElementIterator(g);
    break;
  case EI_BOUNDARY:
    the_iterator = new BoundaryElementIterator(g);
    break;
  case EI_SORTED:
    the_iterator = new SortedElementIterator(g);
    break;
  }
}

ElementIterator::ElementIterator(const CharString &name, const Grid *g) {
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
