// -*- C++ -*-
// $RCSfile: mouseclick.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:07 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "mouseclick.h"
#include <iostream.h>

MouseClick::MouseClick(double xx, double yy, int ix, int iy,
		       int n, const XEvent *xev)
  : cell(ix, iy),
    coord(xx, yy),
    window(n)
{
  setup(xev);
}

MouseClick::MouseClick(double xx, double yy,
		       const XPoint &xp, int n, const XEvent *xev)
  : cell(xp.x, xp.y),
    coord(xx, yy),
    window(n)
{
  setup(xev);
}

void MouseClick::setup(const XEvent *xev) {
  switch(xev->type) {
  case ButtonPress:
    event = MOUSE_DN;
    shiftkey = xev->xbutton.state & ShiftMask;
    button = xev->xbutton.button;
    xpoint.x = xev->xbutton.x;
    xpoint.y = xev->xbutton.y;
    break;
  case ButtonRelease:
    event = MOUSE_UP;
    shiftkey = xev->xbutton.state & ShiftMask;
    button = xev->xbutton.button;
    xpoint.x = xev->xbutton.x;
    xpoint.y = xev->xbutton.y;
    break;
  case MotionNotify:
    event = MOUSE_MV;
    if(xev->xmotion.state & Button1Mask)
      button = 1;
    else if(xev->xmotion.state & Button2Mask)
      button = 2;
    else if(xev->xmotion.state & Button3Mask)
      button = 3;
    else
      button = 0;
    shiftkey = xev->xmotion.state & ShiftMask;
    xpoint.x = xev->xmotion.x;
    xpoint.y = xev->xmotion.y;
    break;
  default:
    cerr << "MouseClick got an unknown event!" << endl;
    break;
  }
}

void MouseClick::init() {
  static int initted = 0;
  if(!initted) {
    initted = 1;
    Enum<MouseEvent>::name(MOUSE_UP, "up");
    Enum<MouseEvent>::name(MOUSE_DN, "dn");
    Enum<MouseEvent>::name(MOUSE_MV, "mv");
    Enum<MouseEvent>::name(MOUSE_TRAP, "trap");
  }
}

ostream &operator<<(ostream &os, const MouseClick &click) {
  os << click.coord << " "
     << click.cell << " "
     << click.xpoint.x << " " << click.xpoint.y << " "
     << click.button << " "
     << click.shiftkey << " "
     << click.window << " "
     << click.event;
  return os;
}

istream &operator>>(istream &is, MouseClick &click) {
  if(is) {
    is >> click.coord;
    is >> click.cell;
    is >> click.xpoint.x >> click.xpoint.y;
    is >> click.button;
    is >> click.shiftkey;
    is >> click.window;
    is >> click.event;
  }
  return is;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Include template definitions
#include "enumio.C"
