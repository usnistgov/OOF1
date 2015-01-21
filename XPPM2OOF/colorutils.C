// -*- C++ -*-
// $RCSfile: colorutils.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-30 19:23:23 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream.h>
#include "stdlib.h"

#include "colorutils.h"
#include "forms.h"
#include "menucolor.h"
#include "varobjects.h"

static int highbit(unsigned long);

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void Graphics::setup() {

  the_display = fl_get_display();
  the_screen = DefaultScreen(the_display);

  // Find the default visual
  Visual *default_visual = DefaultVisual(the_display, the_screen);
  XVisualInfo vinfo_template;
  int nvis;
  XVisualInfo *vinfo = XGetVisualInfo(the_display, VisualNoMask,
				      &vinfo_template, &nvis);
  visinfo = 0;
  for(int i=0; i<nvis; i++)
    if(vinfo[i].visual == default_visual) {
      visinfo = &vinfo[i];
      break;
    }
  // XFree(vinfo);  // Don't free vinfo! visinfo points to it.
  if(!visinfo) {
    cerr << "Can't find X visual info!" << endl;
    exit(1);
  }
  cerr << "Using " << visualname(visinfo->c_class) << " visual of depth "
       << visinfo->depth << "." << endl;
  if(visinfo->c_class == PseudoColor)
    cerr << "Colormap size = " << visinfo->colormap_size << "." << endl;

  the_window = XCreateSimpleWindow(the_display,
				   RootWindow(the_display, the_screen),
				   0, 0,     // x, y
				   10, 10,   // size
				   0,
				   0,
				   WhitePixel(the_display, the_screen));

  if(visinfo->c_class == TrueColor) {
    the_red_shift = 15 - highbit(visinfo->red_mask);
    the_green_shift = 15 - highbit(visinfo->green_mask);
    the_blue_shift = 15 - highbit(visinfo->blue_mask);
  }
}

// stolen from xv source code
static int highbit(unsigned long x) {
  unsigned long hb = 0x80000000UL;
  int i;
  for(i=31; ((x & hb) == 0) && i>=0; i--, x<<=1);
  return i;
}

Colormap Graphics::colormap() const {
  Colormap map;
  if(visinfo->c_class == TrueColor) {
    map = DefaultColormap(the_display, the_screen);
  }
  else if(visinfo->c_class == PseudoColor) {
    map = XCreateColormap(the_display, the_window, visinfo->visual, AllocAll);
  }
  else {
    cerr << "Graphics::colormap --- unsupported X visual." << endl;
    exit(1);
  }
  return map;
}

void Graphics::freecolormap(Colormap map) const {
  if(map != DefaultColormap(the_display, the_screen))
    XFreeColormap(the_display, map);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

const char *visualname(int c_class) {
  switch(c_class) {
  case TrueColor:
    return "TrueColor";
  case StaticColor:
    return "StaticColor";
  case StaticGray:
    return "StaticGray";
  case DirectColor:
    return "DirectColor";
  case PseudoColor:
    return "PseudoColor";
  case GrayScale:
    return "GrayScale";
  }
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// XForms stuff for putting a Color into a menu Variable or Argument

class ColorVarObject : public VarObject {
private:
  FL_OBJECT *colorbutton;
  FL_OBJECT *okbutton;
  FL_OBJECT *inputarea;
  FL_COLOR colorindex;
  void set();
  void browse();
  Color pendingcolor;
  void paint();
  static void CB_okbutton(FL_OBJECT*, long);
  static void CB_Browsebutton(FL_OBJECT*, long);
  static void CB_Colorbutton(FL_OBJECT*, long);
  static void CB_input(FL_OBJECT*, long);
public:
  ColorVarObject(Variable *var, Form *form, VarObjectType type,
		    int x, int y, int w, int h);
  ~ColorVarObject();
  virtual void show();
  virtual void hide();
  virtual CharString get();
  //friend void CB_okbutton(FL_OBJECT*, long);
  //friend void CB_Colorbrowse(FL_OBJECT*, long);
};

ColorVarObject::ColorVarObject(Variable *v, Form *mf, VarObjectType t,
		    int x, int y, int w, int h)
  : VarObject(v, mf, t),
    colorindex(alloc_fl_color()),
    pendingcolor(*(Color*) var->address)
{
  const int colorwidth = (type == VO_PROACTIVE? 70 : 30);
  const int okwidth = (type == VO_PROACTIVE? 65 : 0); // only used if proactive
  const int inputwidth = w/2;
  CharString label(var->name + " =");

  fl_bgn_group();
  colorbutton = fl_add_button(FL_NORMAL_BUTTON,
			      x+w-okwidth-colorwidth,
			      y,
			      colorwidth, h,
			      "");
  fl_set_object_resize(colorbutton, FL_RESIZE_NONE);
  fl_mapcolor(colorindex, pendingcolor.query_red(), pendingcolor.query_green(),
	      pendingcolor.query_blue());
  fl_set_object_color(colorbutton, colorindex, FL_BLACK);
  fl_set_object_lstyle(colorbutton, FL_BOLD_STYLE);
  fl_set_object_callback(colorbutton, CB_Colorbutton, (long) this);
  if(pendingcolor.brightness() < 128)
    fl_set_object_lcol(colorbutton, FL_WHITE);
  else
    fl_set_object_lcol(colorbutton, FL_BLACK);

  if(type == VO_PROACTIVE) {
    okbutton = fl_add_button(FL_RETURN_BUTTON,
			     x+w-okwidth, y, okwidth, h, "Set");
    fl_set_object_lstyle(okbutton, FL_TIMESBOLD_STYLE);
    fl_set_object_lsize(okbutton, 11);
    fl_set_object_resize(okbutton, FL_RESIZE_NONE);
    fl_set_object_callback(okbutton, CB_okbutton, (long) this);
    fl_set_object_color(okbutton, COL_INPUT_BUTTON_UP, COL_INPUT_BUTTON_DOWN);
    fl_set_object_label(colorbutton, "Browse");
  }
  else
    fl_set_object_label(colorbutton, "?");

  inputarea = fl_add_input(FL_NORMAL_INPUT,
			   x+w-okwidth-colorwidth-inputwidth, y,
			   inputwidth, h, label);
  fl_set_object_lsize(inputarea, 11);
  fl_set_object_resize(inputarea, FL_RESIZE_X);
  fl_set_object_callback(inputarea, CB_input, (long) this);
  fl_set_object_color(inputarea, COL_INPUT_UNSELECTED, COL_INPUT_SELECTED);

  if(type == VO_PROACTIVE) { // appearing in main menu form
    fl_set_object_gravity(okbutton, FL_SouthEast, FL_SouthEast);
    fl_set_object_gravity(colorbutton, FL_SouthEast, FL_SouthEast);
    fl_set_object_gravity(inputarea, FL_SouthWest, FL_SouthEast);
  }
  else { // appearing in command form
    fl_set_object_gravity(colorbutton, FL_NorthEast, FL_NorthEast);
    fl_set_object_gravity(inputarea, FL_NorthWest, FL_NorthEast);
  }
  fl_end_group();
}

ColorVarObject::~ColorVarObject() {
  free_fl_color(colorindex);
}

void ColorVarObject::paint() {
  fl_mapcolor(colorindex, pendingcolor.query_red(), pendingcolor.query_green(),
	      pendingcolor.query_blue());
  if(pendingcolor.brightness() < 128)
    fl_set_object_lcol(colorbutton, FL_WHITE);
  else
    fl_set_object_lcol(colorbutton, FL_BLACK);

  fl_redraw_object(colorbutton);
}

void ColorVarObject::hide() {
  fl_hide_object(inputarea);
  fl_hide_object(colorbutton);
  if(type == VO_PROACTIVE)
    fl_hide_object(okbutton);
}

void ColorVarObject::show() {
  fl_set_input(inputarea, var->tostring());
  if(type == VO_PROACTIVE) {
    fl_set_input_selected(inputarea, 1);
    fl_show_object(okbutton);
  }
  fl_show_object(inputarea);
  fl_show_object(colorbutton);
}

CharString ColorVarObject::get() {
  return forminputstring(inputarea);
}

void ColorVarObject::CB_okbutton(FL_OBJECT*, long ptr) {
  ((ColorVarObject *) ptr)->set();
}

void ColorVarObject::set() {
  if(type == VO_PROACTIVE)
    form->setvariable(var, get());
  pendingcolor = *(Color*) var->address;
  paint();
}

void ColorVarObject::CB_Colorbutton(FL_OBJECT *, long ptr) {
  ((ColorVarObject*) ptr)->browse();
}

void ColorVarObject::browse() {
  pendingcolor = colorbrowser(pendingcolor);
  fl_set_input(inputarea, to_charstring(pendingcolor));
  paint();
}

void ColorVarObject::CB_input(FL_OBJECT*, long) {} // does nothing!

// This is called by the menu routines to create the GUI for the Color.

template<>
VarObject *Var<Color>::create_varobject(Variable *var, Form *form,
					   VarObjectType type,
					   int x, int y, int w, int h)
{
  return new ColorVarObject(var, form, type, x, y, w, h);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Form for selecting a color

ColorBrowser::ColorBrowser(const Color &c)
  : Form("Color Browser"),
    color(c),
    oldcolor(c),
    done(0),
    newcolor_index(alloc_fl_color()),
    oldcolor_index(alloc_fl_color())
{
  fdui = create_form_colorbrowser();
  fdui->vdata = this;
  the_form = fdui->colorbrowser;
  fl_set_object_color(fdui->background, COL_BACKGROUND, 0);
  setupslider(fdui->slider_red);
  setupslider(fdui->slider_grn);
  setupslider(fdui->slider_blu);
  fl_set_slider_value(fdui->slider_red, color.query_red());
  fl_set_slider_value(fdui->slider_grn, color.query_green());
  fl_set_slider_value(fdui->slider_blu, color.query_blue());

  fl_mapcolor(newcolor_index,
	      color.query_red(), color.query_green(), color.query_blue());
  fl_mapcolor(oldcolor_index,
	      color.query_red(), color.query_green(), color.query_blue());

  fl_set_object_color(fdui->box_new, newcolor_index, 0);
  fl_set_object_color(fdui->box_old, oldcolor_index, 0);
}

ColorBrowser::~ColorBrowser() {
  free(fdui);
  free_fl_color(newcolor_index);
  free_fl_color(oldcolor_index);
}

void ColorBrowser::makevisible() {
  if(the_form->visible) return;
  fl_show_form(the_form, FL_PLACE_MOUSE, FL_TRANSIENT, name.charstar());
    Nforms()++;
}

void ColorBrowser::setupslider(FL_OBJECT *sl) {
  fl_set_slider_step(sl, 1.0);
  fl_set_slider_bounds(sl, 0.0, 255.0);
  fl_set_slider_precision(sl, 0);
}

void ColorBrowser::CB_slider(FL_OBJECT *obj, long) {
  ColorBrowser *cb = (ColorBrowser*)((FD_colorbrowser*)obj->form->fdui)->vdata; 
  cb->changecolor();
}

void ColorBrowser::changecolor() {
  int red = int(fl_get_slider_value(fdui->slider_red));
  int grn = int(fl_get_slider_value(fdui->slider_grn));
  int blu = int(fl_get_slider_value(fdui->slider_blu));
  color = Color(red, grn, blu);
  fl_mapcolor(newcolor_index, red, grn, blu);
  fl_redraw_object(fdui->box_new);
}

void ColorBrowser::CB_ok(FL_OBJECT *obj, long) {
  ColorBrowser *cb = (ColorBrowser*)((FD_colorbrowser*)obj->form->fdui)->vdata; 
  cb->done = 1;
}

void ColorBrowser::CB_cancel(FL_OBJECT *obj, long) {
  ColorBrowser *cb = (ColorBrowser*)((FD_colorbrowser*)obj->form->fdui)->vdata; 
  cb->cancel();
}

void ColorBrowser::cancel() {
  color = oldcolor;
  done = 1;
}

Color colorbrowser(const Color &oldcolor) {
  fl_deactivate_all_forms();
  ColorBrowser cb(oldcolor);
  cb.makevisible();
  do
    fl_check_forms();
  while(!cb.done);
  fl_activate_all_forms();
  return cb.color;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


// Keep track of which colors in the Forms colormap are being used.

static char flcolorlist[FL_MAX_COLS];

FL_COLOR alloc_fl_color() {
  for(int i=XMENU_FREE_COL; i<FL_MAX_COLS; i++) {
    if(!flcolorlist[i]) {
      flcolorlist[i] = 1;
      return i;
    }
  }
  return 0;
}

void free_fl_color(FL_COLOR i) {
  flcolorlist[i] = 0;
}
