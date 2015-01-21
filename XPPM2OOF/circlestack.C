// -*- C++ -*-
// $RCSfile: circlestack.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-30 19:23:23 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef CIRCLESTACK_C
#define CIRCLESTACK_C

#include "circlestack.h"
#include <iostream.h>
#include "stdlib.h"

template <class TYPE>
CircleStack<TYPE>::CircleStack(int sz)
  : size(sz),
    data(new TYPE[sz]),
    ndata(0),
    currentpos(0),
    bottom(0),
    top(0),
    overwrite(0)
{}

template <class TYPE>
CircleStack<TYPE>::CircleStack(int sz, void (*ovrwrt)(TYPE&))
  : size(sz),
    data(new TYPE[sz]),
    ndata(0),
    currentpos(0),
    bottom(0),
    top(0),
    overwrite(ovrwrt)
{}

template <class TYPE>
CircleStack<TYPE>::~CircleStack() {
  delete [] data;
}

template <class TYPE>
TYPE &CircleStack<TYPE>::current() const {
  if(ndata == 0) {
    cerr << "Error in CircleStack!" << endl;
    exit(1);
  }
  return data[currentpos];
}

template <class TYPE>
void CircleStack<TYPE>::push(const TYPE &x) {
  if(ndata == 0) {		// must initialize
    data[0] = x;
    top = 1;
    currentpos = 0;
    ndata = 1;
  }
  else if((currentpos+1)%size != top) { // not at top of stack!
    currentpos = (currentpos + 1)%size;
    clear(currentpos, top);	// delete from here to top
    ndata++;			// add data at new top position
    top = (currentpos + 1)%size;
    data[currentpos] = x;
  }
  else if(top == bottom) {	// at top, but have to overwrite oldest datum
    if(overwrite)
      (*overwrite)(data[top]);
    data[top] = x;
    currentpos = top;
    top = (top+1)%size;
    bottom = top;
  }
  else {			// at top, space available
    data[top] = x;
    currentpos = top;
    top = (top+1)%size;
    ndata++;
  }
}

template <class TYPE>
bool CircleStack<TYPE>::prev() {
  if(currentpos == bottom || ndata == 0) return 0;
  currentpos = (currentpos + size - 1)%size;
  return 1;
}

template <class TYPE>
bool CircleStack<TYPE>::next() {
  if((currentpos+1)%size == top || ndata == 0) return 0;
  currentpos = (currentpos + 1)%size;
  return 1;
}

template <class TYPE>
void CircleStack<TYPE>::set_current_depth(int depth) {
  if(depth >= ndata)
    currentpos = bottom; // should be an error?
  else {
    currentpos = (top + size - 1 - depth) % size;
  }
}

template <class TYPE>
int CircleStack<TYPE>::get_current_depth() const {
  if(currentpos < top)
    return top - currentpos - 1;
  else
    return size - currentpos + top - 1;
}


// Apply overwrite() to entries i, lo <= i < hi.
// Routines which call this should reset bottom and top themselves.

template <class TYPE>
void CircleStack<TYPE>::clear(int lo, int hi) {
  int nclear = hi - lo;		// how many items to remove
  if(nclear <= 0) nclear += size; // wrap around
  if(ndata == 0) return;
  if(overwrite)
    for(int i=0; i<nclear; i++)
      (*overwrite)(data[(lo + i)%size]);
  ndata -= nclear;
}

template <class TYPE>
void CircleStack<TYPE>::clear() {
  clear(bottom, top);
  top = 0;
  bottom = 0;
  currentpos = 0;
}

// Reallocate the stack and change its physical size.  Copy data into
// the new stack so that bottom=0.

template <class TYPE>
void CircleStack<TYPE>::set_rollover(int newsize) {
  TYPE *newdata = new TYPE[newsize];
  int depth = top - currentpos; // try to maintain this
  if(depth <= 0) depth += size;	// wrap around

  // bottom and top delimit range of data to be copied.  Move bottom
  // up if new size is too small.
  if(ndata > newsize) {
    int oldbottom = bottom;
    bottom = (oldbottom + ndata - newsize) % size;
    clear(oldbottom, bottom);
  }

  for(int i=0; i<ndata; i++)
    newdata[i] = data[(bottom + i) % size];

  bottom = 0;
  top = ndata % newsize;
  if(depth > ndata)
    currentpos = 0;
  else
    currentpos = ndata - depth;

  delete [] data;
  data = newdata;
  size = newsize;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

template <class TYPE>
CircleStackIterator<TYPE>::CircleStackIterator(const CircleStack<TYPE> &stck)
  : stack(stck), done(0)
{
  which = stack.bottom;
  if(stack.ndata == 0) done = 1;
}

template <class TYPE>
void CircleStackIterator<TYPE>::operator++() {
  which = (which + 1) % stack.size;
  if(which == stack.top || stack.size == 1) done = 1;
}

template <class TYPE>
const TYPE &CircleStack<TYPE>::operator[](const CircleStackIterator<TYPE> &iter)
  const
{
  return data[iter.which];
}

#endif
