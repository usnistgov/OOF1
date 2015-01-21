// -*- C++ -*-
// $RCSfile: autodash.C,v $
// $Revision: 1.15 $
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

#include "autodash.h"
#include "goof.h"
#include "imageform.h"
#include "ppm2oof.h"
#include "stdlib.h"

// default values of parameters
CharString AutomationDashboard::graingroup_dflt("grains");
CharString AutomationDashboard::bdygroup_dflt("boundaries");
TrueFalse AutomationDashboard::verbose_dflt(TF_FALSE);
TrueFalse AutomationDashboard::redraw_dflt(TF_FALSE);
Color AutomationDashboard::lowerColorBound_dflt(180);
Color AutomationDashboard::upperColorBound_dflt(210);
int AutomationDashboard::localBurn_dflt(10);
int AutomationDashboard::globalBurn_dflt(60);
int AutomationDashboard::minSize_dflt(10);
double AutomationDashboard::minAspect_dflt(3.5);
double AutomationDashboard::maxAspect_dflt(10.);
double AutomationDashboard::maxPerimeter_dflt(3.25);

// variable names in the menu
static const CharString GRAINGROUP("graingroup");
static const CharString BDYGROUP("bdygroup");
static const CharString VERBOSE("verbose");
static const CharString REDRAW("redraw");
static const CharString LOWERCOLOR("lowerColorBound");
static const CharString UPPERCOLOR("upperColorBound");
static const CharString LOCALFLAM("local_flammability");
static const CharString GLOBALFLAM("global_flammability");
static const CharString MINSELECTION("min_selection");
static const CharString MINASPECT("min_Aspect");
static const CharString MAXASPECT("max_Aspect");
static const CharString MAXPERIMETER("max_Perimeter");

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

AutomationDashboard::AutomationDashboard(ImageForm *iform, FL_OBJECT *grp)
  : Dashboard("Automate", iform, grp),
    graingroup(graingroup_dflt),
    bdygroup(bdygroup_dflt),
    verbose(verbose_dflt),
    redraw(redraw_dflt),
    lowerColorBound(lowerColorBound_dflt),
    upperColorBound(upperColorBound_dflt),
    localBurn(localBurn_dflt),
    globalBurn(globalBurn_dflt),
    minSize(minSize_dflt),
    minAspect(minAspect_dflt),
    maxAspect(maxAspect_dflt),
    maxPerimeter(maxPerimeter_dflt)
{
  AddVariable(menu, GRAINGROUP, "set name of grain group", graingroup,
	      params_changed);
  AddVariable(menu, BDYGROUP, "set name of boundary group", bdygroup,
	      params_changed);
  AddVariable(menu, VERBOSE, "print lots of extra stuff?", verbose,
	      params_changed);
  AddVariable(menu, REDRAW, "redraw screen after each selection?", redraw,
	      params_changed);
  AddVariable(menu, LOWERCOLOR, "minimum gray to ignite",
	      lowerColorBound, params_changed);
  AddVariable(menu, UPPERCOLOR, "maximum gray to ignite",
	      upperColorBound, params_changed);
  AddVariable(menu, LOCALFLAM, "local flammability", localBurn,
	      params_changed);
  AddVariable(menu, GLOBALFLAM, "global flammability", globalBurn,
	      params_changed);
  AddVariable(menu, MINSELECTION, "minimum selection size", minSize,
	      params_changed);
  AddVariable(menu, MINASPECT, "minimum aspect ratio for boundaries",
	      minAspect, params_changed);
  AddVariable(menu, MAXASPECT, "maximum aspect ratio for grains",
	      maxAspect, params_changed);
  AddVariable(menu, MAXPERIMETER, "maximum perimeter ratio for grains",
	      maxPerimeter, params_changed);
  menu->AddCommand("go", "start the automation", go_cmd);

  update_gui();			// make sure current values are displayed
}

void AutomationDashboard::update_gui() const {
  if(textmode) return;
  fl_freeze_form(imageform()->xform());
  fl_set_input(fdui()->input_graingroup, graingroup);
  fl_set_input(fdui()->input_bdygroup, bdygroup);
  fl_set_input(fdui()->input_minburn, to_charstring(lowerColorBound));
  fl_set_input(fdui()->input_maxburn, to_charstring(upperColorBound));
  fl_set_input(fdui()->input_local_flam, to_charstring(localBurn));
  fl_set_input(fdui()->input_global_flam, to_charstring(globalBurn));
  fl_set_input(fdui()->input_min_selection, to_charstring(minSize));
  fl_set_input(fdui()->input_max_aspect, to_charstring(maxAspect));
  fl_set_input(fdui()->input_min_aspect, to_charstring(minAspect));
  fl_set_input(fdui()->input_max_perimeter, to_charstring(maxPerimeter));
  fl_set_button(fdui()->button_verbose, verbose?1:0);
  fl_set_button(fdui()->button_auto_redraw, redraw?1:0);
  fl_unfreeze_form(imageform()->xform());
}

AutomationDashboard::~AutomationDashboard() {
  menu->RemoveOption(GRAINGROUP);
  menu->RemoveOption(BDYGROUP);
  menu->RemoveOption(VERBOSE);
  menu->RemoveOption(REDRAW);
  menu->RemoveOption(LOWERCOLOR);
  menu->RemoveOption(UPPERCOLOR);
  menu->RemoveOption(LOCALFLAM);
  menu->RemoveOption(GLOBALFLAM);
  menu->RemoveOption(MINSELECTION);
  menu->RemoveOption(MINASPECT);
  menu->RemoveOption(MAXASPECT);
  menu->RemoveOption(MAXPERIMETER);
  delete menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AutomationDashboard::hide() {
  get_params();
  return 0;			// don't require a redraw
}

bool AutomationDashboard::get_params() {
  // Find which parameters have changed, and record the changes
  if(active()) {
    // code is separated into blocks for each input data type, so as
    // to reduce the need for temp variables
    {
      CharString temp = fl_get_input(fdui()->input_graingroup);
      if(temp != graingroup) {
	graingroup = graingroup_dflt = temp;
	garcon()->log(menucommand() + GRAINGROUP + "=" + graingroup);
      }
      
      temp = fl_get_input(fdui()->input_bdygroup);
      if(temp != bdygroup) {
	bdygroup = bdygroup_dflt = temp;
	garcon()->log(menucommand() + BDYGROUP + "=" + bdygroup);
      }
    }
    {
      Color temp;
      istrstream iss(fl_get_input(fdui()->input_minburn));
      if(iss >> temp) {
	if(temp != lowerColorBound) {
	  lowerColorBound = lowerColorBound_dflt = temp;
	  garcon()->log(menucommand() + LOWERCOLOR + "=" +
			to_charstring(lowerColorBound));
	}
      }
      else {			// failed to read color
	garcon()->msout << ms_error << "Badly formed color: "
			<< fl_get_input(fdui()->input_minburn)
			<< endl << ms_normal;
	fl_set_input(fdui()->input_minburn, to_charstring(lowerColorBound));
	return false;
      }
      istrstream iss2(fl_get_input(fdui()->input_maxburn));
      if(iss2 >> temp) {
	if(temp != upperColorBound) {
	  upperColorBound = upperColorBound_dflt = temp;
	  garcon()->log(menucommand() + UPPERCOLOR + "=" +
			to_charstring(upperColorBound));
	}
      }
      else {			// failed to read color
	garcon()->msout << ms_error << "Badly formed color: "
			<< fl_get_input(fdui()->input_maxburn)
			<< endl << ms_normal;
	fl_set_input(fdui()->input_maxburn, to_charstring(upperColorBound));
	return false;
      }
    }
    {
      int temp = atoi(fl_get_input(fdui()->input_local_flam));
      if(temp != localBurn) {
	localBurn = localBurn_dflt = temp;
	garcon()->log(menucommand() + LOCALFLAM + "=" +
		      to_charstring(localBurn));
      }
      temp = atoi(fl_get_input(fdui()->input_global_flam));
      if(temp != globalBurn) {
	globalBurn = globalBurn_dflt = temp;
	garcon()->log(menucommand() + GLOBALFLAM + "=" +
		      to_charstring(globalBurn));
      }
      temp = atoi(fl_get_input(fdui()->input_min_selection));
      if(temp != minSize) {
	minSize = minSize_dflt = temp;
	garcon()->log(menucommand() + MINSELECTION + "=" +
		      to_charstring(minSize));
      }
    }
    {
      TrueFalse temp =
	fl_get_button(fdui()->button_verbose)==1?TF_TRUE:TF_FALSE;
      if(temp != verbose) {
	verbose = verbose_dflt = temp;
	garcon()->log(menucommand() + VERBOSE + "=" + to_charstring(verbose));
      }
      temp = fl_get_button(fdui()->button_auto_redraw)==1?TF_TRUE:TF_FALSE;
      if(temp != redraw) {
	redraw = redraw_dflt = temp;
	garcon()->log(menucommand() + REDRAW + "=" + to_charstring(redraw));
      }
    }
    {
      double temp = atof(fl_get_input(fdui()->input_max_aspect));
      if(temp != maxAspect) {
	maxAspect = maxAspect_dflt= temp;
	garcon()->log(menucommand() + MAXASPECT+"="+to_charstring(maxAspect));
      }
      temp = atof(fl_get_input(fdui()->input_min_aspect));
      if(temp != minAspect) {
	minAspect = minAspect_dflt = temp;
	garcon()->log(menucommand() + MINASPECT+"="+to_charstring(minAspect));
      }
      temp = atof(fl_get_input(fdui()->input_max_perimeter));
      if(temp != maxPerimeter) {
	maxPerimeter = maxPerimeter_dflt = temp;
	garcon()->log(menucommand() + MAXPERIMETER + "=" +
		      to_charstring(maxPerimeter));
      }
    }
  }
  return true;
}

void AutomationDashboard::update_dflts(const CharString &which) const {
  // Update the default values that are used for the next GUI window.
  // Called when a variable (which) is set via the text interface.
  if(which == GRAINGROUP)
    graingroup_dflt = graingroup;
  else if(which == BDYGROUP)
    bdygroup_dflt = bdygroup;
  else if(which == LOWERCOLOR)
    lowerColorBound_dflt = lowerColorBound;
  else if(which == UPPERCOLOR)
    upperColorBound_dflt = upperColorBound;
  else if(which == LOCALFLAM)
    localBurn_dflt = localBurn;
  else if(which == GLOBALFLAM)
    globalBurn_dflt = globalBurn;
  else if(which == MINSELECTION)
    minSize_dflt = minSize;
  else if(which == VERBOSE)
    verbose_dflt = verbose;
  else if(which == REDRAW)
    redraw_dflt = redraw;
  else if(which == MAXASPECT)
    maxAspect_dflt = maxAspect;
  else if(which == MINASPECT)
    minAspect_dflt = minAspect;
  else if(which == MAXPERIMETER)
    maxPerimeter_dflt = maxPerimeter;
}

static void order(int a, int b, int &min, int &max) {
  if(a < b) { min = a; max = b; }
  else { min = b; max = a; }
}

bool AutomationDashboard::check_params() {
  if(!graingroup || !bdygroup) {
    garcon()->msout << ms_error << "You must specify group names!"
		    << endl << ms_normal;
    return false;
  }
  // make sure that the rgb values in the lower color bound are less
  // than those in the upper bound.  Use other variables for the
  // reordered colors so as not to mess up the interface.
  int minr, maxr, ming, maxg, minb, maxb;
  order(lowerColorBound.query_red(),upperColorBound.query_red(), minr, maxr);
  order(lowerColorBound.query_green(),upperColorBound.query_green(), ming,maxg);
  order(lowerColorBound.query_blue(),upperColorBound.query_blue(), minb, maxb);
  mincolor = Color(minr, ming, minb);
  maxcolor = Color(maxr, maxg, maxb);
  return true;
}

void AutomationDashboard::params_changed(const CharString &varname) { // static
  // This is called when variables are set via the text interface. The
  // value displayed in the GUI has to be updated, as well as the
  // default values used when the next GUI window is opened.
  AutomationDashboard *ad = (AutomationDashboard*) get_dashboard();
  ad->update_gui();
  ad->update_dflts(varname);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AutomationDashboard::CB_go(FL_OBJECT *obj, long) { // static
  ((AutomationDashboard*) ImageFormGfx::objdash(obj))->go_gui();
}

void AutomationDashboard::go_gui() {
  if(active()) {
    // get and log changes in params before logging the "go" command itself!
    if(get_params()) {
      garcon()->log(menucommand() + "go");
      go();
    }
  }
}

CommandFn AutomationDashboard::go_cmd() {
  ((AutomationDashboard*) get_dashboard())->go();
}

void AutomationDashboard::go() {
  if(check_params())
    current_goof->autoSelect(imageform()->current_picture(),
			     graingroup, bdygroup,
			     mincolor, maxcolor,
			     localBurn, globalBurn,
			     minSize,
			     minAspect, maxAspect, maxPerimeter,
			     verbose, redraw);
}
