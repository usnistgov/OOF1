// -*- C++ -*-
// $RCSfile: distort.C,v $
// $Revision: 1.11 $
// $Author: langer $
// $Date: 2000-11-15 19:41:52 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// apply distortions and forces to groups of nodes

#include "distort.h"
#include "grid.h"
#include "lookfor.h"
#include "menuDef.h"
#include "nodegroups.h"
#include "parameters.h"
#include "mvmd.h"
#include "tocharstring.h"
#include "truefalse.h"
#include "twotwo.h"
#include <iostream.h>
#include <math.h>

static Menu *edistortmenu();
#ifdef THERMAL
static Menu *tdistortmenu();
#endif // THERMAL

// --------------------------------------------------------------- //

static TwoTwo dm;   // distortion matrix
static MeshCoord shift;
static MeshCoord applied_force;
static TrueFalse inc(1); // apply absolute (F) or incremental (T) distortion?

#ifdef THERMAL
static ThermalDistortion thermaldistortion;
static TrueFalse tinc(1);
static const ThermalDistortion zerothermaldistortion;
#endif // THERMAL

// --------------------------------------------------------------- //


// apply distortion

static CommandFn distort_group() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    if(inc) {
      n->incremental_distortion = dm;
      n->incremental_shift = shift;
      n->incremental_force = applied_force;
    }
    else {
      n->absolute_distortion = dm;
      n->absolute_shift = shift;
      n->absolute_force = applied_force;
      grid->apply_absolute_elastic_distortion();
//        grid->distort(dm, shift, applied_force, n);
      grid->redraw();
    }
    //    grid->distortcmds->record();
  }
}

static CommandFn apply_elastic_increment() {
  if(gridinit()) {
    grid->apply_incremental_elastic_distortion();
    grid->redraw();
  }
}

static CommandFn invisible_elastic_increment() {
  if(gridinit())
    grid->apply_incremental_elastic_distortion();
}

CommandFn clear_elastic_distortion() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->clear_elastic_distortion();
    grid->apply_absolute_elastic_distortion();
    grid->redraw();
  }
}

CommandFn clear_elastic_distortions() {
  if(gridinit()) {
    grid->clear_elastic_distortion();
    grid->redraw();
  }
}

static CommandFn reset() {
  dm(0, 0) = dm(0, 1) = dm(1, 0) = dm(1, 1) = 0.0;
  shift.x = shift.y = 0.0;
  applied_force.x = applied_force.y = 0.0;
  edistortmenu()->update_forms();
}

#ifdef THERMAL

static CommandFn tdistort_group() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    if(tinc)
      n->incremental_thermaldistortion = thermaldistortion;
    else {
      n->absolute_thermaldistortion = thermaldistortion;
      grid->apply_absolute_thermal_distortion();
      grid->redraw();
    }
  }
}

static CommandFn apply_increment() {
  if(gridinit()) {
    grid->apply_incremental_elastic_distortion();
    grid->apply_incremental_thermal_distortion();
    grid->redraw();
  }
}

static CommandFn apply_thermal_increment() {
  if(gridinit()) {
    grid->apply_incremental_thermal_distortion();
    grid->redraw();
  }
}

static CommandFn invisible_increment() {
  if(gridinit()) {
    grid->apply_incremental_elastic_distortion();
    grid->apply_incremental_thermal_distortion();
  }
}

static CommandFn invisible_thermal_increment() {
  if(gridinit())
    grid->apply_incremental_thermal_distortion();
}

CommandFn clear_thermal_distortion() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->clear_thermal_distortion();
    grid->apply_absolute_thermal_distortion();
    grid->redraw();
  }
}

CommandFn clear_all_distortion() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->clear_elastic_distortion();
    grid->apply_absolute_elastic_distortion();
    n->clear_thermal_distortion();
    grid->apply_absolute_thermal_distortion();
    grid->redraw();
  }
}

CommandFn clear_thermal_distortions() {
  if(gridinit()) {
    grid->clear_thermal_distortion();
    grid->redraw();
  }
}
 
CommandFn clear_all_distortions() {
  if(gridinit()) {
    grid->clear_elastic_distortion();
    grid->clear_thermal_distortion();
    grid->redraw();
  }
}

static CommandFn treset() {
  thermaldistortion = zerothermaldistortion;
  tdistortmenu()->update_forms();
}

#endif // THERMAL

CommandFn show_distortion() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    garcon()->msout << "Incremental distortion" << endl
		    << n->incremental_distortion << endl
		    << "Incremental shift" << endl
		    << n->incremental_shift.x << " "
		    << n->incremental_shift.y << endl
		    << "Incremental force" << endl
		    << n->incremental_force.x << " " << n->incremental_force.y
		    << endl
		    << "Absolute distortion" << endl
		    << n->absolute_distortion << endl
		    << "Absolute shift" << endl
		    << n->absolute_shift.x << " " << n->absolute_shift.y
		    << endl
		    << "Absolute force" << endl
		    << n->absolute_force.x << " "
		    << n->absolute_force.y << endl;
#ifdef THERMAL
    garcon()->msout << "Absolute thermal distortion" << endl
		    << n->absolute_thermaldistortion << endl
		    << "Incremental thermal distortion" << endl
		    << n->incremental_thermaldistortion << endl;
#endif // THERMAL
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static Menu *edistortmenu() {
  static Menu *dismenu = 0;
  if(dismenu) return dismenu;
  dismenu = new GroupMenu<Node>("set", "set distortion for group",
				distort_group);
  AddVariable(dismenu, "xstrain", "strain in x direction", dm(0, 0));
  AddVariable(dismenu, "ystrain", "strain in y direction", dm(1, 1));
  AddVariable(dismenu, "xshift", "shift in x direction", shift.x);
  AddVariable(dismenu, "yshift", "shift in y direction", shift.y);
  AddVariable(dismenu, "xshear", "shear in x direction", dm(0, 1));
  AddVariable(dismenu, "yshear", "shear in y direction", dm(1, 0));
  AddVariable(dismenu, "xforce", "force in x direction", applied_force.x);
  AddVariable(dismenu, "yforce", "force in y direction", applied_force.y);
  AddVariable(dismenu, "incremental", "absolute or incremental?", inc);
  dismenu->AddCommand("reset_params", "clear the current distortion matrix",
		      reset);
//    dismenu->AddCommand("all_nodes", "set distortion for all nodes", distort_all);
//    dismenu->AddCommand("boundaries", "set distortion for boundary nodes",
//  			  distort_edges);
  return dismenu;
}

#ifdef THERMAL
static Menu *tdistortmenu() {
  static Menu *dismenu = 0;
  if(dismenu) return dismenu;
  dismenu = new GroupMenu<Node>("set", "set the thermal distortion for group",
				tdistort_group);
  AddVariable(dismenu, "T0", "temperature at x=y=0",
	      thermaldistortion.T0);
  AddVariable(dismenu, "dT/dx", "temperature derivative in x direction",
	      thermaldistortion.dTdx);
  AddVariable(dismenu, "dT/dy", "temperature derivative in y direction",
	      thermaldistortion.dTdy);
  AddVariable(dismenu, "heatrate", "appied heat deposition rate",
	      thermaldistortion.heatrate);
  AddVariable(dismenu, "incremental", "absolute or incremental?", tinc);
  dismenu->AddCommand("reset_params", "clear the current distortion", treset);
//    dismenu->AddCommand("all_nodes", "set distortion for all nodes",
//  		      tdistort_all);
//    dismenu->AddCommand("boundaries", "set distortion for boundary nodes",
//  		      tdistort_edges);
  return dismenu;
}

Menu *distmenu() {
  static Menu *dmenu = 0;
  if(dmenu) return dmenu;
  dmenu = new Menu("distort");    

  // Things acting on all distortions (elastic and thermal)
  Menu *clearmenu = dmenu->AddSubMenu("clear",
			     "remove all distortions from groups of nodes",
       new GroupMenu<Node>("clear", "remove all distortions for group",
				   clear_all_distortion));
  clearmenu->AddCommand("all", "remove all distortions", clear_all_distortions);
  dmenu->AddCommand("increment", "apply incremental distortions",
		   apply_increment);
  dmenu->AddCommand("stealth_increment", "apply increment without redrawing", 
		   invisible_increment);

  Menu *sm = dmenu->AddSubMenu("show", "show distortion",
      new GroupMenu<Node>("show", "show distortion for group",
			  show_distortion));


  // Elastic distortions
  Menu *emenu = dmenu->AddSubMenu("elastic", "set elastic distortion");
  emenu->AddCommand("increment", "apply incremental elastic distortions",
		    apply_elastic_increment);
  emenu->AddCommand("stealth_increment",
		    "apply incremental without redrawing",
		    invisible_elastic_increment);
  emenu->AddSubMenu("set", "set the current distortion", edistortmenu());
  Menu *dclearmenu = emenu->AddSubMenu("clear",
			     "remove elastic distortions from groups of nodes",
       new GroupMenu<Node>("clear", "remove elastic distortions for group",
				   clear_elastic_distortion));

  dclearmenu->AddCommand("all", "remove all distortions",
			 clear_elastic_distortions);

  // Thermal distortions
  Menu *tmenu = dmenu->AddSubMenu("thermal", "set thermal distortion");
  tmenu->AddCommand("increment", "apply incremental thermal distortions",
		    apply_thermal_increment);
  tmenu->AddCommand("stealth_increment",
		    "apply incremental without redrawing",
		    invisible_thermal_increment);
  tmenu->AddSubMenu("set", "set the current thermal distortion",
		    tdistortmenu());
  Menu *tclearmenu = tmenu->AddSubMenu("clear",
			     "remove thermal distortions from groups of nodes",
       new GroupMenu<Node>("clear", "remove thermal distortions for group",
				   clear_thermal_distortion));

  tclearmenu->AddCommand("all", "remove all distortions",
			 clear_thermal_distortions);

  return dmenu;
}
#endif // THERMAL

#ifndef THERMAL
Menu *distmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("distort");    
  
  menu->AddSubMenu("set", "set the current distortion", edistortmenu());
  Menu *clearmenu = menu->AddSubMenu("clear",
				     "remove distortions from groups of nodes",
	       new GroupMenu<Node>("clear", "remove distortions for group",
				   clear_elastic_distortion));

  clearmenu->AddCommand("all", "remove all distortions",
			clear_elastic_distortions);

  menu->AddCommand("increment", "apply incremental distortions",
		   apply_elastic_increment);
  menu->AddCommand("stealth_increment", "apply increment without redrawing", 
		   invisible_elastic_increment);

  menu->AddSubMenu("show", "show the incremental distortion matrix",
      new GroupMenu<Node>("show", "show incremental distortion for group",
			  show_distortion));
  return menu;
}

#endif // !THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef THERMAL
ThermalDistortion::operator bool() const {
  return (dTdx != 0.0 || dTdy != 0.0 || T0 != 0.0 || heatrate != 0.0);
}

const ThermalDistortion
&ThermalDistortion::operator+=(const ThermalDistortion &td)
{
  dTdx += td.dTdx;
  dTdy += td.dTdy;
  T0 += td.T0;
  heatrate += td.heatrate;
  return *this;
}

ostream &operator<<(ostream &os, const ThermalDistortion &td) {
  os << "T0: " << td.T0 << " dTdx: " << td.dTdx << " dTdy: " << td.dTdy
     << " heatrate: " << td.heatrate;
  return os;
}

istream &operator>>(istream &is, ThermalDistortion &td) {
  if(!lookfor("T0:", is)) return is;
  is >> td.T0;
  if(!lookfor("dTdx:", is)) return is;
  is >> td.dTdx;
  if(!lookfor("dTdy:", is)) return is;
  is >> td.dTdy;
  if(!lookfor("heatrate:", is)) return is;
  is >> td.heatrate;
  return is;
}

void ThermalDistortion::print(FILE *file) const {
  fprintf(file, "T0: %e dTdx: %e dTdy: %e heatrate: %e",
	  T0, dTdx, dTdy, heatrate);
}
#endif // THERMAL
