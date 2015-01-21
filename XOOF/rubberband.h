// -*- C++ -*-
// $RCSfile: rubberband.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:13 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef RUBBERBAND_H
#define RUBBERBAND_H

#include <X11/Xlib.h>

// The RubberBand class is an abstract base class. The derived classes
// need to provide a "draw()" function which draws the rubberband in
// window, using gc, with the positions given by "start" and
// "finish". The derived class's constructor and destructor should
// both call draw().

class RubberBand {
protected:
  XPoint start;
  XPoint current;
  Window window;
  unsigned long pixel;
  GC gc;
public:
  RubberBand(const XPoint&, const XPoint&, Window w, unsigned long pxl);
  virtual ~RubberBand();
  virtual void draw() const = 0; // does actual drawing, called by redraw()
  void redraw(const XPoint&); // undraw, change current point, draw
};

enum RubberBandType {RB_NONE, RB_RECTANGLE, RB_LINE};

RubberBand *newrubberband(RubberBandType, const XPoint &p1, const XPoint &p2,
			  Window w, unsigned long pxl);

class RectangleRubberBand : public RubberBand {
public:
  RectangleRubberBand(const XPoint&, const XPoint&, Window, unsigned long);
  virtual ~RectangleRubberBand();
  virtual void draw() const;
};

class LineRubberBand : public RubberBand {
public:
  LineRubberBand(const XPoint&, const XPoint&, Window, unsigned long);
  virtual ~LineRubberBand();
  virtual void draw() const;
};

#endif


