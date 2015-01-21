// -*- C++ -*-
// $RCSfile: colordashboard.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// The ColorDashboard handles the interaction, but all the data is
// stored in the ColorDrawer.

#include "colordashboard.h"
#include "colordrawer.h"
#include "colorutils.h"
#include "create_form_drawer.h"
#include "delete_fl_group.h"
#include "formdrawer.h"
#include "oofcanvas.h"
#include "menuDef.h"

int ColorDashboard::colormapsize_dflt = 128;
int ColorDashboard::colormapnumber_dflt = 0;
TrueFalse ColorDashboard::fullscale_flag_dflt = 1;
TrueFalse ColorDashboard::flipcolors_dflt = 0;

Vec<ColorDashboard*> ColorDashboard::allcolordashboards;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Menu commands used to simulate mouse clicks

class ChangeColorCmd : public CommandM {
private:
  double whichbar;
  Color newcolor;

  ChangeColorCmd()
    : CommandM("newcol", "change color")
  {
    AddArgument(this, "bar", whichbar);
    AddArgument(this, "color", newcolor);
  }
  virtual void func() {
    ((ColorDashboard*) get_dashboard())->colorbar_change(whichbar, newcolor);
  }
  virtual CommandM *clone() const {
    ChangeColorCmd *ccc = new ChangeColorCmd;
    ccc->whichbar = whichbar;
    ccc->newcolor = newcolor;
    return ccc;
  }
  friend class ColorDashboard;
};

class RevertColorCmd : public CommandM {
private:
  double whichbar;

  RevertColorCmd()
    : CommandM("revertcol", "revert color")
  {
    AddArgument(this, "bar", whichbar);
  }
  virtual void func() {
    ((ColorDashboard*) get_dashboard())->colorbar_revert(whichbar);
  }
  virtual CommandM *clone() const {
    RevertColorCmd *rcc = new RevertColorCmd;
    rcc->whichbar = whichbar;
    return rcc;
  }
  friend class ColorDashboard;
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

ColorDashboard::ColorDashboard(ColorDrawer *drwr)
  : Dashboard("Color", drwr),
    colordrawer(drwr),
    colormapsize(colormapsize_dflt),
    colormapnumber(colormapnumber_dflt),
    fullscale_flag(fullscale_flag_dflt),
    flipcolors(flipcolors_dflt),
    colorbar_height(0), colorbar_width(0), // force recreation of pixmap
    colorbar_pixmap(1, 1)
{
  allcolordashboards.grow(1, this);
  create_form_color_control(this);

  fl_set_button(color_fullscale, fullscale_flag);
  if(fullscale_flag) {
    fl_hide_object(min_input);
    fl_hide_object(max_input);
  }

  ColorMapRegistration::sort();
  fl_set_choice_fontstyle(color_map_menu, FL_BOLD_STYLE);
  for(int i=0; i<colormapregistry().capacity(); i++)
    fl_addto_choice(color_map_menu, colormapregistry()[i]->name());
  fl_set_choice(color_map_menu, colormapnumber + 1);
  fl_set_input(colormapsize_input, to_charstring(colormapsize));
  fl_set_button(color_flip_button, flipcolors);

  fl_add_canvas_handler(colorbar, Expose, FDCB_colorbar_expose, this);
  fl_add_canvas_handler(colorbar, ButtonPress, FDCB_colorbar_mousedn, this);
  fl_add_canvas_handler(colorbar, ButtonRelease, FDCB_colorbar_mouseup, this);
  fl_set_canvas_visual(colorbar, gfxinfo.visual());
  fl_set_canvas_depth(colorbar, gfxinfo.depth());
  fl_share_canvas_colormap(colorbar, canvas()->get_colormap());
  colorbar_height = colorbar->h;
  colorbar_width = colorbar->w;
  colorbar_pixmap.resize(colorbar_width, colorbar_height);

  AddVariable(&menu, "max", "maximum value plotted", colordrawer->plotmax,
	      set_limits_cmd);
  AddVariable(&menu, "min", "minimum value plotted", colordrawer->plotmin,
	      set_limits_cmd);
  AddVariable(&menu, "fullscale", "fullscale?", fullscale_flag,
	      set_fullscale_cmd);
  AddVariable(&menu, "colormap", "colormap", colormapnumber,
	      set_colormap_cmd);
  AddVariable(&menu, "mapsize", "colormap size", colormapsize,
	      set_colormapsize_cmd);
  AddVariable(&menu, "flip", "flip colormap?", flipcolors,
	      flip_colormap_cmd);
  changecolorcmd = new ChangeColorCmd;
  revertcolorcmd = new RevertColorCmd;
  menu.AddCommand(changecolorcmd);
  menu.AddCommand(revertcolorcmd);
}

ColorDashboard::~ColorDashboard() {
  hide();
  delete_fl_group(color_control);
  /*
  fl_delete_group(color_control);
  fl_delete_object(max_input);
  fl_delete_object(min_input);
  fl_delete_object(color_fullscale);
  fl_delete_object(color_map_menu);
  fl_delete_object(colormapsize_input);
  fl_delete_object(color_flip_button);
  fl_delete_object(valmin_text);
  fl_delete_object(valmax_text);
  fl_delete_object(colorbar);

  fl_free_object(max_input);
  fl_free_object(min_input);
  fl_free_object(color_fullscale);
  fl_free_object(color_map_menu);
  fl_free_object(colormapsize_input);
  fl_free_object(color_flip_button);
  fl_free_object(valmin_text);
  fl_free_object(valmax_text);
  fl_free_object(colorbar);
  fl_free_object(color_control);
  */

  allcolordashboards.remove(this);
  menu.RemoveOption(*changecolorcmd); // also deletes
  menu.RemoveOption(*revertcolorcmd); // also deletes
}

ColorDashboard *ColorDashboard::getcolordashboard(FL_OBJECT *colorbar) {
  for(int i=0; i<allcolordashboards.capacity(); i++)
    if(allcolordashboards[i]->colorbar == colorbar)
      return allcolordashboards[i];
  return 0;
}

void ColorDashboard::show() {
  fl_show_object(color_control);
  draw_fullscale_objects();
}

void ColorDashboard::hide() {
  fl_hide_object(color_control);
}

void ColorDashboard::displaylimits() const {
  fl_set_object_label(valmin_text, "min = " +
		      to_charstring(colordrawer->valmin));
  fl_set_object_label(valmax_text, "max = " +
		      to_charstring(colordrawer->valmax));
  fl_set_input(max_input, to_charstring(colordrawer->plotmax));
  fl_set_input(min_input, to_charstring(colordrawer->plotmin));
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDashboard::set_limits_cmd(const CharString&) {
  ((ColorDashboard*) get_dashboard())->set_limits();
}

void ColorDashboard::set_limits() {
  formdrawer()->draw();
  displaylimits();
}

void ColorDashboard::set_color_max() {
  if(active()) {
    colordrawer->plotmax = atof(fl_get_input(max_input));
    garcon()->log(menucommand() + "max=" + to_charstring(colordrawer->plotmax));
    formdrawer()->draw();
  }
}

void FDCB_color_max(FL_OBJECT*, long ptr) {
  ((ColorDashboard*) ptr)->set_color_max();
}

void ColorDashboard::set_color_min() {
  if(active()) {
    colordrawer->plotmin = atof(fl_get_input(min_input));
    garcon()->log(menucommand() + "min=" + to_charstring(colordrawer->plotmin));
    formdrawer()->draw();
  }
}

void FDCB_color_min(FL_OBJECT*, long ptr) {
  ((ColorDashboard*) ptr)->set_color_min();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDashboard::fullscale() {
  draw_fullscale_objects();
  if(fullscale_flag) {
    colordrawer->fullscale();
    formdrawer()->draw();
  }
}

void ColorDashboard::set_fullscale() {
  if(active()) {
    fullscale_flag = fl_get_button(color_fullscale);
    // don't set fullscale_flag_dflt here -- default setting shouldn't change
    garcon()->log(menucommand() + "fullscale=" + to_charstring(fullscale_flag));
    fullscale();
  }
}

void FDCB_color_fullscale(FL_OBJECT*, long ptr) {
  ((ColorDashboard*) ptr)->set_fullscale();
}

void ColorDashboard::set_fullscale_cmd(const CharString&) {
  ((ColorDashboard*) get_dashboard())->fullscale();
}

void ColorDashboard::draw_fullscale_objects() {
  if(fullscale_flag) {
    fl_hide_object(max_input);
    fl_hide_object(min_input);
  }
  else {
    fl_show_object(max_input);
    fl_show_object(min_input);
  }
  fl_set_button(color_fullscale, fullscale_flag);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDashboard::set_colormap() {
  if(colordrawer->colormap)
    delete colordrawer->colormap;
  colordrawer->colormap =
    colormapregistry()[colormapnumber]->create(colormapsize);
  colordrawer->fill_colormap();
  if(gfxinfo.c_class() == TrueColor) {
    // RW colormaps redisplay colors automatically, but RO ones change
    // the pixel values, so require a redraw.
    paint_colorbar();
    refresh_colorbar();
    formdrawer()->draw();
  }
  fl_set_choice(color_map_menu, colormapnumber+1);
}

void ColorDashboard::change_color_map() {
  if(active()) {
    colormapnumber = colormapnumber_dflt = fl_get_choice(color_map_menu) - 1;
    garcon()->log(menucommand() + "colormap=" + to_charstring(colormapnumber));
    set_colormap();
  }
}

void FDCB_color_map_menu(FL_OBJECT*, long ptr) {
  ((ColorDashboard*) ptr)->change_color_map();
}

void ColorDashboard::set_colormap_cmd(const CharString&) {
  ((ColorDashboard*) get_dashboard())->set_colormap();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDashboard::set_colormapsize() {
  if(colordrawer->colormap)
    delete colordrawer->colormap;
  colordrawer->colormap = 
    colormapregistry()[colormapnumber]->create(colormapsize);
  colordrawer->fill_colormap();
  fl_set_input(colormapsize_input, to_charstring(colormapsize));
  paint_colorbar();
  refresh_colorbar();
  formdrawer()->draw();
}

void ColorDashboard::set_colormapsize_gui() {
  if(active()) {
    int newsize = atoi(fl_get_input(colormapsize_input));
    if(newsize < 2) {
      garcon()->msout << ms_error << "Be serious!" << endl << ms_normal;
      fl_set_input(colormapsize_input, to_charstring(colormapsize));
      return;
    }
    colormapsize = colormapsize_dflt = newsize;
    garcon()->log(menucommand() + "mapsize=" + to_charstring(colormapsize));
    set_colormapsize();
  }
}

void FDCB_colormapsize(FL_OBJECT*, long ptr) {
  ((ColorDashboard*) ptr)->set_colormapsize_gui();
}

void ColorDashboard::set_colormapsize_cmd(const CharString&) {
  ((ColorDashboard*) get_dashboard())->set_colormapsize();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDashboard::set_flip() {
  paint_colorbar();
  refresh_colorbar();
  formdrawer()->draw();
  fl_set_button(color_flip_button, flipcolors);
}

void ColorDashboard::flip_colormap() {
  if(active()) {
    flipcolors = flipcolors_dflt = fl_get_button(color_flip_button);
    garcon()->log(menucommand() + "flip=" + to_charstring(flipcolors));
    set_flip();
  }
}

void FDCB_color_flip(FL_OBJECT*, long ptr) {
  ((ColorDashboard*) ptr)->flip_colormap();
}

void ColorDashboard::flip_colormap_cmd(const CharString&) {
  ((ColorDashboard*) get_dashboard())->set_flip();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void ColorDashboard::paint_colorbar() const {  // draw colors in colorbar_pixmap
  if(colorbar_height == 0) return;
  DrawerColorMap *colormap = colordrawer->colormap;
  double invheight = 1./colorbar_height;
  for(int i=0; i<colorbar_height; i++) {
    unsigned long pixel;
    if(flipcolors)
      pixel = colormap->pixel(i*invheight);
    else
      pixel = colormap->pixel(1.0-i*invheight);
    XSetForeground(gfxinfo.display(), colorbar_pixmap.gc(), pixel);
    XFillRectangle(gfxinfo.display(), colorbar_pixmap.pixmap(),
		   colorbar_pixmap.gc(), 0, i, colorbar_width, 1);
  }
}

void ColorDashboard::refresh_colorbar() const {  // copy pixmap to canvas
  XCopyArea(gfxinfo.display(), colorbar_pixmap.pixmap(),
	    fl_get_canvas_id(colorbar), colorbar_pixmap.gc(),
	    0, 0, colorbar_width, colorbar_height, 0, 0);
}

void ColorDashboard::colorbar_expose(int w, int h) {
  if(colorbar_height != h || colorbar_width != w) { // need to resize pixmap
    colorbar_pixmap.resize(w, h);
    colorbar_height = h;
    colorbar_width = w;
    paint_colorbar();
  }
  refresh_colorbar();
}

int FDCB_colorbar_expose(FL_OBJECT *obj, Window, int win_width,
			   int win_height, XEvent*, void *)
{
  ColorDashboard *d = ColorDashboard::getcolordashboard(obj);
  if(d)
    d->colorbar_expose(win_width, win_height);
  return 0;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// modify color in color map

void ColorDashboard::colorbar_range(double whichbar, double &min, double &max)
     const
{
   double scale = (colordrawer->plotmax - colordrawer->plotmin)/colormapsize;
   if(flipcolors) whichbar = 1.0 - whichbar;
   min = colordrawer->plotmin + int(colormapsize*whichbar)*scale;
   max = colordrawer->plotmin + int(colormapsize*whichbar + 1)*scale;
}

void ColorDashboard::colorbar_change(double whichbar, const Color &newcolor) {
  Colormap cmap = canvas()->get_colormap();
  if(gfxinfo.c_class() == TrueColor) {
    colordrawer->colormap->change_RO(whichbar, newcolor);
    paint_colorbar();
    refresh_colorbar();
    formdrawer()->draw();
  }
  else
    colordrawer->colormap->change_RW(whichbar, newcolor, cmap);
}

void ColorDashboard::colorbar_revert(double whichbar) {
  Colormap cmap = canvas()->get_colormap();
  if(gfxinfo.c_class() == TrueColor) {
    colordrawer->colormap->revert_RO(whichbar);
    paint_colorbar();
    refresh_colorbar();
    formdrawer()->draw();
  }
  else
    colordrawer->colormap->revert_RW(whichbar, cmap);
}

void ColorDashboard::colorbar_mouseup(XEvent *xev) {
  if(active()) {
    int x = xev->xbutton.x;                     // x coord of mouse click
    if(x < 0 || x > colorbar->w) return;        // out of bounds
    int y = xev->xbutton.y;                     // y coord of mouse click
    double whichbar = y/(double) colorbar->h;
    if(whichbar < 0 || whichbar > 1.0) return;  // out of bounds
    if(!flipcolors) whichbar = 1.0 - whichbar;
    double min, max;
    colorbar_range(whichbar, min, max);
    switch(xev->xbutton.button) {
    case 1:         // left button modifies color 
      {
	garcon()->msout << ms_info << "Changing color representing the range ["
			<< min << ", " << max << "]" << endl << ms_normal;
	Color newcolor = colorbrowser(colordrawer->colormap->color(whichbar));
	garcon()->log(menucommand() + "newcol bar=" + to_charstring(whichbar) +
		      " color=" + to_charstring(newcolor));
	colorbar_change(whichbar, newcolor);
      }
    break;
    case 2:        // middle button make this color revert
      {
	garcon()->msout << ms_info << "Reverting color representing the range ["
			<< min << ", " << max << "]" << endl << ms_normal;
	garcon()->log(menucommand() + 
		      "revertcol bar=" + to_charstring(whichbar));
	colorbar_revert(whichbar);
      }
    break;
    case 3:        // right button prints range of this color 
      garcon()->msout << ms_info << "Color "
		      << colordrawer->colormap->color(whichbar)
		      << "  represents the range [" << min << ", "
		      << max << "]" << endl << ms_normal;
      break;
    }
  }
}

void ColorDashboard::colorbar_mousedn(XEvent*) {}

int FDCB_colorbar_mouseup(FL_OBJECT *obj, Window, int, int,
			  XEvent *xev, void *)
{
  ColorDashboard *d = ColorDashboard::getcolordashboard(obj);
  if(d)
    d->colorbar_mouseup(xev);
  return 0;
}

int FDCB_colorbar_mousedn(FL_OBJECT *obj, Window, int, int,
			  XEvent *xev, void *)
{
  ColorDashboard *d = ColorDashboard::getcolordashboard(obj);
  if(d)
    d->colorbar_mousedn(xev);
  return 0;
}
