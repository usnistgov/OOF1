// -*- C++ -*-
// $RCSfile: coorddashboard.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:50 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef COORDDASHBOARD_H
#define COORDDASHBOARD_H

#include "charstring.h"
#include "dashboard.h"
#include "forms.h"
#include "menuDef.h"

class CoordDashboard : public Dashboard {
private:
  CoordDashboard(Drawer *drwr);
  ~CoordDashboard();

  static double zoomfactor_dflt;
  double zoomfactor;

  // XForms objects
  FL_OBJECT *coordinates;
  FL_OBJECT *zoomfactor_input;
  FL_OBJECT *center_button;
  FL_OBJECT *redraw_button;
  FL_OBJECT *xmin_text;
  FL_OBJECT *xmax_text;
  FL_OBJECT *ymin_text;
  FL_OBJECT *ymax_text;
  FL_OBJECT *cliptext;
  FL_OBJECT *zoomtext2;
  FL_OBJECT *zoomtext3;
  FL_OBJECT *zoomtext1;
  FL_OBJECT *zoomtext0;

  // callback routines for buttons, etc.
  void center();
  friend void FDCB_center(FL_OBJECT*, long);
  static CommandFn center_cmd();

  double get_zoomfactor();
  double fix_zoomfactor(double);
  void set_zoomfactor();
  void set_zoomfactor_gui();
  friend void FDCB_zoomfactor(FL_OBJECT*, long);
  static void zoomfactor_cmd(const CharString&);

  void redraw();
  friend void FDCB_redraw(FL_OBJECT*, long);
  static CommandFn redraw_cmd();

  void set_coord_text(const MeshCoord&, const MeshCoord&) const;

  virtual void show();
  virtual void hide();
  virtual void clickhandler(const MouseClick&);

  //  double zoomamount();
  friend class FormDrawer;
  friend class MeshDrawer;
  friend void create_form_coordinates(CoordDashboard*);
};   

#endif
