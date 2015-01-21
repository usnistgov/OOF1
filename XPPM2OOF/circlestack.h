// -*- C++ -*-
// $RCSfile: circlestack.h,v $
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

// A finite stack which accepts limitless items. Later items override
// earlier ones if the stack is full.

#ifndef CIRCLESTACK_H
#define CIRCLESTACK_H

template <class TYPE> class CircleStack;

// Iterator for looping over all entries in the stack, from bottom to top:
//  for(CircleStackIterator iter(stack); !iter.end(); ++iter)
//      do something with stack[iter]
template <class TYPE>
class CircleStackIterator {
private:
  const CircleStack<TYPE> &stack;
  int which;
  bool done;
public:
  CircleStackIterator(const CircleStack<TYPE>&);
  bool end() const { return done; }
  void operator++();
  friend class CircleStack<TYPE>;
};

template <class TYPE>
class CircleStack {
private:
  TYPE *data;
  int size;
  int ndata;
  int currentpos;		// current value
  int bottom;			// oldest datum
  int top;			// next spot to be filled
  void (*overwrite)(TYPE &);
  void clear(int lo, int hi);	// apply overwrite() to entries i, lo <= i < hi
public:
  CircleStack(int rollover, void (*overwrite)(TYPE&));
  CircleStack(int rollover);
  ~CircleStack();
  TYPE &current() const;
  void push(const TYPE&);	// insert after current value
  bool prev();			// go to previous value
  bool next();			// go to next value
  void first() { currentpos = bottom; }
  void last() { currentpos = (top + size - 1)%size; }
  void clear();			// apply overwrite() to all entries
  int capacity() const { return ndata; }

  void set_rollover(int);	// sets maximum number of entries
  int get_rollover() const { return size; }

  // set current position to given distance from top of stack.
  void set_current_depth(int);
  int get_current_depth() const;

  // sequential access without changing the stack pointers
  const TYPE &operator[](const CircleStackIterator<TYPE>&) const;
  friend class CircleStackIterator<TYPE>;
};

#include "circlestack.C"

#endif
