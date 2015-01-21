// -*- C++ -*-
// $RCSfile: oofcanvas.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:09 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef OOFCANVAS_H
#define OOFCANVAS_H

class OofCanvas;

#include "scrollcanvas.h"

class FormDrawer;
class MouseClick;
class RubberBand;
class MeshCoord;

class OofCanvas : public ScrollCanvas {
private:
  enum DragMode {DM_NOTHING, DM_RUBBERBAND, DM_PAN};
  DragMode dragmode;
  FormDrawer *formdrawer;
  RubberBand *rubberband;
public:
  OofCanvas(FormDrawer*, int, int, int, int, Colormap,
	    void (*)(const MeshCoord&, const MeshCoord&, void*));
  virtual ~OofCanvas();
  virtual void redraw();
  virtual void mouse(const MouseClick&);
  Colormap get_colormap() const { return colormap; }

  // set what to do when the mouse is dragged
  void drag_rubberband() { dragmode = DM_RUBBERBAND; }
  void drag_pan() { dragmode = DM_PAN; }
  void drag_nothing() { dragmode = DM_NOTHING; }
};

#endif
