// -*- C++ -*-
// $RCSfile: rastack.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-30 19:23:29 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef RASTACK_C
#define RASTACK_C

#include "rastack.h"

template <class TYPE>
RAStack<TYPE>::~RAStack() {
  for(int i=0; i<data.capacity(); i++)
    delete data[i];
}

template <class TYPE>
void RAStack<TYPE>::decapitate() { // delete everything past the current item
  if(data.capacity() > 0) {
    for(int i=currentitem+1; i<data.capacity(); i++)
      delete data[i];
    data.resize(currentitem+1);
  }
}

template <class TYPE>
void RAStack<TYPE>::clear() {
  for(int i=0; i<data.capacity(); i++)
    delete data[i];
  currentitem = 0;
  data.resize(0);
}

template <class TYPE>
void RAStack<TYPE>::push(const TYPE &item) {
  decapitate();
  currentitem = data.grow(1, new TYPE(item));
}

template <class TYPE>
TYPE *RAStack<TYPE>::dup() {
  decapitate();
  currentitem = data.grow(1, new TYPE(*data[currentitem]));
  return data[currentitem];
}

template <class TYPE>
TYPE RAStack<TYPE>::pop() {
  TYPE popped = *data[currentitem];
  currentitem--;
  decapitate();
  return popped;
}

template <class TYPE>
TYPE *RAStack<TYPE>::prev() {
  if(currentitem == 0)
    return 0;
  return data[--currentitem];
}

template <class TYPE>
TYPE *RAStack<TYPE>::next() {
  if(currentitem == data.capacity()-1)
    return 0;
  return data[++currentitem];
}

#endif
