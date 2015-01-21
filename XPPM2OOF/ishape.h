// -*- C++ -*-
// $RCSfile: ishape.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-09-06 20:11:37 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Shapes with integer coordinates (shapes defined on an image)

#ifndef ISHAPE_H
#define ISHAPE_H

#include "array.h"

class Cell_Coordinate;
class ImageRect;

class iShape {
public:
  virtual int contains(const Cell_coordinate&) const = 0;
  virtual ImageRect containing_rectangle() const = 0;
};

// A rectangle is defined by two diagonally opposing corners.

class ImageRect : public iShape {
public:
  int xmin, xmax, ymin, ymax;
  ImageRect() : xmin(0), xmax(0), ymin(0), ymax(0) {}
  ImageRect(const Cell_coordinate &c1, const Cell_coordinate &c2);
  int width() const { return xmax - xmin; }
  int height() const { return ymax - ymin; }
  virtual int contains(const Cell_coordinate &) const;
  virtual ImageRect containing_rectangle() const { return *this; }
  friend ImageRect operator&&(const ImageRect&, const ImageRect&); // intersect.
  friend ostream& operator<<(ostream &, const ImageRect&);
};

#ifdef OLD
// A circle is defined by its center and a point on the perimeter.

class ImageCircle : public iShape {
private:
  int xcenter, ycenter;
  int r, rsquare;
  ImageRect container;
public:
  ImageCircle() : xcenter(0), ycenter(0), r(0), rsquare(0) {}
  ImageCircle(const Cell_coordinate &c1, const Cell_coordinate &c2);
  virtual int contains(const Cell_coordinate &) const;
  virtual ImageRect containing_rectangle() const { return container; }
};

// An ellipse is defined by the box that it fits into. The axes must
// be along x and y.

class ImageEllipse : public iShape {
private:
  int xcenter, ycenter;
  double invxaxis, invyaxis;
  int null;			// is this a degenerate ellipse?
  ImageRect container;
public:
  ImageEllipse() : xcenter(0), ycenter(0), invxaxis(0), invyaxis(0), null(1) {}
  ImageEllipse(const Cell_coordinate &c1, const Cell_coordinate &c2);
  virtual int contains(const Cell_coordinate&) const;
  virtual ImageRect containing_rectangle() const { return container; }
};

class iShapeIterator {
private:
  ArrayIterator iter;
  const iShape &shape;
public:
  iShapeIterator(const iShape &sh)
    : shape(sh),
      iter(sh.containing_rectangle())
    {}
  int operator()(Cell_coordinate &pixel);
  void reset() { iter.reset(); }
};

#endif // OLD
#endif
