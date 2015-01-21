// -*- C++ -*-
// $RCSfile: scrollcanvas.C,v $
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


#include "colorutils.h"
#include "forms.h"
#include "menucolor.h"
#include "meshcoord.h"
#include "mouseclick.h"
#include "readbinary.h"
#include "scrollcanvas.h"
#include "shape.h"
#include "mvvd.h"
#include "vec.h"
#include <iostream.h>
#include <stdio.h>

const int ScrollCanvas::scrollbarwidth(20);  // thickness of scrollbars

int ScrollCanvas::Ncanvases(0);

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// The following is a modfied version of the fdesign code in scrcanvas.c

void create_form_scrollcanvas(ScrollCanvas *fdui,
				     int x, int y, int w, int h)
{
  FL_OBJECT *obj;

  fdui->backdrop = obj = fl_add_box(FL_FLAT_BOX,
				    x+ScrollCanvas::scrollbarwidth,
				    y+ScrollCanvas::scrollbarwidth,
				    w-ScrollCanvas::scrollbarwidth,
				    h-ScrollCanvas::scrollbarwidth,"");
  fl_set_object_color(obj,FL_WHITE,FL_COL1);
  fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fdui->horscrollbar = obj = fl_add_slider(FL_HOR_SLIDER,
					   x+ScrollCanvas::scrollbarwidth,
					   y,
					   w-ScrollCanvas::scrollbarwidth,
					   ScrollCanvas::scrollbarwidth,"");
  fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fl_set_object_resize(obj, FL_RESIZE_X);
  fl_set_object_callback(obj,ScrollCanvas::horscrollcallback,(long) fdui);
  fl_set_slider_return(obj, FL_RETURN_CHANGED);
  fdui->verscrollbar = obj = fl_add_slider(FL_VERT_SLIDER,
					   x,
					   y+ScrollCanvas::scrollbarwidth,
					   ScrollCanvas::scrollbarwidth,
					   h-ScrollCanvas::scrollbarwidth,"");
  fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
  fl_set_object_resize(obj, FL_RESIZE_Y);
  fl_set_object_callback(obj,ScrollCanvas::verscrollcallback,(long) fdui);
  fl_set_slider_return(obj, FL_RETURN_CHANGED);
  fdui->zoombutton = obj = fl_add_button(FL_NORMAL_BUTTON,
					 x,
					 y,
					 ScrollCanvas::scrollbarwidth,
					 ScrollCanvas::scrollbarwidth,"z");
  fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fl_set_object_callback(obj,ScrollCanvas::zoomcallback,(long) fdui);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

ScrollCanvas::ScrollCanvas(FL_FORM *form,
			   int x, int y, int w, int h,
			   void (*scb)(const MeshCoord&, const MeshCoord&,
				       void *),
			   void *scbptr)
  : backdropvisible(1),
    xscroll(0), yscroll(0),
    xscrollmin(0), xscrollmax(w-scrollbarwidth),
    yscrollmin(0), yscrollmax(h-scrollbarwidth),
    pmap(w-scrollbarwidth, h-scrollbarwidth),
    xmin(0), xmax(1), ymin(0), ymax(1),
    zoomfactor(1.5),
    usestipple(0),
    index(Ncanvases),
    holding(0),
    scrollcallback(scb), scrollcallbackptr(scbptr),
    xscrollcallback(0), yscrollcallback(0), xyscrollcallbackptr(0),
    zoombutton_callback(0), zoombutton_callbackptr(0),
    resize_callback(0), resize_callbackptr(0)
{
  index++;

  fl_addto_form(form);
  create_form_scrollcanvas(this, x, y, w, h);
  canvas = fl_add_canvas(FL_NORMAL_CANVAS,
			backdrop->x, backdrop->y, backdrop->w, backdrop->h, "");
  fl_set_object_gravity(canvas, FL_NorthWest, FL_SouthEast);
  fl_set_canvas_decoration(canvas, FL_NO_FRAME);

  fl_add_canvas_handler(canvas, Expose, exposecallback, this);
  fl_add_canvas_handler(canvas, ButtonPress, mousedncallback, this);
  fl_add_canvas_handler(canvas, ButtonRelease, mouseupcallback, this);
  fl_add_canvas_handler(canvas, MotionNotify, mousemvcallback, this);
  fl_end_form();

  fl_set_canvas_visual(canvas, gfxinfo.visual());
  fl_set_canvas_depth(canvas, gfxinfo.depth());

  fillgc = XCreateGC(gfxinfo.display(), gfxinfo.window(), 0, 0);
  XSetForeground(gfxinfo.display(), fillgc,
		 BlackPixel(gfxinfo.display(), gfxinfo.screen()));

  canvasw = canvas->w;
  canvash = canvas->h;
  setscrollbars();
}

ScrollCanvas::~ScrollCanvas() {
  fl_hide_object(canvas);
  fl_hide_object(horscrollbar);
  fl_hide_object(verscrollbar);
  fl_hide_object(zoombutton);
  fl_hide_object(backdrop);

  fl_delete_object(canvas);
  fl_delete_object(horscrollbar);
  fl_delete_object(verscrollbar);
  fl_delete_object(zoombutton);
  fl_delete_object(backdrop);

  fl_free_object(canvas);
  fl_free_object(horscrollbar);
  fl_free_object(verscrollbar);
  fl_free_object(zoombutton);
  fl_free_object(backdrop);
}

void ScrollCanvas::set_colormap(Colormap cmap) {
  colormap = cmap;
  fl_share_canvas_colormap(canvas, cmap);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ScrollCanvas::show() {
  fl_show_object(horscrollbar);
  fl_show_object(verscrollbar);
  fl_show_object(zoombutton);
  if(backdropvisible)
    fl_show_object(backdrop);
  else
    fl_show_object(canvas);
}

void ScrollCanvas::hide() {
  fl_hide_object(horscrollbar);
  fl_hide_object(verscrollbar);
  fl_hide_object(zoombutton);
  if(backdropvisible)
    fl_hide_object(backdrop);
  else
    fl_hide_object(canvas);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// copy pixmap to screen

static void copylimits(int scroll, int psize, int csize,
		       int &source, int &dest, int &copy)
{
  source = scroll;
  if(source >= 0) {	  // left edge of pixmap is beyond left edge of canvas
    dest = 0;
    copy = psize - source;
    if(copy > csize)      // right edge of pixmap is beyond right edge of canvas
      copy = csize;
  }
  else {			// left edge of pixmap is within canvas
    dest = -source;
    source = 0;
    copy = csize - dest;
    if(copy > psize)		// right edge of pixmap is within canvas
      copy = psize;
  }
}

void ScrollCanvas::refresh() {
  backdropvisible = 0;
  copyimage(fl_get_canvas_id(canvas));
}

void ScrollCanvas::copyimage(Drawable destination) const {
  int xsource, xdest, xcopy;
  int ysource, ydest, ycopy;

  // Find how much of the pixmap is visible.
  copylimits(xscroll, pmap.width(), canvas->w,
	     xsource, xdest, xcopy);
  copylimits(yscroll, pmap.height(), canvas->h,
	     ysource, ydest, ycopy);

  // Copy visible region of pixmap to canvas.
  XCopyArea(gfxinfo.display(), pixmap(), destination, gc(),
	    xsource, ysource,
	    xcopy, ycopy,
	    xdest, ydest);

  // If pixmap didn't fill canvas, fill in the rest.
  if(xdest > 0 || xdest + xcopy < canvas->w ||
     ydest > 0 || ydest + ycopy < canvas->h) {

    if(usestipple) {
      // find origin for stipple pattern. It has to be coincident with
      // the origin of the pixmap, so that the pixmap background and
      // the canvas background merge.
      //int xorigin = xdest > 0 ? xdest : xscrollmin - xscroll;
      //int yorigin = ydest > 0 ? ydest : yscrollmin - yscroll;
      XSetTSOrigin(gfxinfo.display(), fillgc, -xscroll, -yscroll);
    }

    if(xdest > 0)
      XFillRectangle(gfxinfo.display(), destination, fillgc,
		     0, 0, xdest, canvas->h);
    if(ydest > 0)
      XFillRectangle(gfxinfo.display(), destination, fillgc,
		     0, 0, canvas->w, ydest);
    if(xdest + xcopy < canvas->w)
      XFillRectangle(gfxinfo.display(), destination, fillgc,
		     xdest+xcopy, 0, canvas->w-xdest-xcopy, canvas->h);
    if(ydest + ycopy < canvas->h)
      XFillRectangle(gfxinfo.display(), destination, fillgc,
		     0, ydest+ycopy, canvas->w, canvas->h-ydest-ycopy);

  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Scrolling

static void setscrolllimits(int scroll, int pw, int cw,
			    int &scrollmin, int &scrollmax)
{
  if(scroll < 0)
    scrollmin = scroll;
  else
    scrollmin = 0;
  if(scroll + cw > pw)
    scrollmax = scroll + cw;
  else
    scrollmax = pw;
}

void ScrollCanvas::setscrollbars() {
  fl_set_slider_bounds(horscrollbar, xscrollmin, xscrollmax-canvas->w);
  fl_set_slider_size(horscrollbar,
		     canvas->w/(double) (xscrollmax - xscrollmin));
  fl_set_slider_value(horscrollbar, xscroll);
  
  fl_set_slider_bounds(verscrollbar, yscrollmin, yscrollmax-canvas->h);
  fl_set_slider_size(verscrollbar,
		     canvas->h/(double) (yscrollmax - yscrollmin));
  fl_set_slider_value(verscrollbar, yscroll);
}

void ScrollCanvas::save_scroll(ostream &os) const {
  os << xscrollmin << " " << xscroll << " " << xscrollmax << " "
     << yscrollmin << " " << yscroll << " " << yscrollmax << " "
     << pmap.width() << " " << pmap.height() << endl;
//    cerr << "ScrollCanvas::save_scroll w=" << pmap.width() 
//         << " h=" << pmap.height() << endl
//         << xscrollmin << " " << xscroll << " " << xscrollmax << " "
//         << yscrollmin << " " << yscroll << " " << yscrollmax << endl;
}

void ScrollCanvas::read_scroll(istream &is) {
  int w, h;
  is >> xscrollmin >> xscroll >> xscrollmax
     >> yscrollmin >> yscroll >> yscrollmax >> w >> h;
//    cerr << "ScrollCanvas::read_scroll w=" << pmap.width() 
//         << " h=" << pmap.height() << endl
//         << xscrollmin << " " << xscroll << " " << xscrollmax << " "
//         << yscrollmin << " " << yscroll << " " << yscrollmax << endl;
  pmap.resize(w, h);
  setscrolllimits(xscroll, pmap.width(), canvas->w, xscrollmin, xscrollmax);
  setscrolllimits(yscroll, pmap.height(), canvas->h, yscrollmin, yscrollmax);
  setscrollbars();
  do_scrollcallback();
//    if(xscrollcallback)
//      (*xscrollcallback)(xscroll - oldscroll, xyscrollcallbackptr);
//    if(yscrollcallback)
//      (*yscrollcallback)(yscroll - oldscroll, xyscrollcallbackptr);
  refresh();
}

void ScrollCanvas::horscrollcallback(FL_OBJECT*, long arg) {
  ScrollCanvas *sc = (ScrollCanvas*) arg;
  sc->horscrollhandler();
}

void ScrollCanvas::horscrollhandler() {
  int oldscroll = xscroll;
  xscroll = int(fl_get_slider_value(horscrollbar));
  // adjust size & position of scroll bars here in case the image had
  // been slid off the screen (with scroll(int, int)). In that case,
  // the scroll bars make the unphysical region accessible. Adjusting
  // the bars here makes unphysical regions that are scrolled off
  // screen inaccessible.
  setscrolllimits(xscroll, pmap.width(), canvas->w, xscrollmin, xscrollmax);
  setscrollbars();
  do_scrollcallback();
  if(xscrollcallback)
    (*xscrollcallback)(xscroll - oldscroll, xyscrollcallbackptr);
  refresh();
}

void ScrollCanvas::verscrollcallback(FL_OBJECT*, long arg) {
  ScrollCanvas *sc = (ScrollCanvas*) arg;
  sc->verscrollhandler();
}

void ScrollCanvas::verscrollhandler() {
  int oldscroll = yscroll;
  yscroll = int(fl_get_slider_value(verscrollbar));
  setscrolllimits(yscroll, pmap.height(), canvas->h, yscrollmin, yscrollmax);
  setscrollbars();
  do_scrollcallback();
  if(yscrollcallback)
    (*yscrollcallback)(yscroll - oldscroll, xyscrollcallbackptr);
  refresh();
}

void ScrollCanvas::scroll(int x, int y) {
  xscroll += x;
  yscroll += y;
  setscrolllimits(xscroll, pmap.width(), canvas->w, xscrollmin, xscrollmax);
  setscrolllimits(yscroll, pmap.height(), canvas->h, yscrollmin, yscrollmax);
  setscrollbars();
  refresh();
  do_scrollcallback();
}

void ScrollCanvas::scroll(double x, double y) {
  XPoint xp(xpoint(x, y));
  xscroll += xp.x;
  yscroll += xp.y;
  setscrolllimits(xscroll, pmap.width(), canvas->w, xscrollmin, xscrollmax);
  setscrolllimits(yscroll, pmap.height(), canvas->h, yscrollmin, yscrollmax);
  setscrollbars();
  refresh();
  do_scrollcallback();
}

XPoint ScrollCanvas::get_scroll_pixels() const {
  XPoint sc;
  sc.x = xscroll;
  sc.y = yscroll;
  return sc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ScrollCanvas::set_xyscrollcallbacks(void (*fx)(int, void*),
					 void (*fy)(int, void*),
					 void *ptr)
{
  xscrollcallback = fx;
  yscrollcallback = fy;
  xyscrollcallbackptr = ptr;
}

void ScrollCanvas::set_zoombutton_callback(void (*f)(int, void*), void *ptr) {
  zoombutton_callback = f;
  zoombutton_callbackptr = ptr;
}

void ScrollCanvas::set_resize_callback(void (*f)(int, int, void*), void *ptr) {
  resize_callback = f;
  resize_callbackptr = ptr;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Zooming

void ScrollCanvas::zoomcallback(FL_OBJECT*, long arg) {
  ((ScrollCanvas*) arg)->buttonzoom();
}

void ScrollCanvas::buttonzoom() {
  int buttonnumber = fl_get_button_numb(zoombutton);
  zoom_button(buttonnumber);
  if(zoombutton_callback)
    (*zoombutton_callback)(buttonnumber, zoombutton_callbackptr);
}

void ScrollCanvas::zoom_button(int buttonnumber) {
  if(buttonnumber == 1)
    zoom(1./zoomfactor);
  else if(buttonnumber == 2)
    fullzoom();
  else if(buttonnumber == 3)
    zoom(zoomfactor);
}

void ScrollCanvas::zoom(double factor) {
  if(holding) return;
  XPoint center;
  center.x = int(xscroll + 0.5*canvas->w);
  center.y = int(yscroll + 0.5*canvas->h);
  zoom(factor, center);
}

void ScrollCanvas::zoom(double factor, const XPoint &fixedpoint) {
                      // fixedpoint is given in pixmap coordinates
  if(holding) return;
  int w = int(pmap.width()*factor);
  int h = int(pmap.height()*factor);
  if(h == 0 || w == 0) {
    cerr << "Can't zoom down that far!" << endl;
    return;
  }
  pmap.resize(w, h);

  xscroll += int(fixedpoint.x*(factor - 1.0));
  yscroll += int(fixedpoint.y*(factor - 1.0));

  setscrolllimits(xscroll, pmap.width(), canvas->w, xscrollmin, xscrollmax);
  setscrolllimits(yscroll, pmap.height(), canvas->h, yscrollmin, yscrollmax);
  setscrollbars();
  redraw();
  refresh();
  do_scrollcallback();
}

void ScrollCanvas::fullzoom() {
  if(holding) return;
  int pwch = pmap.width()*canvas->h;
  int phcw = pmap.height()*canvas->w;
  int neww = canvas->w;
  int newh = canvas->h;
  if(pwch != phcw) {
    if(pwch > phcw) // h/w for canvas is greater than for pixmap
      newh = int(neww*pmap.height()/(double) pmap.width());
    else           // h/w for pixmap is greater than for canvas
      neww = int(newh*pmap.width()/(double) pmap.height());
  }
  pmap.resize(neww, newh);
  xscroll = 0;
  yscroll = 0;
  xscrollmin = 0;
  yscrollmin = 0;
  xscrollmax = canvas->w;
  yscrollmax = canvas->h;
  setscrollbars();
  redraw();
  refresh();
  do_scrollcallback();
}

void ScrollCanvas::do_scrollcallback() const {
  if(scrollcallback) {
    MeshCoord ul, lr; // upper left and lower right corners of canvas
    physicalcoords(xscroll, yscroll, ul.x, ul.y);
    physicalcoords(xscroll+canvasw, yscroll+canvash, lr.x, lr.y);
    (*scrollcallback)(ul, lr, scrollcallbackptr);
  }
}

Rectangle ScrollCanvas::visible_region() const {
  MeshCoord ul, lr;
  physicalcoords(xscroll, yscroll, ul.x, ul.y);
  physicalcoords(xscroll+canvasw, yscroll+canvash, lr.x, lr.y);
  return Rectangle(ul, lr);
}


//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ScrollCanvas::hold() {
  holding = 1;
  fl_deactivate_object(zoombutton);
  fl_deactivate_object(verscrollbar);
  fl_deactivate_object(horscrollbar);
  fl_set_object_lcol(zoombutton, FL_INACTIVE_COL);
  fl_set_object_color(horscrollbar, COL_BACKGROUND, COL_BACKGROUND);
  fl_set_object_color(verscrollbar, COL_BACKGROUND, COL_BACKGROUND);
}

void ScrollCanvas::release_hold() {
  holding = 0;
  fl_activate_object(zoombutton);
  fl_activate_object(verscrollbar);
  fl_activate_object(horscrollbar);
  fl_set_object_lcol(zoombutton, FL_BLACK);
  fl_set_object_color(horscrollbar, FL_COL1, FL_COL1);
  fl_set_object_color(verscrollbar, FL_COL1, FL_COL1);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ScrollCanvas::resizepixmap(int w, int h) {
  int oldw = pmap.width();
  int oldh = pmap.height();
  pmap.resize(w, h);
  xscroll = int(xscroll*w/(double) oldw);
  yscroll = int(yscroll*h/(double) oldh);
  setscrolllimits(xscroll, pmap.width(), canvas->w, xscrollmin, xscrollmax);
  setscrolllimits(yscroll, pmap.height(), canvas->h, yscrollmin, yscrollmax);
  setscrollbars();
  do_scrollcallback();
}

void ScrollCanvas::setcoords_resize(double x0, double x1, double y0, double y1)
{
  // set the physical coordinates and resize the pixmap so that the
  // range of physical coordinates visible in the canvas doesn't
  // change. Write down the equations for the physical coords of the
  // canvas limits and solve them for xscroll, yscroll, pixmap-width,
  // and pixmap-height.
  double oldw = xmax - xmin;
  double oldh = ymax - ymin;
  double xfactor = (x1 - x0)/oldw;
  double yfactor = (y1 - y0)/oldh;
  xscroll += int(pmap.width()*(xmin - x0)/oldw);
  yscroll += int(pmap.height()*(yfactor-1-(ymin-y0)/oldh));
  setcoords(x0, x1, y0, y1);
  pmap.resize(pmap.width()*xfactor, pmap.height()*yfactor);
  setscrolllimits(xscroll, pmap.width(), canvas->w, xscrollmin, xscrollmax);
  setscrolllimits(yscroll, pmap.height(), canvas->h, yscrollmin, yscrollmax);
  setscrollbars();
  do_scrollcallback();
  redraw();
  refresh();
}

void ScrollCanvas::setcoords_reset(double x0, double x1, double y0, double y1)
{
  // set the physical coordinates and resize the pixmap so that it
  // fits the canvas.
  pmap.resize(canvasw, canvash);
  setcoords(x0, x1, y0, y1);
  fullzoom();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


// convert to pixmap coordinates

XPoint ScrollCanvas::xpoint(double x, double y) const {
  XPoint p;
  // order of multiplication and division is important to avoid round off error
  p.x = short((x - xmin)*pmap.width()/(xmax - xmin));
  p.y = short(pmap.height() - ((y - ymin)*pmap.height()/(ymax - ymin)));
  return p;
}

XPoint ScrollCanvas::xpoint(const MV_Vector_double &xy) const {
  return xpoint(xy[0], xy[1]);
}

void ScrollCanvas::physicalcoords(int ix, int iy, double &x, double &y) const {
  x = xmin + (xmax - xmin)*ix/pmap.width();
  y = ymin + (ymax - ymin)*(1.0 - iy/(double) pmap.height());
}

void ScrollCanvas::setcoords(double x0, double x1, double y0, double y1) {
  xmin = x0;
  xmax = x1;
  ymin = y0;
  ymax = y1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int ScrollCanvas::exposecallback(FL_OBJECT*, Window win, int w, int h,
			     XEvent*, void *arg)
{
  ((ScrollCanvas*) arg)->exposehandler(win, w, h);
  return 0;
}

void ScrollCanvas::exposehandler(Window, int w, int h) {
  if(w != canvasw || h != canvash) {  // size has changed
    canvasw = w;
    canvash = h;
    if(canvasw + xscroll > pmap.width())
      xscrollmax = canvasw + xscroll;
    else
      xscrollmax = pmap.width();
    if(canvash + yscroll > pmap.height())
      yscrollmax = canvash + yscroll;
    else
      yscrollmax = pmap.height();
    setscrollbars();
    if(resize_callback)
      (*resize_callback)(w, h, resize_callbackptr);
    do_scrollcallback();
  }
  refresh();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ScrollCanvas::clearcanvas() {
  if(!backdropvisible) {
    XSetTSOrigin(gfxinfo.display(), fillgc, -xscroll, -yscroll);
    XFillRectangle(gfxinfo.display(), fl_get_canvas_id(canvas), fillgc,
		   0, 0, canvas->w, canvas->h);
  }
}

void ScrollCanvas::clear() {
  if(usestipple)
    XSetTSOrigin(gfxinfo.display(), fillgc, 0, 0);
  pmap.clear(fillgc);
  refresh();
}

void ScrollCanvas::clearpixmap() {
  if(usestipple)
    XSetTSOrigin(gfxinfo.display(), fillgc, 0, 0);
  pmap.clear(fillgc);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Filling in the undrawn regions

void ScrollCanvas::setfillcolor(unsigned long fillcolor) {
  usestipple = 0;
  XSetFillStyle(gfxinfo.display(), fillgc, FillSolid);
  XSetForeground(gfxinfo.display(), fillgc, fillcolor);
}

void ScrollCanvas::setstipple(int w, int h, char *bits,
			      unsigned long fg, unsigned long bg)
{
  usestipple = 1;
  stipple_w = w;
  stipple_h = h;
  stipple = XCreateBitmapFromData(gfxinfo.display(), gfxinfo.window(), bits,
					w, h);
  XSetForeground(gfxinfo.display(), fillgc, fg);
  XSetBackground(gfxinfo.display(), fillgc, bg);
  XSetFillStyle(gfxinfo.display(), fillgc, FillOpaqueStippled);
  XSetStipple(gfxinfo.display(), fillgc, stipple);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Mouse

MouseClick ScrollCanvas::lastclick;     // remember last mouse down

int ScrollCanvas::checkclick(int x, int y) const {
  // make sure click is on the image. Return 1 if it's NOT.
  return (x < 0 || y < 0 || x >= pmap.width() || y >= pmap.height());
}

int ScrollCanvas::mousedncallback(FL_OBJECT*, Window, int, int,
				   XEvent *xev, void *arg)
{
  ((ScrollCanvas*) arg)->mousedncb(xev);
  return 0;
}

void ScrollCanvas::mousedncb(XEvent *xev) {
  double x, y;
  int ix = xev->xbutton.x + xscroll;
  int iy = xev->xbutton.y + yscroll;
  //if(checkclick(ix, iy)) return;
  physicalcoords(ix, iy, x, y);
  lastclick = MouseClick(x, y, ix, iy, index, xev);
  mouse(lastclick);  // virtual function
}

const MouseClick &ScrollCanvas::downclick() const {
  return lastclick;
}

// ----------------------------------

int ScrollCanvas::mouseupcallback(FL_OBJECT*, Window, int, int,
				   XEvent *xev, void *arg)
{
  ((ScrollCanvas*) arg)->mouseupcb(xev);
  return 0;
}

void ScrollCanvas::mouseupcb(XEvent *xev) {
  double x, y;
  if(!lastclick) return;
  if(lastclick.window == index) {    // same window as last mouse down?
    int ix = xev->xbutton.x + xscroll;
    int iy = xev->xbutton.y + yscroll;
    //if(checkclick(ix, iy)) return;
    physicalcoords(ix, iy, x, y);
    mouse(MouseClick(x, y, ix, iy, index, xev));  // virtual function
  }
  lastclick.event = MOUSE_TRAP; // last click no longer meaningful
}

// -----------------------------------

int ScrollCanvas::mousemvcallback(FL_OBJECT*, Window, int, int,
				   XEvent *xev, void *arg)
{
  ((ScrollCanvas*) arg)->mousemvcb(xev);
  return 0;
}

void ScrollCanvas::mousemvcb(XEvent *xev) {
  double x, y;
  if(!lastclick || lastclick.window != index) return; // wrong window!
  int ix = xev->xbutton.x + xscroll;
  int iy = xev->xbutton.y + yscroll;
  //if(checkclick(ix, iy)) return;
  physicalcoords(ix, iy, x, y);
  mouse(MouseClick(x, y, ix, iy, index, xev));  // virtual function
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// simulate a mouse click

void ScrollCanvas::robomouse(const MouseClick &click) {
  if(click.event == MOUSE_DN)
    lastclick = click;
  mouse(click);
  if(click.event == MOUSE_UP)
    lastclick.event = MOUSE_TRAP;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static inline void writebyte(FILE* file, unsigned char c) {
  writebinary(file, c, 1);
}

class ColorList {
private:
  Display *display;
  Colormap colormap;
  Vec<XColor> list;
public:
  ColorList(Display *d, Colormap cmap)
    : display(d), colormap(cmap), list(0, BlockSize(100))
  {}
  const XColor &operator[](unsigned long);
};

const XColor &ColorList::operator[](unsigned long pixel) {
  // look for color in list
  for(int i=0; i<list.capacity(); i++) {
    if(list[i].pixel == pixel)
      return list[i];
  }
  // didn't find color, have to look it up
  int which = list.grow(1);
  XColor &color = list[which];
  color.pixel = pixel;
  XQueryColor(display, colormap, &color);
  return color;
}

void ScrollCanvas::ppmfile(FILE *file) const {
  // We can't use XGetImage directly on the canvas, because some of
  // the window might be obscured. We can't do it on the pixmap, since
  // the whole pixmap might not be displayed, or it might not fill the
  // canvas. So we have to construct another pixmap which is what the
  // canvas would look like if it were all visible, and then use
  // XGetImage on that.
  PixMap temp(canvas->w, canvas->h);
  copyimage(temp.pixmap());
  unsigned long plane_mask = ~(unsigned long) 0; // all planes
  XImage *image = XGetImage(gfxinfo.display(), temp.pixmap(),
			    0, 0, canvas->w, canvas->h, plane_mask, ZPixmap);

  // header for PPM file
  fputs("P6\n", file);  /* P3 for ascii version */
  fputs("# CREATOR: oof\n", file);
  fprintf(file, "%d %d\n", canvas->w, canvas->h);
  fprintf(file, "255\n");  /* 65535 for ascii version? */

  ColorList colorlist(gfxinfo.display(), colormap);

  for(int i=0; i<image->height; i++) {
    for(int j=0; j<image->width; j++) {
      XColor color = colorlist[XGetPixel(image, j, i)]; // look up rgb
      writebyte(file, color.red>>8);
      writebyte(file, color.green>>8);
      writebyte(file, color.blue>>8);
    }
  }
  
  XDestroyImage(image);
}
