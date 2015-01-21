// -*- C++ -*-
// $RCSfile: array.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-09-06 20:11:23 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef ARRAY_C
#define ARRAY_C

#include "array.h"
#include "cell_coordinate.h"
#include <string.h>

template <class TYPE>
Array<TYPE>::Array(const int h, const int w)
  : ArrayBase(h, w)
{
  allocate();
}

template <class TYPE>
Array<TYPE>::~Array() {
  free();
}

template <class TYPE>
void Array<TYPE>::allocate() {
  data = new TYPE*[height];
  if(!data) {
    cerr << "Array: Failed to allocate array of " << height
	 << " pointers!" << endl;
    exit(1);			// should throw an exception
  }
  data[0] = new TYPE[height*width];
  if(!data[0]) {
    cerr << "Array: Failed to allocate array of " << width << "x" << height
	 << " objects of size " << sizeof(TYPE) << "!" << endl;
    exit(1);			// should throw an exception here.
  }
  for(int i=1; i<height; i++)
    data[i] = data[i-1] + width;
}

template <class TYPE>
void Array<TYPE>::free() {
  if(data) {
    delete [] data[0];
    delete [] data;
  }
}

template <class TYPE>
Array<TYPE> &Array<TYPE>::operator=(const Array &a) {
  if(this != &a) {
    if(height != a.height || width != a.width) {
      free();
      height = a.height;
      width = a.width;
      allocate();
    }
    (void) memcpy(data[0], a.data[0], height*width*sizeof(TYPE));
  }
  return *this;
}

template <class TYPE>
Array<TYPE>::Array(const Array &a) {
  height = a.height;
  width = a.width;
  allocate();
  (void) memcpy(data[0], a.data[0], height*width*sizeof(TYPE));
}

template <class TYPE>
void Array<TYPE>::clear(const TYPE& t) {
  for(int i=0; i<width*height; i++)
    data[0][i] = t;
}

template <class TYPE>
TYPE &Array<TYPE>::operator[](const Cell_coordinate &z) {
#ifdef DEBUG
  assert(z.y < height);
  assert(z.x < width);
  assert(z.y >= 0);
  assert(z.x >= 0);
#endif
  return data[z.y][z.x];
}

template <class TYPE>
const TYPE &Array<TYPE>::operator[](const Cell_coordinate &z) const {
#ifdef DEBUG
  assert(z.y < height);
  assert(z.x < width);
  assert(z.y >= 0);
  assert(z.x >= 0);
#endif
  return data[z.y][z.x];
}

#endif
