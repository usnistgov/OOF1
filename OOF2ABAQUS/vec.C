// -*- C++ -*-
// $RCSfile: vec.C,v $
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


// a simple vector template class

// doesn't work unless TYPE has a null constructor and an operator=().
// doesn't work if TYPE requires a destructor

#ifndef VEC_C
#define VEC_C

#include <stdlib.h>
#include <iostream.h>
#include "fatalerror.h"
#include "vec.h"

// construct a null vector
template <class TYPE>
Vec<TYPE>::Vec() : data(0), logicalsize(0), allocatedsize(0), blocksize(1) {}

// construct a vector of length n

template <class TYPE>
Vec<TYPE>::Vec(int n)
  : data(0), logicalsize(n), allocatedsize(n), blocksize(1)
{
  if(n > 0) {
    data = new TYPE[n];
    if(!data) {
      cerr << "Vec: failed to allocate vec of " << n << " objects of size "
	   << sizeof(TYPE) << "!" << endl;
      fatalerror();			// should throw an exception
    }
  }
}

// construct a vector of length n, blocksize b

template <class TYPE>
Vec<TYPE>::Vec(int n, BlockSize b)
  : data(0), logicalsize(n), allocatedsize(n), blocksize(b)
{  
  if(n > 0) {
    data = new TYPE[n];
    if(!data) {
      cerr << "Vec: failed to allocate vec of " << n << " objects of size "
	   << sizeof(TYPE) << "!" << endl;
      fatalerror();			// should throw an exception
    }
  }
}

// construct initialized vector of length n, all data = x, blocksize 1

template <class TYPE>
Vec<TYPE>::Vec(int n, TYPE x)
  : data(0), logicalsize(n), allocatedsize(n), blocksize(1)
{
  if(n > 0) {
    data = new TYPE[n];
    if(!data) {
      cerr << "Vec: failed to allocate vec of " << n << " objects of size "
	   << sizeof(TYPE) << "!" << endl;
      fatalerror();			// should throw an exception
    }
    for(int i=0; i<logicalsize; i++)
      data[i] = x;
  }
}

// construct initialized vector of length n, all data = x, blocksize b

template <class TYPE>
Vec<TYPE>::Vec(int n, TYPE x, BlockSize b)
  : data(0), logicalsize(n), allocatedsize(n), blocksize(b)
{
  if(n > 0) {
    data = new TYPE[n];
    if(!data) {
      cerr << "Vec: failed to allocate vec of " << n << " objects of size "
	   << sizeof(TYPE) << "!" << endl;
      fatalerror();			// should throw an exception
    }
      for(int i=0; i<logicalsize; i++)
      data[i] = x;
  }
}

// destruct a vector
template <class TYPE>
Vec<TYPE>::~Vec() {
  delete [] data;
}

// copy constructor
template <class TYPE>
Vec<TYPE>::Vec(const Vec<TYPE> &v)
: data(new TYPE [v.logicalsize]),
  logicalsize(v.logicalsize), allocatedsize(v.logicalsize),
  blocksize(v.blocksize)
{
  if(v.logicalsize > 0 && !data) {
    cerr << "Vec: failed to copy vec of " << v.logicalsize
	 << " objects of size " << sizeof(TYPE) << "!" << endl;
    fatalerror();			// should throw an exception
  }
  copydata(v.data);
}

// assignment operator
template <class TYPE>
const Vec<TYPE>& Vec<TYPE>::operator=(const Vec<TYPE> &v) {
  if(&v != this) {
    delete [] data;
    logicalsize = allocatedsize = v.logicalsize;
    blocksize = v.blocksize;
    data = new TYPE [logicalsize];
    if(logicalsize > 0 && !data) {
      cerr << "Vec: failed to allocate vec of " << logicalsize
	   << " objects of size " << sizeof(TYPE) << "!" << endl;
      fatalerror();			// should throw an exception
    }
    copydata(v.data);
  }
  return *this;
}

// access
template <class TYPE>
TYPE& Vec<TYPE>::operator[](const int n) const {
    return data[n];
}

/*  inlined
// get size
template <class TYPE>
int Vec<TYPE>::capacity() {
    return logicalsize;
}
*/

// change size
#include <math.h>   // for ceil

template <class TYPE>
void Vec<TYPE>::resize(const int newsize) {
    if(newsize == logicalsize) return;	// no change
    
    if(newsize < logicalsize) {		// shrinking
	logicalsize = newsize;
	// here is where extra elements should be destructed. See p 173
	// R. Murray's "C++ Strategies and Tactics". The right thing to do
	// is to copy the first newsize elements of data and delete all of it.
	return;
    }
    
    if(newsize <= allocatedsize) {	// growing into preallocated space
	logicalsize = newsize;
	return;
    }
    
    // growing beyond preallocated space

    int nblocks = (int) ceil(((double)(newsize - allocatedsize))/blocksize.bs);
    allocatedsize += nblocks*blocksize.bs;
    TYPE *newdata = new TYPE [allocatedsize];
    if(allocatedsize > 0 && !newdata) {
      cerr << "Vec: failed to resize vec to " << allocatedsize
	   << " objects of size " << sizeof(TYPE) << "!" << endl;
      fatalerror();			// should throw an exception
    }
    for(int i=0; i<logicalsize; i++)
	newdata[i] = data[i];
    logicalsize = newsize;
    delete [] data;
    data = newdata;
}

template <class TYPE>
void Vec<TYPE>::resize(const int newsize, const TYPE &x) {

    if(newsize == logicalsize) return;	// no change
    
    if(newsize < logicalsize) {		// shrinking
	logicalsize = newsize;
	// here is where extra elements should be destructed. See p 173
	// R. Murray's "C++ Strategies and Tactics". The right thing to do
	// is to copy the first newsize elements of data and delete all of it.
	return;
    }
    
    if(newsize <= allocatedsize) {	// growing into preallocated space
	// initialize "new" elements
	for(int i=logicalsize; i<newsize; i++)
	    data[i] = x;
	// change size
	logicalsize = newsize;
	return;
    }
    
    // growing beyond preallocated space

    int nblocks = (int) ceil(((double)(newsize - allocatedsize))/blocksize.bs);
    allocatedsize += nblocks*blocksize.bs;
    TYPE *newdata = new TYPE [allocatedsize];
    if(allocatedsize > 0 && !newdata) {
      cerr << "Vec: failed to resize vec to " << allocatedsize
	   << "objects of size " << sizeof(TYPE) << "!" << endl;
      fatalerror();
    }
    // copy old data
    int i;
    for(i=0; i<logicalsize; i++)
	newdata[i] = data[i];
    // initialize new data
    for(i=logicalsize; i<newsize; i++)
	newdata[i] = x;
    logicalsize = newsize;
    delete [] data;
    data = newdata;
}

template <class TYPE>
int Vec<TYPE>::grow(const int n) {
    int newspot = logicalsize;
    resize(logicalsize + n);
    return newspot;
}

template <class TYPE>
int Vec<TYPE>::grow(const int n, const TYPE &x) {
    int newspot = logicalsize;
    resize(logicalsize + n, x);
    return newspot;
}

template <class TYPE>
int Vec<TYPE>::remove(const TYPE &x) {
  for(int i=0; i<logicalsize; i++) {
    if(x == data[i]) {
      for(int j=i+1; j<logicalsize; j++)
	data[j-1] = data[j];
      logicalsize--;
      return 1;
    }
  }
  return 0;
}

template <class TYPE>
int Vec<TYPE>::remove(const TYPE &x, int (*compare)(const TYPE&, const TYPE&)) {
  for(int i=0; i<logicalsize; i++) {
    if(compare(data[i], x)) {
      for(int j=i+1; j<logicalsize; j++)
	data[j-1] = data[j];
      logicalsize--;
      return 1;
    }
  }
  return 0;
}

// remove *all* members for which f(data[i]) == 1 
template <class TYPE>
int Vec<TYPE>::remove_conditional(int (*f)(const TYPE&)) {
  int j=0;			// next available spot
  int oldsize = logicalsize;
  for(int i=0; i<logicalsize; i++) {
    if(!f(data[i])) {
      data[j] = data[i];
      j++;
    }
  }
  logicalsize = j;
  return oldsize - logicalsize;
}
template <class TYPE>
int Vec<TYPE>::remove_conditional(int (*f)(TYPE&)) {
  int j=0;			// next available spot
  int oldsize = logicalsize;
  for(int i=0; i<logicalsize; i++) {
    if(!f(data[i])) {
      data[j] = data[i];
      j++;
    }
  }
  logicalsize = j;
  return oldsize - logicalsize;
}

// remove all members for which flag==1
template <class TYPE>
int Vec<TYPE>::remove_conditional(const Vec<int> &flag) {
  int j=0;
  int oldsize = logicalsize;
  for(int i=0; i<logicalsize; i++) {
    if(!flag[i]) {
      data[j] = data[i];
      j++;
    }
  }
  logicalsize = j;
  return oldsize - logicalsize;
}

template <class TYPE>
void Vec<TYPE>::remove_all(const TYPE &x) {
  int j=0;
  for(int i=0; i<logicalsize; i++) {
    if(!(data[i] == x)) {	// don't use !=, since TYPE might not have it
      data[j] = data[i];
      j++;
    }
  }
  logicalsize = j;
}

template <class TYPE>
void Vec<TYPE>::trim(const TYPE &x) {
    for(int i=logicalsize; i>=0; i--) {
	if(x == data[i]) {
	    for(int j=i+1; j<logicalsize; j++)
		data[j-1] = data[j];
	    logicalsize--;
	    return;
	}
    }
}

template <class TYPE>
void Vec<TYPE>::replace(const TYPE &a, const TYPE &b) {
  for(int i=0; i<logicalsize; i++)
    if(data[i] == a) {
      data[i] = b;
      return;
    }
}

// free space without deleting Vec
template <class TYPE>
void Vec<TYPE>::smash() {
  setphysicalsize(0);
}

template <class TYPE>
void Vec<TYPE>::setphysicalsize(int n) {
  allocatedsize = n;
  logicalsize = 0;
  delete [] data;
  data = new TYPE [allocatedsize];
  if(allocatedsize > 0 && !data) {
    cerr << "Vec: Failed to allocate space for " << allocatedsize
	 << " objects of size " << sizeof(TYPE) << "!" << endl;
    fatalerror();			// should throw an exception here?
  }
}

// private copying function

template <class TYPE>
void Vec<TYPE>::copydata(TYPE *dd) {
    for(int i=0; i<logicalsize; i++)
	data[i] = dd[i];
}

// for printing

template <class TYPE>
ostream& operator<<(ostream &s, const Vec<TYPE> &v) {
    for(int i=0; i<v.logicalsize; i++)
	s << v[i] << " ";
    return s;
}

#endif
