// -*- C++ -*-
// $RCSfile: vec.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:59 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// a simple vector template class

// doesn't work unless TYPE has a null constructor and an operator=().
// doesn't work if TYPE requires a destructor

template <class TYPE> class Vec;

#ifndef VEC_H
#define VEC_H

#include <iostream.h>

template <class TYPE>
ostream& operator<<(ostream &s, const Vec<TYPE> &v);

class BlockSize {
public:
  int bs;
  BlockSize(int s = 1) : bs(s==0? 1 : s) {}
};

template <class TYPE> class Vec {
private:
  TYPE *data;
  int allocatedsize;
  int logicalsize;
  BlockSize blocksize;	// allocation chunk
  void copydata(TYPE*);
public:
  // create null vector
  Vec();
  // create vector of length n, blocksize 1
  Vec(int n);
  // create vector of length n, blocksize b
  Vec(int n, BlockSize b);
  // create vector of length n, initialize to x, blocksize 1
  Vec(int n, TYPE x);
  // create vector of length n, initialize to x, blocksize b
  Vec(int n, TYPE x, BlockSize b);
  Vec(const Vec<TYPE> &);	// copy constructor
  ~Vec();
  const Vec<TYPE>& operator=(const Vec<TYPE> &);    // assignment operator
  TYPE& operator[](const int) const; // access an element 
  void resize(const int);	// change logical size, *preserving* data
  void resize(const int, const TYPE &x); // resize and initialize new data
  int grow(const int = 1); // increase size by n
  int grow(const int, const TYPE&); // increase size by n and initialize
  int remove(const TYPE&);   // remove first instance and consolidate
  int remove(const TYPE&, int (*)(const TYPE&, const TYPE&));
  int remove_conditional(int (*f)(const TYPE&)); //remove all for which f(T)==1
  int remove_conditional(int (*f)(TYPE&)); //remove all for which f(T)==1
  int remove_conditional(const Vec<int> &flag); // remove all for which flag==1
  void remove_all(const TYPE &); // remove all instances and consolidate
  void trim(const TYPE&);   // remove from end and consolidate
  void replace(const TYPE&, const TYPE&); // replace 1 by 2
  int capacity() const { return logicalsize; } // report logical size

  // these functions change the allocated size and DO NOT preserve data
  void smash(); // set size to zero and free space
  void setphysicalsize(int); // set allocated size without changing logical size
  
  operator TYPE*() {return data;}	// convert to normal array
  

#if defined(sun) && !defined(__GNUG__)
  friend ostream& operator<<(ostream &, const Vec<TYPE> &);
#else
  friend ostream& operator<< <>(ostream &, const Vec<TYPE> &);
#endif
  friend class Debug;
};

#include "vec.C"

#endif
