// -*- C++ -*-
// $RCSfile: colorutils.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-13 20:02:45 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "menuform.h"
#include "forms.h"
#include "color.h"

#include "beginheader.h"

class Graphics {
private:
  XVisualInfo *visinfo;
  int the_screen;
  Display *the_display;
  Window the_window; // a window that can be used in XCreatePixmap calls, etc.
  int the_red_shift; // for constructing TrueColor pixel values
  int the_green_shift; // for constructing TrueColor pixel values
  int the_blue_shift; // for constructing TrueColor pixel values
public:
  Graphics() {}

  void setup();  // call this once after X has been initialized

  Colormap colormap() const;  // create a new colormap
  void freecolormap(Colormap) const;
  Window window() const { return the_window; }
  Display *display() const { return the_display; }
  int screen() const { return the_screen; }
  int c_class() const { return visinfo->c_class; }
  int depth() const { return visinfo->depth; }
  Visual *visual() const { return visinfo->visual; }
  int colormap_size() const { return visinfo->colormap_size; }
  unsigned long red_mask() const { return visinfo->red_mask; }
  unsigned long green_mask() const { return visinfo->green_mask; }
  unsigned long blue_mask() const { return visinfo->blue_mask; }
  int red_shift() const { return the_red_shift; }
  int green_shift() const { return the_green_shift; }
  int blue_shift() const { return the_blue_shift; }
};

const char *visualname(int c_class); // get printable X visual name

EXTERN Graphics gfxinfo;

Color colorbrowser(const Color&);  // gui for picking a color
FL_COLOR alloc_fl_color();         // allocate a color in the XForms colormap
void free_fl_color(FL_COLOR);

class ColorBrowser : public Form {
private:
  Color color;
  Color oldcolor;
  int done;

  FL_OBJECT *slider_red;
  FL_OBJECT *slider_grn;
  FL_OBJECT *slider_blu;
  FL_OBJECT *box_new;
  FL_OBJECT *box_old;
  FL_OBJECT *button_ok;
  FL_OBJECT *button_cancel;

  FL_COLOR newcolor_index;
  FL_COLOR oldcolor_index;

  static void CB_slider(FL_OBJECT*, long);
  void changecolor();

  static void CB_ok(FL_OBJECT*, long);
  static void CB_cancel(FL_OBJECT*, long);
  void cancel();

  static int CB_close(FL_FORM*, void*);

  ColorBrowser(const Color &c);
  ~ColorBrowser();
  virtual void makevisible();

  void setupslider(FL_OBJECT*);

  friend void create_form_colorbrowser(ColorBrowser*);
  friend Color colorbrowser(const Color&);
};

EXTERN Color wallpaper_foreground DFLT(Color(200, 200, 230));
EXTERN Color wallpaper_background DFLT(Color(228, 228, 255));

#include "endheader.h"

#endif
