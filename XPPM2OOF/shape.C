// -*- C++ -*-
// $RCSfile: shape.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2000-09-06 20:11:48 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Shapes with integer coordinates (shapes defined on an image)

#include "cell_coordinate.h"
#include "ishape.h"
#include "meshcoord.h"
#include "shape.h"
#include <math.h>

Rectangle::Rectangle(const MeshCoord &c1, const MeshCoord &c2) {
  if(c1.x < c2.x) {
    ll.x = c1.x;
    ur.x = c2.x;
  }
  else {
    ll.x = c2.x;
    ur.x = c1.x;
  }
  if(c1.y < c2.y) {
    ll.y = c1.y;
    ur.y = c2.y;
  }
  else {
    ll.y = c2.y;
    ur.y = c1.y;
  }
}

Shape *Rectangle::clone() const {
  return new Rectangle(*this);
}

ImageRect Rectangle::containing_rectangle() const {
  return ImageRect(mesh2cell_dn(ll), mesh2cell_up(ur));
}

int Rectangle::contains(const MeshCoord &c) const {
  return (c.x >= ll.x && c.x < ur.x && c.y >= ll.y && c.y < ur.y);
}

double Rectangle::width() const {
  return ur.x - ll.x;
}

double Rectangle::height() const {
  return ur.y - ll.y;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Circle::Circle(const MeshCoord &c1, const MeshCoord &c2)
  : xcenter(c1.x), ycenter(c1.y)
{
  double dx = c1.x - c2.x;
  double dy = c1.y - c2.y;
  rsquare = dx*dx + dy*dy;
  r = sqrt(rsquare);
  MeshCoord diag(M_SQRT2*r, M_SQRT2*r); // diagonal of containing box
  container = ImageRect(mesh2cell_dn(c1-diag), mesh2cell_up(c1+diag));
}

int Circle::contains(const MeshCoord &c) const {
  double dx = c.x - xcenter;
  double dy = c.y - ycenter;
  return dx*dx + dy*dy < rsquare;
}

Shape *Circle::clone() const {
  return new Circle(*this);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Ellipse::Ellipse(const MeshCoord &c1, const MeshCoord &c2)
  : container(mesh2cell_dn(c1), mesh2cell_up(c2)), null(0)
{
  double dx = c2.x - c1.x;
  double dy = c2.y - c1.y;
  if(dx == 0.0 || dy == 0.0) {
    null = 1;
    return;
  }
  invxaxis = 4./(dx*dx);	// 1/semi-axis^2
  invyaxis = 4./(dy*dy);
  xcenter = 0.5*(c1.x + c2.x);
  ycenter = 0.5*(c1.y + c2.y);
}

int Ellipse::contains(const MeshCoord &c) const {
  if(null) return 0;
  double dx = c.x - xcenter;
  double dy = c.y - ycenter;
  return dx*dx*invxaxis + dy*dy*invyaxis < 1.0;
}

Shape *Ellipse::clone() const {
  return new Ellipse(*this);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Intersection::Intersection(const Shape &s1, const Shape &s2) 
  : shape1(s1.clone()), shape2(s2.clone())
{
  ImageRect r1 = shape1->containing_rectangle();
  ImageRect r2 = shape2->containing_rectangle();
  container = r1 && r2;
}

Intersection::~Intersection() {
  delete shape1;
  delete shape2;
}

Shape *Intersection::clone() const {
  return new Intersection(*shape1, *shape2);
}

int Intersection::contains(const MeshCoord &c) const {
  return shape1->contains(c) && shape2->contains(c);
}

ImageRect Intersection::containing_rectangle() const {
  return container;
}

Intersection operator&&(const Shape &s1, const Shape &s2) {
  return Intersection(s1, s2);
}



//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int ShapeIterator::operator()(Cell_coordinate &pixel) {
  while(iter(pixel))
    if(shape->contains(cell2mesh(pixel)))
      return 1;
  return 0;
}

ShapeIterator::~ShapeIterator() {
  delete shape;
}

int ShapeIterator::upperbound() const {
  ImageRect boundingbox = shape->containing_rectangle();
  return boundingbox.height()*boundingbox.width();
}
