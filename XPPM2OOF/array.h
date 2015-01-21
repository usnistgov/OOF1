// -*- C++ -*-
// $RCSfile: array.h,v $
// $Revision: 1.5 $
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

#ifndef ARRAY_H
#define ARRAY_H

class Cell_coordinate;
class ImageRect;

#include "cell_coordinate.h"
#include <string.h>
#include <assert.h>

class ArrayBase {
protected:
  int height, width;
  virtual void allocate() = 0;
  virtual void free() = 0;
public:
  ArrayBase() : height(0), width(0) {}
  ArrayBase(const int h, const int w) : height(h), width(w) {}
  virtual ~ArrayBase() {}
  int query_height() const { return height; }
  int query_width() const { return width; }
  Cell_coordinate query_size() const { return Cell_coordinate(width, height); }
  int contains(const Cell_coordinate&) const;
  void resize(const Cell_coordinate &size);
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

template <class TYPE>
class Array : public ArrayBase {
protected:
  TYPE **data;
private:
  virtual void allocate();
  virtual void free();
public:
  Array() : data(0) {}
  
  Array(const int h, const int w);
  Array(const Array&);
  virtual ~Array();

  Array &operator=(const Array &a);
  
  virtual void clear(const TYPE& t);
  
  TYPE &operator[](const Cell_coordinate &z);

  const TYPE &operator[](const Cell_coordinate &z) const;

  TYPE **ptrptr() { return data; } // caveat emptor

  // Don't use these! access via Cell_coordinate avoids confusion
  // about which is x and which is y.
  //	TYPE &operator()(const int i, const int j)       { return data[i][j]; }
  //	TYPE  operator()(const int i, const int j) const { return data[i][j]; }
};

typedef enum {ARI_INCLUSIVE, ARI_EXCLUSIVE} ARI_Type;

class ArrayIterator {
private:
  int i, j;     // current point
  int x1, y1;   // upper limits
  int x0, y0;   // lower limits
public:
  ArrayIterator(const ArrayBase &a)
    : i(0), j(0), x0(0), y0(0), y1(a.query_height()), x1(a.query_width()) {}
  // iterate over a rectangular subregion
  ArrayIterator(const ImageRect &r, const ARI_Type flag = ARI_INCLUSIVE);
  int operator()(Cell_coordinate &pixel);
  void reset();
};

#include "array.C"

#endif
