// -*- C++ -*-
// $RCSfile: plotdashboard.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2001-02-18 02:34:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef PLOTDASHBOARD_H
#define PLOTDASHBOARD_H

#include "dashboard.h"
#include "forms.h"
#include "lineplot.h"
#include "menuDef.h"
#include "meshcoord.h"

class Rectangle;

class PlotDashboard : public Dashboard {
private:
  PlotDashboard(Drawer *drwr);
  virtual ~PlotDashboard();

  LinePlot currentplot;
  static LinePlot dfltplot;
  bool linedrawn;

  static Vec<PlotDashboard*> allplotdash;

  void draw_plotline() const;
  void draw_plotline_ps(FILE*, const Rectangle&) const;
  void redraw_plotline();
  void update_inputs();

  virtual void show();
  virtual void hide();
  virtual void draghandler(const MouseClick&, const MouseClick&);

  // XForms objects
  FL_OBJECT *plotsettings;
  FL_OBJECT *input_xmin;
  FL_OBJECT *input_ymin;
  FL_OBJECT *input_xmax;
  FL_OBJECT *input_ymax;
  FL_OBJECT *input_tolerance;
  FL_OBJECT *button_even_spacing;
  FL_OBJECT *input_npts;
  FL_OBJECT *button_saveplot;
  FL_OBJECT *browser_plotselect;
  FL_OBJECT *button_save_settings;
  FL_OBJECT *button_plotx;
  FL_OBJECT *button_ploty;
  FL_OBJECT *button_plotarc;
  FL_OBJECT *button_clearplot;

  static int nplots;
  static Vec<LinePlot*> storedplots;

  static void set_xmin_cmd(const CharString&);
  bool set_xmin(double);
  void set_xmin();
  void set_xmin_gui();
  friend void FDCB_plot_input_xmin(FL_OBJECT *, long);

  static void set_ymin_cmd(const CharString&);
  bool set_ymin(double);
  void set_ymin();
  void set_ymin_gui();
  friend void FDCB_plot_input_ymin(FL_OBJECT *, long);

  static void set_xmax_cmd(const CharString&);
  bool set_xmax(double);
  void set_xmax();
  void set_xmax_gui();
  friend void FDCB_plot_input_xmax(FL_OBJECT *, long);

  static void set_ymax_cmd(const CharString&);
  bool set_ymax(double);
  void set_ymax();
  void set_ymax_gui();
  friend void FDCB_plot_input_ymax(FL_OBJECT *, long);

  static void set_equalspace_cmd(const CharString&);
  void set_equalspace_gui();
  void set_equalspace();
  void set_equalspace(bool);
  friend void FDCB_button_spacing(FL_OBJECT *, long);

  static void set_npts_cmd(const CharString&);
  void set_npts();
  void set_npts_gui();
  void set_npts(int);
  friend void FDCB_plot_input_npts(FL_OBJECT*, long);

  // tolerance for unequally spaced plot points
  static void set_tolerance_cmd(const CharString&);
  void set_tolerance();
  void set_tolerance_gui();
  void set_tolerance(double);
  friend void FDCB_plot_input_tol(FL_OBJECT*, long);
  void activate_tolerance_input() const; // allows/disallows input

  // should x be saved in the output file?
  static void set_plotx_cmd(const CharString&);
  void set_plotx();
  void set_plotx_gui();
  void set_plotx(bool);
  friend void FDCB_button_plotx(FL_OBJECT*, long);

  // should y be saved in the output file?
  static void set_ploty_cmd(const CharString&);
  void set_ploty();
  void set_ploty_gui();
  void set_ploty(bool);
  friend void FDCB_button_ploty(FL_OBJECT*, long);

  // should arclength be saved in the output file?
  static void set_plotarc_cmd(const CharString&);
  void set_plotarc();
  void set_plotarc_gui();
  void set_plotarc(bool);
  friend void FDCB_button_plotarc(FL_OBJECT*, long);

  friend void FDCB_button_saveplot(FL_OBJECT *, long);
  friend class CrossSectionFileCmd;
  void plot_file(const FileName&) const;
  void saveplot_gui();

  friend void FDCB_browser_plotselect(FL_OBJECT *, long);
  friend class CrossSectionRecallCmd;
  void recall_settings_gui();
  void recall_settings(int);

  friend void FDCB_button_save_settings(FL_OBJECT *, long);
  friend class CrossSectionStoreCmd;
  void save_settings_gui();
  void save_settings(const CharString&);

  friend void FDCB_button_clearplot(FL_OBJECT*, long);
  static CommandFn clearplot_cmd();
  void clearplot();
  void clearplot_gui();

  friend void create_form_plot(PlotDashboard*);
  friend class ColorDrawer;
  friend class CrossSectionEqCmd;
  friend class CrossSectionUneqCmd;
};

#endif
