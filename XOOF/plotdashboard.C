// -*- C++ -*-
// $RCSfile: plotdashboard.C,v $
// $Revision: 1.11 $
// $Author: langer $
// $Date: 2000-10-31 19:15:13 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "colordrawer.h"
#include "colorutils.h"
#include "delete_fl_group.h"
#include "filename.h"
#include "formdrawer.h"
#include "graphics.h"
#include "grid.h"
#include "lineplot.h"
#include "linklist2.h"
#include "menuDef.h"
#include "mouseclick.h"
#include "parameters.h"
#include "plotdashboard.h"
#include "oofcanvas.h"
#include <iomanip.h>
#include <math.h>

// default values used for "output/crosssection" commands as well as
// when opening new plot dashboards:
LinePlot PlotDashboard::dfltplot("", MeshCoord(0,0), MeshCoord(1,0),
				 100, 1.e-6, 1, 1, 1, 0);

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class CrossSectionFileCmd : public CommandM {
private:
  FileName filename;
public:
  CrossSectionFileCmd()
    : CommandM("plot_file", "write plot data to a file")
  {
    AddArgument(this, "filename", filename);
  }
  virtual CommandFn func() {
    ((PlotDashboard*) get_dashboard())->plot_file(filename);
  }
  virtual CommandM *clone() const {
    CrossSectionFileCmd *pfc = new CrossSectionFileCmd;
    pfc->filename = filename;
    return pfc;
  }
};

class CrossSectionStoreCmd : public CommandM {
private:
  CharString name;
public:
  CrossSectionStoreCmd()
    : CommandM("plot_store", "save plot parameters")
  {
    AddArgument(this, "name", name);
  }
  virtual void func() {
    ((PlotDashboard*) get_dashboard())->save_settings(name);
  }
  virtual CommandM *clone() const {
    CrossSectionStoreCmd *psc = new CrossSectionStoreCmd;
    psc->name = name;
    return psc;
  }
};

class CrossSectionRecallCmd : public CommandM {
private:
  int which;
public:
  CrossSectionRecallCmd()
    : CommandM("plot_recall", "recall plot parameters")
  {
    AddArgument(this, "n", which);
  }
  virtual void func() {
    ((PlotDashboard*) get_dashboard())->recall_settings(which);
  }
  virtual CommandM *clone() const {
    CrossSectionRecallCmd *prc = new CrossSectionRecallCmd;
    prc->which = which;
    return prc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

Vec<PlotDashboard*> PlotDashboard::allplotdash;

PlotDashboard::PlotDashboard(Drawer *drwr)
  : Dashboard("Cross Section", drwr, RB_LINE),
    currentplot(dfltplot),
    linedrawn(0)
{
  create_form_plot(this);

  fl_freeze_form(formdrawer()->the_form);

  if(linedrawn) {
    fl_set_input(input_xmin, to_charstring(currentplot.start.x));
    fl_set_input(input_xmax, to_charstring(currentplot.finish.x));
    fl_set_input(input_ymin, to_charstring(currentplot.start.y));
    fl_set_input(input_ymax, to_charstring(currentplot.finish.y));
  }
  fl_set_input(input_npts, to_charstring(currentplot.npts));
  fl_set_button(button_even_spacing, currentplot.equalspace);
  fl_set_input(input_tolerance, to_charstring(currentplot.tolerance));
  fl_set_button(button_plotx, currentplot.plotx);
  fl_set_button(button_ploty, currentplot.ploty);
  fl_set_button(button_plotarc, currentplot.plots);

  activate_tolerance_input();

  // deactivate the "store settings" button until a line has been defined
  fl_deactivate_object(button_save_settings);
  fl_set_object_lcolor(button_save_settings, FL_INACTIVE_COL);

  fl_set_browser_fontstyle(browser_plotselect, FL_BOLD_STYLE);
  for(int i=0; i<storedplots.capacity(); i++)
    fl_add_browser_line(browser_plotselect, storedplots[i]->name);
  fl_unfreeze_form(formdrawer()->the_form);

  AddVariable(&menu, "plot_xmin", "set x min for plot",
	      currentplot.start.x, set_xmin_cmd);
  AddVariable(&menu, "plot_xmax", "set x max for plot",
	      currentplot.finish.x, set_xmax_cmd);
  AddVariable(&menu, "plot_ymin", "set y min for plot",
	      currentplot.start.y, set_ymin_cmd);
  AddVariable(&menu, "plot_ymax", "set y max for plot",
	      currentplot.finish.y, set_ymax_cmd);
  AddVariable(&menu, "plot_npts", "set npts for plot",
	      currentplot.npts, set_npts_cmd);
  AddVariable(&menu, "equalspace", "use equally spaced points?",
	      currentplot.equalspace, set_equalspace_cmd);
  AddVariable(&menu, "plot_tolerance",
	      "minimum distance between unequally spaced point",
	      currentplot.tolerance, set_tolerance_cmd);
  AddVariable(&menu, "plot_x", "should x be plotted?", currentplot.plotx,
	      set_plotx_cmd);
  AddVariable(&menu, "plot_y", "should y be plotted?", currentplot.ploty,
	      set_ploty_cmd);
  AddVariable(&menu, "plot_arc", "should arc be plotted?", currentplot.plots,
	      set_plotarc_cmd);
  menu.AddCommand(new CrossSectionFileCmd);
  menu.AddCommand(new CrossSectionStoreCmd);
  menu.AddCommand(new CrossSectionRecallCmd);
  menu.AddCommand("plot_clear", "clear the plot parameters", clearplot_cmd);
  allplotdash.grow(1, this);
}

PlotDashboard::~PlotDashboard() {
  hide();
  delete_fl_group(plotsettings);
  menu.RemoveOption("plot_xmin");
  menu.RemoveOption("plot_ymin");
  menu.RemoveOption("plot_xmax");
  menu.RemoveOption("plot_ymax");
  menu.RemoveOption("plot_npts");
  menu.RemoveOption("equalspace");
  menu.RemoveOption("plot_file");
  menu.RemoveOption("plot_x");
  menu.RemoveOption("plot_y");
  menu.RemoveOption("plot_arc");
  menu.RemoveOption("plot_store");
  menu.RemoveOption("plot_recall");
  menu.RemoveOption("plot_clear");
  menu.RemoveOption("plot_tolerance");
  allplotdash.remove(this);
}

void PlotDashboard::activate_tolerance_input() const {
  fl_freeze_form(formdrawer()->the_form);
  if(!currentplot.equalspace) {
    fl_activate_object(input_tolerance);
    fl_set_object_lcolor(input_tolerance, FL_BLACK);
    fl_set_input_color(input_tolerance, FL_BLACK, FL_BLACK);
    fl_deactivate_object(input_npts);
    fl_set_object_lcolor(input_npts, FL_INACTIVE_COL);
    fl_set_input_color(input_npts, FL_INACTIVE_COL, FL_BLACK);
  }
  else {
    fl_deactivate_object(input_tolerance);
    fl_set_object_lcolor(input_tolerance, FL_INACTIVE_COL);
    fl_set_input_color(input_tolerance, FL_INACTIVE_COL, FL_BLACK);
    fl_activate_object(input_npts);
    fl_set_object_lcolor(input_npts, FL_BLACK);
    fl_set_input_color(input_npts, FL_BLACK, FL_BLACK);
  }
  fl_redraw_object(input_tolerance);
  fl_redraw_object(input_npts);
  fl_unfreeze_form(formdrawer()->the_form);
}

void PlotDashboard::show() {
  setcursor(*crosshaircursor);
  fl_show_object(plotsettings);
  formdrawer()->drag_rubberband();
}

void PlotDashboard::hide() {
  unsetcursor();
  fl_hide_object(plotsettings);
  formdrawer()->drag_nothing();
}

void PlotDashboard::draghandler(const MouseClick &mousedn,
				const MouseClick &mouseup)
{
  if(!grid) return;
  currentplot.start = mousedn.coord;
  currentplot.finish = mouseup.coord;
  set_xmin();
  set_xmax();
  set_ymin();
  set_ymax();
  redraw_plotline();
}

void PlotDashboard::draw_plotline() const {
  if(linedrawn) {
    XPoint p1 = canvas()->xpoint(currentplot.start);
    XPoint p2 = canvas()->xpoint(currentplot.finish);
    XSetForeground(gfxinfo.display(), canvas()->gc(),
		   dynamic_cast<MeshDrawer*>(drawer)->plotlinecolor.pixel);
    XSetLineAttributes(gfxinfo.display(), canvas()->gc(), 1, LineSolid,
		       CapButt, JoinBevel);
    XDrawLine(gfxinfo.display(), canvas()->pixmap(), canvas()->gc(),
	      p1.x, p1.y, p2.x, p2.y);
  }
}

static bool inputexists(FL_OBJECT *obj) {
  return CharString(fl_get_input(obj)) != "";
}

void PlotDashboard::redraw_plotline() {
  bool oldlinedrawn = linedrawn;
  // check to see if the line has been completely specified
  linedrawn = inputexists(input_xmin) && inputexists(input_xmax)
    && inputexists(input_ymin) && inputexists(input_ymax);
  // redraw if the line has been completely specified, or if it had
  // been specified and is no longer specified (in which case it must
  // be erased)
  if(linedrawn || oldlinedrawn)
    formdrawer()->draw();
  if(linedrawn) {
    fl_activate_object(button_save_settings);
    fl_set_object_lcol(button_save_settings, FL_BLACK);
  }
  else {
    fl_deactivate_object(button_save_settings);
    fl_set_object_lcol(button_save_settings, FL_INACTIVE_COL);
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static bool checkinput(FL_OBJECT *obj, double val) {
  CharString inp(fl_get_input(obj));
  return (inp != "") && (atof(inp) != val);
}

void PlotDashboard::update_inputs() {
  if(checkinput(input_xmin, currentplot.start.x)) set_xmin_gui();
  if(checkinput(input_ymin, currentplot.start.y)) set_ymin_gui();
  if(checkinput(input_xmax, currentplot.finish.x)) set_xmax_gui();
  if(checkinput(input_ymax, currentplot.finish.y)) set_ymax_gui();
  if(currentplot.npts != atoi(fl_get_input(input_npts)))
    set_npts_gui();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_xmin_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_xmin();
  ((PlotDashboard*) get_dashboard())->redraw_plotline();
}

void PlotDashboard::set_xmin() {
  set_xmin(currentplot.start.x);
  fl_set_input(input_xmin, to_charstring(currentplot.start.x));
}

void FDCB_plot_input_xmin(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_xmin_gui();
}

void PlotDashboard::set_xmin_gui() {
  if(active()) {
    if(set_xmin(atof(fl_get_input(input_xmin))))
      redraw_plotline();
    garcon()->log(menucommand()
		  + "plot_xmin = " + to_charstring(currentplot.start.x));
  }
}

bool PlotDashboard::set_xmin(double v) {
  bool changed = (currentplot.start.x != v);
  currentplot.start.x = dfltplot.start.x = v;
  return changed;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_ymin_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_ymin();
  ((PlotDashboard*) get_dashboard())->redraw_plotline();
}

void PlotDashboard::set_ymin() {
  set_ymin(currentplot.start.y);
  fl_set_input(input_ymin, to_charstring(currentplot.start.y));
}

void FDCB_plot_input_ymin(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_ymin_gui();
}

void PlotDashboard::set_ymin_gui() {
  if(active()) {
    if(set_ymin(atof(fl_get_input(input_ymin))))
      redraw_plotline();
    garcon()->log(menucommand() +
		  "plot_ymin = " + to_charstring(currentplot.start.y));
  }
}

bool PlotDashboard::set_ymin(double v) {
  bool changed = (currentplot.start.y != v);
  currentplot.start.y = dfltplot.start.y = v;
  return changed;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_xmax_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_xmax();
  ((PlotDashboard*) get_dashboard())->redraw_plotline();
}

void PlotDashboard::set_xmax() {
  set_xmax(currentplot.finish.x);
  fl_set_input(input_xmax, to_charstring(currentplot.finish.x));
}

void FDCB_plot_input_xmax(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_xmax_gui();
}

void PlotDashboard::set_xmax_gui() {
  if(active()) {
    if(set_xmax(atof(fl_get_input(input_xmax))))
      redraw_plotline();
    garcon()->log(menucommand()
		  + "plot_xmax = " + to_charstring(currentplot.finish.x));
  }
}

bool PlotDashboard::set_xmax(double v) {
  bool changed = (currentplot.finish.x != v);
  currentplot.finish.x = dfltplot.finish.x = v;
  return changed;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_ymax_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_ymax();
  ((PlotDashboard*) get_dashboard())->redraw_plotline();
}

void PlotDashboard::set_ymax() {
  set_ymax(currentplot.finish.y);
  fl_set_input(input_ymax, to_charstring(currentplot.finish.y));
}

void FDCB_plot_input_ymax(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_ymax_gui();
}

void PlotDashboard::set_ymax_gui() {
  if(active()) {
    if(set_ymax(atof(fl_get_input(input_ymax))))
      redraw_plotline();
    garcon()->log(menucommand()
		  + "plot_ymax = " + to_charstring(currentplot.finish.y));
  }
}

bool PlotDashboard::set_ymax(double v) {
  bool changed = (currentplot.finish.y != v);
  currentplot.finish.y = dfltplot.finish.y = v;
  return changed;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_equalspace_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_equalspace();
}

void PlotDashboard::set_equalspace() {
  set_equalspace(currentplot.equalspace);
}

void PlotDashboard::set_equalspace(bool eq) {
  currentplot.equalspace = dfltplot.equalspace = eq;
  fl_set_button(button_even_spacing, currentplot.equalspace);
  activate_tolerance_input();
}

void FDCB_button_spacing(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_equalspace_gui();
}

void PlotDashboard::set_equalspace_gui() {
  if(active()) {
    set_equalspace(fl_get_button(button_even_spacing));
    garcon()->log(menucommand() + "equalspace = "
		  + to_charstring(currentplot.equalspace));
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_tolerance_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_tolerance();
}

void PlotDashboard::set_tolerance() {
  set_tolerance(currentplot.tolerance);
  fl_set_input(input_tolerance, to_charstring(currentplot.tolerance));
}

void FDCB_plot_input_tol(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_tolerance_gui();
}

void PlotDashboard::set_tolerance_gui() {
  if(active()) {
    set_tolerance(atof(fl_get_input(input_tolerance)));
    garcon()->log(menucommand()
		  + "plot_tolerance = " + to_charstring(currentplot.tolerance));
  }
}

void PlotDashboard::set_tolerance(double tol) {
  currentplot.tolerance = dfltplot.tolerance = tol;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_plotx_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_plotx();
}

void PlotDashboard::set_plotx() {
  set_plotx(currentplot.plotx);
}

void PlotDashboard::set_plotx(bool px) {
  currentplot.plotx = dfltplot.plotx = px;
  fl_set_button(button_plotx, currentplot.plotx);
}

void FDCB_button_plotx(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_plotx_gui();
}

void PlotDashboard::set_plotx_gui() {
  if(active()) {
    set_plotx(fl_get_button(button_plotx));
    garcon()->log(menucommand() + "plot_x = "
		  + to_charstring(currentplot.plotx));
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_ploty_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_ploty();
}

void PlotDashboard::set_ploty() {
  set_ploty(currentplot.ploty);
}

void PlotDashboard::set_ploty(bool py) {
  currentplot.ploty = dfltplot.ploty = py;
  fl_set_button(button_ploty, currentplot.ploty);
}

void FDCB_button_ploty(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_ploty_gui();
}

void PlotDashboard::set_ploty_gui() {
  if(active()) {
    set_ploty(fl_get_button(button_ploty));
    garcon()->log(menucommand() + "plot_y = "
		  + to_charstring(currentplot.ploty));
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_plotarc_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_plotarc();
}

void PlotDashboard::set_plotarc() {
  set_plotarc(currentplot.plots);
}

void PlotDashboard::set_plotarc(bool parc) {
  currentplot.plots = dfltplot.plots = parc;
  fl_set_button(button_plotarc, currentplot.plots);
}

void FDCB_button_plotarc(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_plotarc_gui();
}

void PlotDashboard::set_plotarc_gui() {
  if(active()) {
    set_plotarc(fl_get_button(button_plotarc));
    garcon()->log(menucommand() + "plot_arc = "
		  + to_charstring(currentplot.plots));
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::set_npts_cmd(const CharString&) {
  ((PlotDashboard*) get_dashboard())->set_npts();
}

void PlotDashboard::set_npts() {
  set_npts(currentplot.npts);
}

void PlotDashboard::set_npts(int n) {
  currentplot.npts = dfltplot.npts = n;
  fl_set_input(input_npts, to_charstring(currentplot.npts));
}

void FDCB_plot_input_npts(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->set_npts_gui();
}

void PlotDashboard::set_npts_gui() {
  if(active()) {
    set_npts(atoi(fl_get_input(input_npts)));
    garcon()->log(menucommand() + "plot_npts = "
		  + to_charstring(currentplot.npts));
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

CommandFn PlotDashboard::clearplot_cmd() {
  ((PlotDashboard*) get_dashboard())->clearplot();
}

void FDCB_button_clearplot(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->clearplot_gui();
}

void PlotDashboard::clearplot_gui() {
  clearplot();
  garcon()->log(menucommand() + "plot_clear");
}

void PlotDashboard::clearplot() {
  fl_freeze_form(formdrawer()->the_form);
  fl_set_input(input_xmin, "");
  fl_set_input(input_xmax, "");
  fl_set_input(input_ymin, "");
  fl_set_input(input_ymax, "");
  fl_unfreeze_form(formdrawer()->the_form);
  redraw_plotline();		// will set linedrawn=0
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void PlotDashboard::plot_file(const FileName &file) const {
  if(grid) {
    grid->lineplot(file, currentplot,
		   dynamic_cast<ColorDrawer*>(drawer)->func());
  }
}

void FDCB_button_saveplot(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->saveplot_gui();
}

void PlotDashboard::saveplot_gui() {
  FileName fname(fl_show_fselector("Enter plot file name", ".", "", ""));
  if(fname != "") {
    update_inputs();
    plot_file(fname);
    garcon()->log(menucommand() + "plot_file filename=" + fname);
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

int PlotDashboard::nplots = 0;
Vec<LinePlot*> PlotDashboard::storedplots;

void FDCB_browser_plotselect(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->recall_settings_gui();
}

void PlotDashboard::recall_settings_gui() {
  int which = fl_get_browser(browser_plotselect) - 1;
  recall_settings(which);
  garcon()->log(menucommand() + "plot_recall n=" + to_charstring(which));
}

void PlotDashboard::recall_settings(int which) {
  if(which < storedplots.capacity() && which >= 0) {
    dfltplot = currentplot = *storedplots[which];

    // display the current settings
    set_xmin();
    set_xmax();
    set_ymin();
    set_ymax();
    set_npts();
    set_plotx();
    set_ploty();
    set_plotarc();
    set_equalspace();
    redraw_plotline();
  }
}

void FDCB_button_save_settings(FL_OBJECT*, long ptr) {
  ((PlotDashboard*) ptr)->save_settings_gui();
}

void PlotDashboard::save_settings_gui() {
  CharString plotname = garcon()->getstring("Give this plot a name:",
					    "plot"+to_charstring(nplots));
  save_settings(plotname);
  garcon()->log(menucommand() + "plot_store name=" + plotname);
}

void PlotDashboard::save_settings(const CharString &plotname) {
  if(plotname != "") {
    // Store a copy of the current plot
    update_inputs();

    // Look for an existing plot with the same name
    for(int i=0; i<storedplots.capacity(); i++) {
      if(storedplots[i]->name == plotname) {
	// replace existing plot
	delete storedplots[i];
	storedplots[i] = new LinePlot(currentplot);
	storedplots[i]->name = plotname;
	return;
      }
    }

    nplots++;

    // Add a new plot to the list
    int j = storedplots.grow(1, new LinePlot(currentplot));
    storedplots[j]->name = plotname;

    // Add it to the list in all the plot dashboards
    for(int i=0; i<allplotdash.capacity(); i++)
      fl_add_browser_line(allplotdash[i]->browser_plotselect,
			  plotname.charstar());
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//
//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class LinePlotPt {
public:
  LinePlotPt(const MeshCoord &p, double ess, Element *el, double v)
    : pt(p), s(ess), element(el), val(v)
  {}
  Element *element;		// element under this point
  MeshCoord pt;
  double s;
  double val;
  void plot(const LinePlot&, ostream &) const;
};

void LinePlotPt::plot(const LinePlot &plot, ostream &file) const {
  file << setw(19) << setprecision(16);
  if(plot.plotx) file << pt.x << " ";
  if(plot.ploty) file << pt.y << " ";
  if(plot.plots) file << s << " ";
  file << val << endl;
}

void Grid::lineplot(const FileName &filename,
		    const LinePlot &plot,
		    const ElementFunctionBase &elephant)
{
  if(plot.equalspace && plot.npts <= 2) return;

  if(!findElement(plot.start) || !findElement(plot.finish))
    garcon()->msout
      << ms_error
      << "Warning! Endpoints are not inside the mesh!" << endl << ms_normal;

  ofstream file(filename);

  // make a linked list of pointers to points
  LinkList2<LinePlotPt*> list;

  // fill the list with equally spaced points
  // start with only 10 points for unequally spaced plots
  int npts = (plot.equalspace ? plot.npts : 10);
  MeshCoord delta = (plot.finish - plot.start)/(npts - 1.0);
  for(int i=0; i<npts; i++) {
    MeshCoord here = plot.start + i*delta;
    double v = 0.0;
    Element *el = findElement(here);
    if(el)
      v = elephant(*el);
    double s = sqrt(sq_distance(here, plot.start));
    list.AddTail(new LinePlotPt(here, s, el, v));
  }

  if(!plot.equalspace) {
    // insert a new point between adjacent points in different
    // elements, if the points aren't too close
    for(LinkList2Iterator<LinePlotPt*> iter(list.begin()); !iter.end(); ++iter) 
      {
	LinePlotPt *thispt = list[iter];
	LinkList2Iterator<LinePlotPt*> nextiter(iter+1);
	if(nextiter.end()) break; // ugly
	LinePlotPt *nextpt = list[nextiter];
	while(thispt->element != nextpt->element &&
	   sq_distance(thispt->pt, nextpt->pt) > plot.tolerance*plot.tolerance)
	  {
	    MeshCoord newpos = 0.5*(thispt->pt + nextpt->pt);
	    double s = sqrt(sq_distance(newpos, plot.start));
	    Element *el = findElement(newpos);
	    if(el)
	      nextpt = new LinePlotPt(newpos, s, el, elephant(*el));
	    else
	      nextpt = new LinePlotPt(newpos, s, 0, 0.0);
	    list.Insert(iter, nextpt);
	}
      }

    // remove redundant points: points whose elements are the same as
    // the points on either side.

    for(LinkList2Iterator<LinePlotPt*> iter(list.begin()+1),
	  nextiter(list.begin()+2); !nextiter.end(); ++iter, nextiter=iter+1)
      {
	Element *prevelem = list[iter-1]->element;
	while(iter && nextiter
	      && list[iter]->element == list[nextiter]->element
	      && list[iter]->element == prevelem)
	  {
	    list.Remove(iter);	// increments iter
	    ++nextiter;
	  }
      }
  }

  // Plot the list of points
  int nactual = 0;
  for(LinkList2Iterator<LinePlotPt*> iter(list.begin()); !iter.end(); ++iter) {
    if(list[iter]->element) {
      list[iter]->plot(plot, file);
      nactual++;
    }
    delete list[iter];
  }

  if(!plot.equalspace)
    garcon()->msout << ms_info << "Plotting " << nactual << " points"
		    << endl << ms_normal;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//
//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Make a plot when using the text interface

class CrossSectionEqCmd : public CommandM {
private:
  const ElementFunctionBase &elfun;
  FileName filename;
public:
  CrossSectionEqCmd(const ElementFunctionBase &ef)
    : CommandM(ef.name().despace(), "plot " + ef.name() + " along a line"),
      elfun(ef), filename(ef.name().despace())
  {
    AddArgument(this, "filename", filename);
    AddArgument(this, "start", PlotDashboard::dfltplot.start);
    AddArgument(this, "finish", PlotDashboard::dfltplot.finish);
    AddArgument(this, "npts", PlotDashboard::dfltplot.npts);
    AddArgument(this, "x", PlotDashboard::dfltplot.plotx);
    AddArgument(this, "y", PlotDashboard::dfltplot.ploty);
    AddArgument(this, "s", PlotDashboard::dfltplot.plots);
  }
  virtual void func() {
    if(gridinit()) {
      PlotDashboard::dfltplot.equalspace = 1;
      grid->lineplot(filename, PlotDashboard::dfltplot, elfun);
    }
  }
  virtual CommandM *clone() const {
    CrossSectionEqCmd *csc = new CrossSectionEqCmd(elfun);
    csc->filename = filename;
    return csc;
  }
};

class CrossSectionUneqCmd : public CommandM {
private:
  const ElementFunctionBase &elfun;
  FileName filename;
public:
  CrossSectionUneqCmd(const ElementFunctionBase &ef)
    : CommandM(ef.name().despace(), "plot " + ef.name() + " along a line"),
      elfun(ef), filename(ef.name().despace())
  {
    AddArgument(this, "filename", filename);
    AddArgument(this, "start", PlotDashboard::dfltplot.start);
    AddArgument(this, "finish", PlotDashboard::dfltplot.finish);
    AddArgument(this, "tolerance", PlotDashboard::dfltplot.tolerance);
    AddArgument(this, "x", PlotDashboard::dfltplot.plotx);
    AddArgument(this, "y", PlotDashboard::dfltplot.ploty);
    AddArgument(this, "s", PlotDashboard::dfltplot.plots);
  }
  virtual void func() {
    if(gridinit()) {
      PlotDashboard::dfltplot.equalspace = 0;
      grid->lineplot(filename, PlotDashboard::dfltplot, elfun);
    }
  }
  virtual CommandM *clone() const {
    CrossSectionUneqCmd *csc = new CrossSectionUneqCmd(elfun);
    csc->filename = filename;
    return csc;
  }
};

Menu *crosssectionmenu() {
  static Menu *menu = 0;
  if(!menu) {
    menu = new Menu("cross_section");
    Menu *eqmenu = menu->AddSubMenu("uniform",
				    "plot points with uniform spacing");
    Menu *uneqmenu = menu->AddSubMenu("nonuniform",
				      "plot points at element boundaries only");

    // put functions in order before putting them in menu
    ElementFunctionBase::sort();

    const Vec<ElementFunctionBase*> &fns
      = ElementFunctionBase::allElementFunctions();
    for(int i=0; i<fns.capacity(); i++) {
      eqmenu->AddCommand(new CrossSectionEqCmd(*fns[i]));
      uneqmenu->AddCommand(new CrossSectionUneqCmd(*fns[i]));
    }
  }
  return menu;
}
