// -*- C++ -*-
// $RCSfile: mouseclick.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:42 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// MouseClick contains the physical coordinates (doubles), the pixmap
// coordinates, and the window coordinates (XPoint).

#ifndef MOUSECLICK_H
#define MOUSECLICK_H

#include <X11/Xlib.h>
#include "cell_coordinate.h"
#include "meshcoord.h"
#include "enum.h"
#include <iostream.h>

typedef enum {MOUSE_UP, MOUSE_DN, MOUSE_MV, MOUSE_TRAP} MouseEvent;

class MouseClick {
private:
  void setup(const XEvent*);
public:
  MouseClick() : event(MOUSE_TRAP) {}
  MouseClick(double, double, const XPoint&, int, const XEvent*);
  MouseClick(double, double, int, int, int, const XEvent*);
  MeshCoord coord;		// physical coordinates
  Cell_coordinate cell;		// pixmap coordinates
  XPoint xpoint;		// window coordinates
  unsigned int button;
  unsigned int shiftkey;
  int window;
  Enum<MouseEvent> event;

  static void init();

  int operator!() const { return event == MOUSE_TRAP ? 1 : 0; }
};

ostream &operator<<(ostream&, const MouseClick&);
istream &operator>>(istream&, MouseClick&);

#endif
