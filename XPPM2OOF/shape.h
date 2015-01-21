// -*- C++ -*-
// $RCSfile: shape.h,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2001-02-16 14:53:54 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Shapes with integer coordinates (shapes defined on an image)

#ifndef SHAPE_H
#define SHAPE_H

class Shape;
class Intersection;

#include "array.h"
#include "ishape.h"
#include "meshcoord.h"

class Cell_coordinate;
class ImageRect;

class Shape {
public:
  virtual ~Shape() {}
  virtual int contains(const MeshCoord&) const = 0;
  virtual ImageRect containing_rectangle() const = 0;
  virtual Shape *clone() const = 0;
};

// A rectangle is defined by two diagonally opposing corners.

class Rectangle : public Shape {
public:
  MeshCoord ll;			// lower left
  MeshCoord ur;			// upper right
  Rectangle() {}
  Rectangle(const MeshCoord &c1, const MeshCoord &c2);
  double width() const;
  double height() const;
  virtual int contains(const MeshCoord&) const;
  virtual ImageRect containing_rectangle() const;
  virtual Shape *clone() const;
};

// A circle is defined by its center and a point on the perimeter.

class Circle : public Shape {
private:
  double xcenter, ycenter;
  double r, rsquare;
  ImageRect container;
public:
  Circle() : xcenter(0), ycenter(0), r(0), rsquare(0) {}
  Circle(const MeshCoord &c1, const MeshCoord &c2);
  virtual int contains(const MeshCoord &) const;
  virtual ImageRect containing_rectangle() const { return container; }
  virtual Shape *clone() const;
};

// An ellipse is defined by the box that it fits into. The axes must
// be along x and y.

class Ellipse : public Shape {
private:
  double xcenter, ycenter;
  double invxaxis, invyaxis;
  bool null;			// is this a degenerate ellipse?
  ImageRect container;
public:
  Ellipse() : xcenter(0), ycenter(0), invxaxis(0), invyaxis(0), null(1) {}
  Ellipse(const MeshCoord &c1, const MeshCoord &c2);
  virtual int contains(const MeshCoord&) const;
  virtual ImageRect containing_rectangle() const { return container; }
  virtual Shape *clone() const;
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class Intersection : public Shape {
private:
  // have to have a copy of the shapes, in case the constructor's
  // arguments are temporary variables.
  const Shape *shape1;
  const Shape *shape2;
  ImageRect container;
public:
  Intersection(const Shape&, const Shape&);
  virtual ~Intersection();
  Shape *clone() const;
  virtual int contains(const MeshCoord &) const;
  virtual ImageRect containing_rectangle() const; // { return container; }
};

Intersection operator&&(const Shape &s1, const Shape &s2);

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


class ShapeIterator {
private:
  ArrayIterator iter;
  const Shape *shape;
public:
  ShapeIterator(const Shape &sh)
    : shape(sh.clone()),
      iter(sh.containing_rectangle())
    {}
  ~ShapeIterator();
  int operator()(Cell_coordinate &pixel);
  void reset() { iter.reset(); }
  int upperbound() const;	// an upper bound on the number of pixels
};

#endif
