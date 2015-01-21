// -*- C++ -*-
// $RCSfile: coorddashboard.C,v $
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

#include "colorutils.h"
#include "create_form_drawer.h"
#include "coorddashboard.h"
#include "delete_fl_group.h"
#include "formdrawer.h"
#include "graphics.h"
#include "meshcoord.h"
#include "oofcanvas.h"
#include "robomouse.h"

double CoordDashboard::zoomfactor_dflt = 1.5;

CoordDashboard::CoordDashboard(Drawer *drwr)
  : Dashboard("Coordinates", drwr, RB_RECTANGLE),
    zoomfactor(zoomfactor_dflt)
{
  create_form_coordinates(this);
  fl_set_input(zoomfactor_input, to_charstring(zoomfactor));
  fl_set_input_selected(zoomfactor_input, 1);

  menu.AddCommand("center", "show entire image", CoordDashboard::center_cmd);
  menu.AddCommand("redraw", "redraw image", CoordDashboard::redraw_cmd);
  AddVariable(&menu, "zoomfactor", "set zoom factor", zoomfactor,
	      zoomfactor_cmd);
  canvas()->setzoomfactor(zoomfactor_dflt);
}

CoordDashboard::~CoordDashboard() {
  hide();
  delete_fl_group(coordinates);
  /*
  fl_delete_object(coordinates);
  fl_free_object(coordinates);
  fl_free_object(zoomfactor_input);
  fl_free_object(center_button);
  fl_free_object(redraw_button);
  fl_free_object(xmin_text);
  fl_free_object(xmax_text);
  fl_free_object(ymin_text);
  fl_free_object(ymax_text);
  fl_free_object(cliptext);
  fl_free_object(zoomtext0);
  fl_free_object(zoomtext1);
  fl_free_object(zoomtext2);
  fl_free_object(zoomtext3);
  */
  menu.RemoveOption("center");
  menu.RemoveOption("redraw");
  menu.RemoveOption("zoomfactor");
}

void CoordDashboard::show() {
  setcursor(*zoomcursor);
  canvas()->do_scrollcallback(); // so that coords are up to date
  fl_show_object(coordinates);
  formdrawer()->drag_pan();
}

void CoordDashboard::hide() {
  unsetcursor();
  fl_hide_object(coordinates);
  formdrawer()->drag_nothing();
}

void CoordDashboard::set_coord_text(const MeshCoord &ul, const MeshCoord &lr)
     const
{
  if(!xmin_text) return; // XForms objects not constructed yet
  fl_set_object_label(xmin_text, "xmin = " + to_charstring(ul.x));
  fl_set_object_label(xmax_text, "xmax = " + to_charstring(lr.x));
  fl_set_object_label(ymin_text, "ymin = " + to_charstring(lr.y));
  fl_set_object_label(ymax_text, "ymax = " + to_charstring(ul.y));
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// The zoomfactor is always greater than one. To ensure this, and to
// ensure that the current value in the form is the one used, all
// routines other than the ones that set the value use
// get_zoomfactor().

double CoordDashboard::get_zoomfactor() {
  double factor = atof(fl_get_input(zoomfactor_input));
  return fix_zoomfactor(factor);
}

double CoordDashboard::fix_zoomfactor(double factor) {
  if(factor <= 0.0) {
    factor = zoomfactor_dflt;
    fl_set_input(zoomfactor_input, to_charstring(zoomfactor_dflt));
  }
  if(factor > 1.0)
    return factor;
  return 1./factor;
}

void CoordDashboard::set_zoomfactor_gui() {
  if(active()) {
    zoomfactor = zoomfactor_dflt = get_zoomfactor();
    garcon()->log(menucommand() + "zoomfactor = " + to_charstring(zoomfactor));
    canvas()->setzoomfactor(zoomfactor);
  }
}

void CoordDashboard::set_zoomfactor() {
  zoomfactor = zoomfactor_dflt = fix_zoomfactor(zoomfactor);
  fl_set_input(zoomfactor_input, to_charstring(zoomfactor));
  canvas()->setzoomfactor(zoomfactor);
}

void FDCB_zoomfactor(FL_OBJECT*, long ptr) {
  ((CoordDashboard*) ptr)->set_zoomfactor_gui();
}

void CoordDashboard::zoomfactor_cmd(const CharString &) {
  ((CoordDashboard*) get_dashboard())->set_zoomfactor();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void CoordDashboard::center() {
  if(active()) {
    garcon()->log(menucommand() + "center");
    formdrawer()->center_image();
  }
}

void FDCB_center(FL_OBJECT*, long ptr) {
  ((CoordDashboard*) ptr)->center();
}

CommandFn CoordDashboard::center_cmd() {
  get_formdrawer()->center_image();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void CoordDashboard::redraw() {
  if(active()) {
    garcon()->log(menucommand() + "redraw");
    formdrawer()->redraw_image();
  }
}

void FDCB_redraw(FL_OBJECT*, long ptr) {
  ((CoordDashboard*) ptr)->redraw();
}

CommandFn CoordDashboard::redraw_cmd() {
  get_formdrawer()->redraw_image();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void CoordDashboard::clickhandler(const MouseClick &click) {
  const MeshCoord &where = click.coord;
  if(click.button == 1)
    canvas()->zoom(1./get_zoomfactor(), canvas()->xpoint(where.x, where.y));
  else if(click.button == 3)
    canvas()->zoom(get_zoomfactor(), canvas()->xpoint(where.x, where.y));
}
