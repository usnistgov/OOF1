// -*- C++ -*-
// $RCSfile: rubberband.C,v $
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



#include "colorutils.h"
#include "rubberband.h"

RubberBand::RubberBand(const XPoint &p1, const XPoint &p2, Window w,
		       unsigned long pxl)
  : start(p1),
    current(p2),
    window(w),
    pixel(pxl)
{
  gc = XCreateGC(gfxinfo.display(), gfxinfo.window(), 0, 0);
  XSetFunction(gfxinfo.display(), gc, GXxor);
  XSetForeground(gfxinfo.display(), gc, pixel);
  XSetLineAttributes(gfxinfo.display(), gc,
		     2, LineSolid, CapButt, JoinBevel);
}

RubberBand::~RubberBand() {
  XFreeGC(gfxinfo.display(), gc);
}

void RubberBand::redraw(const XPoint &newpt) {
  draw();  // erase
  current = newpt;
  draw();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

RectangleRubberBand::RectangleRubberBand(const XPoint &p1, const XPoint &p2,
		      Window w, unsigned long pxl)
  : RubberBand(p1, p2, w, pxl)
{
  draw();
}

RectangleRubberBand::~RectangleRubberBand() {
  draw();			// erase
}

void RectangleRubberBand::draw() const {
  short xmin, xmax, ymin, ymax;
  if(start.x < current.x) {
    xmin = start.x;
    xmax = current.x;
  }
  else {
    xmin = current.x;
    xmax = start.x;
  }

  if(start.y < current.y) {
    ymin = start.y;
    ymax = current.y;
  }
  else {
    ymin = current.y;
    ymax = start.y;
  }

  XDrawRectangle(gfxinfo.display(), window, gc, xmin, ymin,
		 xmax-xmin, ymax-ymin);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

LineRubberBand::LineRubberBand(const XPoint &p1, const XPoint &p2,
			       Window w, unsigned long pxl)
  : RubberBand(p1, p2, w, pxl)
{
  draw();
}

LineRubberBand::~LineRubberBand() {
  draw();			// erase
}

void LineRubberBand::draw() const {
  XDrawLine(gfxinfo.display(), window, gc, start.x, start.y,
	    current.x, current.y);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

RubberBand *newrubberband(RubberBandType type,
			  const XPoint &p1, const XPoint &p2,
			  Window w, unsigned long pxl)
{
  switch (type) {
  case RB_NONE:
    return 0;
  case RB_RECTANGLE:
    return new RectangleRubberBand(p1, p2, w, pxl);
  case RB_LINE:
    return new LineRubberBand(p1, p2, w, pxl);
  }
  return 0;
}
