// -*- C++ -*-
// $RCSfile: imagecanvas.h,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2001-02-16 18:06:10 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef IMAGECANVAS_H
#define	IMAGECANVAS_H

class ImageCanvas;

class Color;
class ImageFormGfx;
class MouseClick;
class MeshCoord;
class Image;
class RubberBand;

#include "scrollcanvas.h"
#include "vec.h"

class ImageCanvas : public ScrollCanvas {
private:
  int imagew, imageh;
  ImageFormGfx *imageform;
  enum DragMode {DM_NOTHING, DM_RUBBERBAND, DM_PAN};
  DragMode dragmode;
  RubberBand *rubberband;

  Vec<int> pixind_x;
  Vec<int> pixind_y;
  bool pixind_outofdate;
public:
  ImageCanvas(ImageFormGfx*, int, int, int, int, int, int, double,
	      void (*)(const MeshCoord&, const MeshCoord&, void*));
  virtual ~ImageCanvas() {}

  virtual void redraw();
  virtual void mouse(const MouseClick&);

  Colormap get_colormap() const { return colormap; }

  XPoint mesh2xpoint(const MeshCoord &c) const { // physical --> screen
    return ScrollCanvas::xpoint(c.x, c.y);
  }

  Cell_coordinate mouselocation(const MouseClick &); // screen --> physical
  virtual void resizepixmap(int, int);

  void loadimage(Image*);
  void draw_pixels(const Vec<Cell_coordinate>&, const Color&);
  void draw_pixel(const Cell_coordinate&, const Color&);
  
  // set what to do when the mouse is dragged
  void drag_rubberband() { dragmode = DM_RUBBERBAND; }
  void drag_pan() { dragmode = DM_PAN; }
  void drag_nothing() { dragmode = DM_NOTHING; }

  // how far the mouse has to move to invoke the drag routines.
  static const int dragmove;
};

#endif
