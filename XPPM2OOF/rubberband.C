// -*- C++ -*-
// $RCSfile: rubberband.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-30 19:23:29 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "colorutils.h"
#include "math.h"
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
  XSetLineAttributes(gfxinfo.display(), gc, 2, LineSolid, CapButt, JoinBevel);
  draw();
}

RectangleRubberBand::~RectangleRubberBand() {
  draw(); // erase
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

CircleRubberBand::CircleRubberBand(const XPoint &p1, const XPoint &p2,
		      Window w, unsigned long pxl)
  : RubberBand(p1, p2, w, pxl)
{
  XSetLineAttributes(gfxinfo.display(), gc, 1, LineSolid, CapButt, JoinBevel);
  draw();
}

CircleRubberBand::~CircleRubberBand() {
  draw(); // erase
}

void CircleRubberBand::draw() const {
  short dx = current.x - start.x;
  short dy = current.y - start.y;
  short r = (short) sqrt(double(dx*dx + dy*dy));
  XDrawArc(gfxinfo.display(), window, gc, start.x - r, start.y - r, 2*r, 2*r,
	   0, 23040);		// 23040 = 360*64
  XDrawLine(gfxinfo.display(), window, gc,
	    start.x, start.y, current.x, current.y);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

EllipseRubberBand::EllipseRubberBand(const XPoint &p1, const XPoint &p2,
		      Window w, unsigned long pxl)
  : RubberBand(p1, p2, w, pxl)
{
  XSetLineAttributes(gfxinfo.display(), gc, 1, LineSolid, CapButt, JoinBevel);
  draw();
}

EllipseRubberBand::~EllipseRubberBand() {
  draw(); // erase
}

void EllipseRubberBand::draw() const {
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

  XDrawArc(gfxinfo.display(), window, gc, xmin, ymin, xmax-xmin, ymax-ymin,
	   0, 23040);		// 23040 = 360*64
//   XDrawLine(gfxinfo.display(), window, gc,
// 	    start.x, start.y, current.x, current.y);
}
