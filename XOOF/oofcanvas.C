// -*- C++ -*-
// $RCSfile: oofcanvas.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:12 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "dashboard.h"
#include "drawer.h"
#include "formdrawer.h"
#include "mouseclick.h"
#include "oofcanvas.h"
#include "rubberband.h"

static const int dragmove = 2;

OofCanvas::OofCanvas(FormDrawer *fd, int x, int y, int w, int h,
		     Colormap xcmap,
		     void (*scb)(const MeshCoord&, const MeshCoord&, void*))
 : ScrollCanvas(fd->the_form, x, y, w, h, scb, fd),
   formdrawer(fd),
   rubberband(0),
   dragmode(DM_NOTHING)
{
  set_colormap(xcmap);
}

OofCanvas::~OofCanvas() {
  if(rubberband)
    delete rubberband;
}

void OofCanvas::redraw() { // called by ScrollCanvas base class
  if(!holding)
    formdrawer->current_drawer->draw();
}

void OofCanvas::mouse(const MouseClick &click) {
  garcon()->log("graphics " + formdrawer->formdrawer_id +
		" mouse click = " + to_charstring(click));

  // variables used during mouse motion don't have to be class
  // members, since there is only one mouse.
  if(holding) return;
  static int panning = 0;    // currently panning?

  if(click.event == MOUSE_DN)
    return; // don't actually do anything

  if(click.event == MOUSE_UP) {
    // ignore some cases that we can't understand
    if(!downclick()) return; 
    if(rubberband) {  // stop rubberbanding
      delete rubberband;
      rubberband = 0;
    }
    else if(panning) { // stop panning
      panning = 0;
      return;
    }
    if(click.window != downclick().window) return;
    if(click.button != downclick().button) return;

    if(abs(click.xpoint.x - downclick().xpoint.x) > dragmove ||
       abs(click.xpoint.y - downclick().xpoint.y) > dragmove)
      formdrawer->current_dashboard->draghandler(downclick(), click);
    else
      formdrawer->current_dashboard->clickhandler(click);
    
    return;
  }

  if(click.event == MOUSE_MV) {
    if(click.button == 0) return; // ignore motion w/out button
    switch(dragmode) {
    case DM_RUBBERBAND:
      if(!rubberband)
	rubberband =
	  newrubberband(formdrawer->current_drawer->
			current_dashboard->rubberbandtype,
			downclick().xpoint, click.xpoint,
			window(),
			formdrawer->current_drawer->rubberbandpixel());
      else
	rubberband->redraw(click.xpoint);
      break;
    case DM_PAN:
      if(click.button == 2) {
	static int lastx, lasty;
	if(!panning) {
	  lastx = downclick().xpoint.x;
	  lasty = downclick().xpoint.y;
	  panning = 1;
	}
	int x = click.xpoint.x;
	int y = click.xpoint.y;
	scroll(lastx-x, lasty-y);
	lastx = x;
	lasty = y;
      }
      break;
    }
  }
  return;
}
