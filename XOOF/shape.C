// -*- C++ -*-
// $RCSfile: shape.C,v $
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


#include "shape.h"

Rectangle::Rectangle(const MeshCoord &a, const MeshCoord &b) {
  if(a.x < b.x) {
    xmin = a.x;
    xmax = b.x;
  }
  else {
    xmin = b.x;
    xmax = a.x;
  }
  if(a.y < b.y) {
    ymin = a.y;
    ymax = b.y;
  }
  else {
    ymin = b.y;
    ymax = a.y;
  }
}


void Rectangle::swallow(const MeshCoord &a) {
  double x = a.x;
  double y = a.y;
  if(xmin > x) xmin = x;
  if(xmax < x) xmax = x;
  if(ymin > y) ymin = y;
  if(ymax < y) ymax = y;
}

void Rectangle::swallow(const Rectangle &r) {
  if(xmin > r.xmin) xmin = r.xmin;
  if(xmax < r.xmax) xmax = r.xmax;
  if(ymin > r.ymin) ymin = r.ymin;
  if(ymax < r.ymax) ymax = r.ymax;
}

void Rectangle::swell(double xfactor, double yfactor) {
  double xmid = 0.5*(xmax + xmin);
  double ymid = 0.5*(ymax + ymin);
  double xrng = 0.5*xfactor*(xmax - xmin);
  double yrng = 0.5*yfactor*(ymax - ymin);
  xmin = xmid - xrng;
  xmax = xmid + xrng;
  ymin = ymid - yrng;
  ymax = ymid + yrng;
}

void Rectangle::shift(const MeshCoord &a) {
  xmin += a.x;
  xmax += a.x;
  ymin += a.y;
  ymax += a.y;
}

// expand to achieve aspect ratio w/h
void Rectangle::aspect(double w, double h) {
  double myw = width();
  double myh = height();
  if(myw == 0.0 && myh == 0.0) return;
  if(myw == 0.0) {
    double xrng = 0.5*myh*w/h;
    xmin -= xrng;
    xmax += xrng;
    return;
  }
  if(myh == 0.0) {
    double yrng = 0.5*myw*h/w;
    ymin -= yrng;
    ymax += yrng;
    return;
  }
  if(myh*w > myw*h)   // increase myw
    swell(w*myh/(myw*h), 1.0);
  else
    swell(1.0, h*myw/(myh*w));
}

