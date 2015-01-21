// -*- C++ -*-
// $RCSfile: scrollcanvas.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2001-02-16 14:53:54 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Scrolled canvas object for installing in a form.

// The canvas keeps a pixmap for redrawing.

// The constructor's arguments are
//    FL_FORM *form:  the form in which to install the object
//    int x, y:       the position in the form
//    int w, h:       the size of the object (canvas + scroll bars)
//    void (*)(const MeshCoord&, const MeshCoord&, void*):
//                    callback function to be called when the visible region
//                    changes. The first two args are the coordinates of
//                    the upper-left and lower-right corners of the region.
//                    The third argument is a user specified datum.
//    void *ptr:      The datum to be passed to the callback.

// This is a virtual base class. To be usable, the following virtual
// functions should be defined in a derived class:
//     void redraw()                  // redraw the contents of the pixmap
//     void mouse(const MouseClick&)  // handle mouse up down and motion events
// Null versions of these functions are defined in the base class, but
// the ScrollCanvas won't be very useful if they aren't overridden.
// The MouseClick class contains all the necessary info about the event.

// Don't draw on the canvas, draw on the pixmap [Pixmap ScrollCanvas::pixmap()]
// using ScrollCanvas::gc() and ScrollCanvas::display(),
// and then update the canvas via void ScrollCanvas::refresh().

// When drawing on the pixmap, use
//     void ScrollCanvas::setcoords(double xmin, double xmax,
//                                  double ymin, double ymax)
// to set the physical coordinate system, and
//     XPoint ScrollCanvas::xpoint(double x, double y)
// to convert to coordinates suitable for X drawing routines. The
// physical coordinates increase from the lower left corner. The
// screen coordinates (which you shouldn't have to use) increase from
// the upper left corner.
//
// When changing images, you may have to change the range of the
// physical coordinates. Use either
//     void setcoords_resize(double xmin, double xmax,
//                           double ymin, double ymax)
// which preserves the physical coordinates of the visible region,
// or
//     void setcoords_reset(double xmin, double xmax,
//                          double ymin, double ymax)
// which makes the whole physical region visible.
//
// Other routines:
//  void show(), void hide() --- hide and show the whole scrollcanvas.
//  int visible() -- is the canvas visible?
//  void clear() -- clear the pixmap and the screen.
//  void clearpixmap()  -- clear the pixmap but keep the display the same.
//  int pwidth(), int pheight() -- get size of the pixmap (in pixels).
//  int vwidth(), int vheight() -- get size of the visible region (in pixels).
//  void setfillcolor(unsigned long) --
//         set color (pixel value) to use for background.
//  void setstipple(int w, int h, char *bits,
//                               unsigned long fg, unsigned long bg) --
//         set stipple pattern to use for background. The size of the stipple
//         is w by h, the pattern itself is stored in bits, the foreground
//         and background colors (pixel values) are fg and bg.
//  void zoom(double factor) -- zoom by factor around center of visible region.
//  void zoom(double factor, const XPoint &center) -- zoom around given point.
//  void setzoomfactor(double) -- set zoom factor for zoom button.
//  void fullzoom() -- make whole image visible on screen.
//  void resizepixmap(int w, int h) -- change size of pixmap
//  void scroll(int, int) -- scroll by given number of pixels in x and y.
//  void scroll(double, double) -- scroll by given distance in physical coords.
//  void do_scrollcallback() -- execute scroll callback function.
//  void set_xyscrollcallbacks(void (*fx)(int dx, void *arg),
//                             void (*fy)(int dy, void *arg),
//                             void *arg) --
//               Set additional functions to be called when the scroll bars 
//               are moved. The previously mentioned "scroll callback" is called
//               whenever the visible region changes. The xyscrollcallbacks are
//               called only when the scroll bars are used to change the visible
//               region. dx and dy are measured in pixels.
//  void set_zoombutton_callback(void (*f)(int, void*), void *arg) --
//               Set additional function to be called when the zoom button
//               is pressed. The int argument tells which mouse button was used.
//  void zoom_button(int) -- simulate a click on the zoom button. The argument
//              tells which mouse button was used (1, 2, or 3).
//  void set_resize_callback(void (*f)(int, int, void*), void *arg) --
//               Set a function to be called when the canvas size changes
//               The int arguments are the canvas size in pixels.	 
//  const MouseClick &downclick() -- find where the mouse was pressed last.
//  void robomouse(const MouseClick&) -- simulate a mouse click
//  void hold() -- disable scrolling and mousing.
//  void hold_release() -- enable scrolling and mousing.
//  void ppmfile(FILE*) -- save visible part of image to file in ppm format
//  void save_scroll(ostream&) const; -- save current position of scroll bars
//  void read_scroll(istream&); -- read and set current position of scroll bars
//  Rectangle visible_region() const; -- get visible region in real coords

#ifndef SCROLLCANVAS_H
#define SCROLLCANVAS_H

#include "forms.h"
#include "pixmap.h"
#include "mouseclick.h"

class MV_Vector_double;
class Rectangle;

class ScrollCanvas {
private:
  static int Ncanvases;
  int index;    // each canvas has a unique index

  FL_OBJECT *canvas;
  FL_OBJECT *horscrollbar;
  FL_OBJECT *verscrollbar;
  FL_OBJECT *zoombutton;
  FL_OBJECT *backdrop;

 // static callbacks are called directly by the forms library
  static void horscrollcallback(FL_OBJECT*, long);
  static void verscrollcallback(FL_OBJECT*, long);
  static void zoomcallback(FL_OBJECT*, long);
  static int exposecallback(FL_OBJECT*, Window, int, int, XEvent*, void*);
  static int mousedncallback(FL_OBJECT*, Window, int, int, XEvent*, void*);
  static int mouseupcallback(FL_OBJECT*, Window, int, int, XEvent*, void*);
  static int mousemvcallback(FL_OBJECT*, Window, int, int, XEvent*, void*);

  static MouseClick lastclick;

  // nonstatic callbacks are called by the static ones
  void buttonzoom();
  void exposehandler(Window, int, int);
  void mouseupcb(XEvent*);
  void mousedncb(XEvent*);
  void mousemvcb(XEvent*);
  void verscrollhandler();
  void horscrollhandler();

  int checkclick(int, int) const;

  PixMap pmap;

  int canvash, canvasw;  // current size of canvas, check for size change

  // scrolling region in pixmap coordinate system
  int xscrollmin, xscrollmax, yscrollmin, yscrollmax;

  // distance in pixels from upper left corner of pixmap to upper left
  // corner of canvas
  int xscroll, yscroll;

  // physical coords of corners of pixmap
  double xmin, xmax, ymin, ymax;

  double zoomfactor;

  // user function to be called when visible region has changed
  void (*scrollcallback)(const MeshCoord&, const MeshCoord&, void*);
  void *scrollcallbackptr;
  // user functions to be called when scrollbars are used
  void (*xscrollcallback)(int, void*);
  void (*yscrollcallback)(int, void*);
  void *xyscrollcallbackptr;
  // user function to be called when zoom button is used
  void (*zoombutton_callback)(int, void*);
  void *zoombutton_callbackptr;
  // user function to be called when size changes
  void (*resize_callback)(int, int, void*);
  void *resize_callbackptr;

  void setscrollbars();
  void clearcanvas();
  void copyimage(Drawable) const; // copy image onto given Window (or Pixmap)

  int backdropvisible;  // invisible if anything has been drawn

  // stuff for filling the part of the canvas not covered by the pixmap
  Pixmap stipple;
  int usestipple;
  int stipple_w, stipple_h;
  GC fillgc;

protected:
  virtual void redraw() {};                   // recreate contents of pixmap
  virtual void mouse(const MouseClick&) {};   // mouse handler

  Colormap colormap;

  int holding; // are scrolling and zooming turned off?

public:
  ScrollCanvas(FL_FORM*, // form in which canvas is to appear
	       int x, int y, int w, int h, // where and how big
	       void (*)(const MeshCoord&, const MeshCoord&, void*)=0,
	                         // function to call when visible region changes
	       void *scbptr=0);  // arg to pass to the function
  virtual ~ScrollCanvas();

  void show();
  void hide();

  static const int scrollbarwidth;

  // set conversion from physical to pixmap coordinates
  void setcoords(double x0, double x1, double y0, double y1);
  XPoint xpoint(double x, double y) const; // physical -> pixmap coords
  XPoint xpoint(const MV_Vector_double&) const;
  void physicalcoords(int, int, double&, double&) const; // pixmap -> physical
  Rectangle visible_region() const;

  void refresh();    // updates visible image using pixmap
  void clear();      // clears pixmap and canvas
  void clearpixmap();

  int pwidth() const { return pmap.width(); }  // size of pixmap
  int pheight() const { return pmap.height(); }
  int vheight() const { return canvash; }      // size of visible region
  int vwidth() const { return canvasw; }
  virtual void resizepixmap(int, int);	// doesn't redraw!


  // stuff for filling the part of the canvas not covered by the pixmap
  void setfillcolor(unsigned long p); // p is an X pixel value
  void setstipple(int w, int h, char *bits, unsigned long fg, unsigned long bg);

  // change size of pixmap by the given factor and redraw. 
  void zoom(double); // zoom around center of image
  void zoom(double, const XPoint&); // zoom around given point of pixmap
  // set zooming factor for zoombutton
  void setzoomfactor(double z) { zoomfactor = z; }

  // resize the pixmap by specifying the new coordinate system. The
  // scale will stay the same, and the coordinates of the visible
  // region will stay the same.
  void setcoords_resize(double x0, double x1, double y0, double y1);

  // set the new coordinate system while resizing the pixmap so that
  // it fits on the canvas.
  void setcoords_reset(double x0, double x1, double y0, double y1);

  void fullzoom();             // fit image to screen


  void scroll(int dx, int dy);  // scroll by given # of pixels
  void scroll(double dx, double dy);  // scroll by given physical amount
  void do_scrollcallback() const; // execute scrollcallback function
  XPoint get_scroll_pixels() const;

  // set functions to be called when scroll bars are used
  void set_xyscrollcallbacks(void (*fx)(int, void*), void (*fy)(int, void*),
			     void *ptr);

  // set function to be called when canvas size changes
  void set_resize_callback(void (*)(int, int, void*), void*);

  // simulate pressing the zoom button with given mouse button
  void zoom_button(int);
  // set function to be called when zoom button is used
  void set_zoombutton_callback(void (*f)(int, void*), void *ptr);

  const MouseClick &downclick() const; // position of mouse-down event
  void robomouse(const MouseClick&); // simulate a mouse click

  // turn interaction on and off
  void hold();
  void release_hold();

  Pixmap pixmap() const { return pmap.pixmap(); }
  GC gc() const { return pmap.gc(); }
  Window window() const { return fl_get_canvas_id(canvas); }

  void set_colormap(Colormap cm); // set X colormap for canvas

  int visible() const { return canvas->visible; }

  void ppmfile(FILE*) const; // save visible image to ppm file

  void save_scroll(ostream &) const;
  void read_scroll(istream &);

  friend void create_form_scrollcanvas(ScrollCanvas*, int, int, int, int);
};

#endif
