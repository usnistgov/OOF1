// -*- C++ -*-
// $RCSfile: rastack.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-24 14:35:17 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef RASTACK_H
#define RASTACK_H

// Random Access Stack -- can get at any item in the stack, but the
// top of the stack is lost if something is pushed or popped when the
// current item isn't the top item. 

#include "vec.h"

template <class TYPE> class RAStack {
private:
  Vec<TYPE*> data;
  int currentitem;
  void decapitate();
public:
  RAStack() : currentitem(0) {}
  ~RAStack();
  void clear();
  void push(const TYPE &);
  TYPE *dup();			// copy current item on stack, chop top off
  TYPE pop();			// pops current item and chops top off stack
  TYPE* current() const { return data[currentitem]; }
  TYPE* prev();
  TYPE* next();

  int attop() const {
    return data.capacity() == 0 || currentitem == (data.capacity() - 1);
  }
  int atbottom() const {
    return data.capacity() == 0 || currentitem == 0;
  }

  // allow the stack to be used as a vector, sort of
  int size() const { return data.capacity(); }
  const TYPE *operator[](int i) const { return data[i]; }
  int current_index() const { return currentitem; }
  void go_to(int where) { currentitem = where; }
};

#include "rastack.C"

#endif
