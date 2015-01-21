// -*- C++ -*-
// $RCSfile: coordinates.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:27 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "coordinates.h"
#include "gfx.h"
#include "goof.h"
#include "imagecanvas.h"
#include "image_drawer2.h"
#include "imageform.h"
#include "meshcoord.h"
#include "ppm2oof.h"
#include "robomouse.h"
#include "stdlib.h"

double CoordDashboard::zoomfactor_dflt = 1.5;
double CoordDashboard::scalefactor_dflt = 1.0;

CoordDashboard::CoordDashboard(ImageForm *iform, FL_OBJECT *grp)
  : Dashboard("Coordinates", iform, grp),
    zoomfactor(zoomfactor_dflt),
    scalefactor(scalefactor_dflt)
{
  if(!textmode) {
    fl_set_input(fdui()->input_zoomfactor, to_charstring(zoomfactor));
    fl_set_input_selected(fdui()->input_zoomfactor, 1);
    fl_set_input(fdui()->input_scalefactor, to_charstring(scalefactor));
  }

  buildmenu(menu, MOPT_NORMAL);
  buildmenu(imageformmenu(), MOPT_NOPRINT); // for backwards compatibility
}

void CoordDashboard::buildmenu(Menu *carte, MenuAttribute attr) {
  carte->AddCommand("center", "show entire image", center_cmd, attr);
  carte->AddCommand("redraw", "redraw image", redraw_cmd, attr);
  AddVariable(carte, "zoomfactor", "set zoom factor", zoomfactor,
	      zoomfactor_cmd, attr);
  AddVariable(carte, "scalefactor", "set scale factor", scalefactor,
	      scalefactor_cmd, attr);
}

CoordDashboard::~CoordDashboard() {
  menu->RemoveOption("center");
  menu->RemoveOption("redraw");
  menu->RemoveOption("zoomfactor");
  menu->RemoveOption("scalefactor");
  delete menu;
}

int CoordDashboard::show() {
  canvas()->drag_pan();
  setcursor(*zoomcursor);
  return 0;
}

int CoordDashboard::hide() {
  canvas()->drag_nothing();
  unsetcursor();
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void CoordDashboard::show_range(const MeshCoord &ul, const MeshCoord &lr) {
  if(!fdui()) return;
  fl_set_object_label(fdui()->text_xmin, "xmin = " + to_charstring(ul.x));
  fl_set_object_label(fdui()->text_xmax, "xmax = " + to_charstring(lr.x));
  fl_set_object_label(fdui()->text_ymin, "ymin = " + to_charstring(lr.y));
  fl_set_object_label(fdui()->text_ymax, "ymax = " + to_charstring(ul.y));
  if(canvas()) {
    scalefactor = scalefactor_dflt =
      canvas()->pwidth()/(double)goof()->query_width();
    fl_set_input(fdui()->input_scalefactor, to_charstring(scalefactor));
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// called by forms
void CoordDashboard::CB_center(FL_OBJECT *obj, long) {
  ((CoordDashboard*) ImageFormGfx::objdash(obj))->center_gui();
}

void CoordDashboard::center_gui() {
  if(active()) {
    garcon()->log(menucommand() + "center");
    imageform()->center_image();
  }
}

// called by menus
CommandFn CoordDashboard::center_cmd() {
  get_imageform()->center_image();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void CoordDashboard::CB_redraw(FL_OBJECT *obj, long) {
  ((CoordDashboard*) ImageFormGfx::objdash(obj))->redraw_gui();
}

void CoordDashboard::redraw_gui() {
  if(active()) {
    garcon()->log(menucommand() + "redraw");
    imageform()->draw();
  }
}

CommandFn CoordDashboard::redraw_cmd() {
  get_imageform()->draw();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

double CoordDashboard::get_zoomfactor() {
  double factor = atof(fl_get_input(fdui()->input_zoomfactor));
  return fix_zoomfactor(factor);
}

double CoordDashboard::fix_zoomfactor(double factor) {
  if(factor <= 0.0) {
    factor = zoomfactor_dflt;
    fl_set_input(fdui()->input_zoomfactor, to_charstring(factor));
  }
  if(factor > 1.0)
    return factor;
  return 1./factor;
}

// called by forms
void CoordDashboard::CB_zoomfactor(FL_OBJECT *obj, long) {
   ((CoordDashboard*)(ImageFormGfx::objdash(obj)))->set_zoomfactor_gui();
}

// called by menus
void CoordDashboard::zoomfactor_cmd(const CharString &) {
  ((CoordDashboard*) get_dashboard())->set_zoomfactor();
}

void CoordDashboard::set_zoomfactor() {
  // make canvas zoomfactor and form input field agree with zoomfactor
  if(textmode) return;
  zoomfactor = fix_zoomfactor(zoomfactor);
  fl_set_input(fdui()->input_zoomfactor, to_charstring(zoomfactor));
  canvas()->setzoomfactor(zoomfactor);
}

void CoordDashboard::set_zoomfactor_gui() {
  if(active()) {
    zoomfactor = zoomfactor_dflt = get_zoomfactor();
    set_zoomfactor();
    garcon()->log(menucommand() + "zoomfactor = " + to_charstring(zoomfactor));
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void CoordDashboard::set_scalefactor(double factor) {
  if(factor <= 0.0) {
    factor = scalefactor_dflt;
    fl_set_input(fdui()->input_scalefactor, to_charstring(factor));
  }
  scalefactor = scalefactor_dflt = factor;
  canvas()->resizepixmap(goof()->query_width()*scalefactor,
			 goof()->query_height()*scalefactor);
  canvas()->setcoords(0, goof()->query_width(), 0, goof()->query_height());
  imageform()->draw();
}

void CoordDashboard::CB_input_scalefactor(FL_OBJECT *obj, long) {
  ((CoordDashboard*)(ImageFormGfx::objdash(obj)))->set_scalefactor_gui();
}

void CoordDashboard::set_scalefactor_gui() {
  if(active()) {
    double factor = atof(fl_get_input(fdui()->input_scalefactor));
    set_scalefactor(factor);
    garcon()->log(menucommand() + "scalefactor = "
		  + to_charstring(scalefactor));
  }
}

void CoordDashboard::scalefactor_cmd(const CharString&) {
  ((CoordDashboard*) get_dashboard())->set_scalefactor();
}

void CoordDashboard::set_scalefactor() {
  if(textmode) return;
  fl_set_input(fdui()->input_scalefactor, to_charstring(scalefactor));
  set_scalefactor(scalefactor);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void CoordDashboard::clickhandler(const MouseClick &click) {
  const MeshCoord &where = click.coord;
  if(click.button == 1)
    canvas()->zoom(1./get_zoomfactor(), canvas()->xpoint(where.x, where.y));
  else if(click.button == 3)
    canvas()->zoom(get_zoomfactor(), canvas()->xpoint(where.x, where.y));
}
