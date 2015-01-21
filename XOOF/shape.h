// -*- C++ -*-
// $RCSfile: shape.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:14 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "vec.h"
#include "meshcoord.h"

class Shape {
    public:
	Shape() {}
	virtual ~Shape() {}
	virtual int contains(const MeshCoord &) const = 0;
};

class Rectangle : public Shape {
public:
  double xmin, xmax, ymin, ymax;
  Rectangle() : xmin(0), xmax(0), ymin(0), ymax(0) {}
  Rectangle(double xmn, double xmx, double ymn, double ymx)
    : xmin(xmn), xmax(xmx), ymin(ymn), ymax(ymx)
  {}
  Rectangle(const MeshCoord&, const MeshCoord&);

  virtual int contains(const MeshCoord &a) const {
    return (a.x <= xmax && a.x >= xmin &&
	    a.y <= ymax && a.y >= ymin);
  }
	
  // routines that modify the rectangle
  void swallow(const MeshCoord &a);
  void swallow(const Rectangle &r);
  void swell(double xfactor, double yfactor);
  void swell(double factor) { swell(factor, factor); }
  void shift(const MeshCoord &);
  void aspect(double x, double y);
	
  double height() const { return ymax - ymin; }
  double width() const { return xmax - xmin; }
};

class Circle : public Shape {
    public:
	double x, y, r;
	Circle() : x(0), y(0), r(0) {}
	Circle(const double xx, const double yy, const double rr)
	    : x(xx), y(yy), r(rr) {}
	virtual int contains(const MeshCoord &a) const {
	    double dx = a.x - x;
	    double dy = a.y - y;
	    return (dx*dx + dy*dy <= r*r);
	}
};

class Ellipse : public Shape {
    public:
	double ainv, binv;	// inverses of the semi-axes
	double x, y;		// center
	Ellipse(const double xx, const double yy,
		const double aa, const double bb)
	    : x(xx), y(yy), ainv(1.0/aa), binv(1.0/bb) {}
	virtual int contains(const MeshCoord &r) const {
	    double dx = ainv*(r.x - x);
	    double dy = binv*(r.y - y);
	    return (dx*dx + dy*dy <= 1.0);
	}
	// NO---  need the xy term too!
};

class Conglomerate : public Shape {
    private:
	Vec<Shape*> shapelist;
    public:
	Conglomerate() {}
	void AddShape(Shape *sh) {
	    shapelist.grow(1, sh);
	}
	virtual int contains(const MeshCoord &a) const {
	    for(int i=0; i<shapelist.capacity(); i++)
		if(shapelist[i]->contains(a))
		    return 1;
	    return 0;
	}
};

#endif
