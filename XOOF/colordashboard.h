// -*- C++ -*-
// $RCSfile: colordashboard.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef COLORDASHBOARD_H
#define COLORDASHBOARD_H

#include "pixmap.h"
#include "dashboard.h"
#include "forms.h"
#include "menuDef.h"

class ColorDrawer;
class ChangeColorCmd;
class RevertColorCmd;

class ColorDashboard : public Dashboard {
private:
  ColorDrawer *colordrawer;

  static Vec<ColorDashboard*> allcolordashboards;
  static ColorDashboard *getcolordashboard(FL_OBJECT*);

  FL_OBJECT *color_control;
  FL_OBJECT *max_input;
  FL_OBJECT *min_input;
  FL_OBJECT *color_fullscale;
  FL_OBJECT *color_map_menu;
  FL_OBJECT *colormapsize_input;
  FL_OBJECT *color_flip_button;
  FL_OBJECT *valmin_text;
  FL_OBJECT *valmax_text;
  FL_OBJECT *colorbar;

  static int colormapsize_dflt;
  static int colormapnumber_dflt;
  static TrueFalse fullscale_flag_dflt;
  static TrueFalse flipcolors_dflt;

  int colormapsize;
  int colormapnumber;
  TrueFalse fullscale_flag;
  TrueFalse flipcolors;

  void set_color_max();
  void set_color_min();
  friend void FDCB_color_max(FL_OBJECT *, long);
  friend void FDCB_color_min(FL_OBJECT *, long);
  void set_limits();
  static void set_limits_cmd(const CharString&);

  void fullscale();
  void set_fullscale();
  friend void FDCB_color_fullscale(FL_OBJECT *, long);
  static void set_fullscale_cmd(const CharString&);

  void set_colormap();
  void change_color_map();
  friend void FDCB_color_map_menu(FL_OBJECT *, long);
  static void set_colormap_cmd(const CharString&);

  void set_colormapsize();
  void set_colormapsize_gui();
  friend void FDCB_colormapsize(FL_OBJECT *, long);
  static void set_colormapsize_cmd(const CharString&);
  
  void set_flip();
  void flip_colormap();
  friend void FDCB_color_flip(FL_OBJECT*, long);
  static void flip_colormap_cmd(const CharString&);

  int colorbar_height, colorbar_width;
  PixMap colorbar_pixmap;
  void paint_colorbar() const;
  void refresh_colorbar() const;

  void colorbar_expose(int, int);
  friend int FDCB_colorbar_expose(FL_OBJECT*, Window, int, int,
				    XEvent*, void*);

  void colorbar_change(double, const Color&); // change a single color
  void colorbar_revert(double);               // revert a single color
  void colorbar_range(double, double&, double&) const;
  void colorbar_mousedn(XEvent*);
  void colorbar_mouseup(XEvent*);
  friend int FDCB_colorbar_mouseup(FL_OBJECT*, Window, int, int,
				    XEvent*, void*);
  friend int FDCB_colorbar_mousedn(FL_OBJECT*, Window, int, int,
				    XEvent*, void*);
  ChangeColorCmd *changecolorcmd;
  RevertColorCmd *revertcolorcmd;

  void displaylimits() const;
  void draw_fullscale_objects();

  virtual void show();
  virtual void hide();

  ColorDashboard(ColorDrawer*);
  virtual ~ColorDashboard();

  friend class ColorDrawer;
  friend class ChangeColorCmd;
  friend class RevertColorCmd;
  friend void create_form_color_control(ColorDashboard*);
};

#endif
