// -*- C++ -*-
// $RCSfile: coordinates.h,v $
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

#ifndef COORDINATES_H
#define COORDINATES_H

#include "dashboard.h"
#include "menuDef.h"

class MeshCoord;

class CoordDashboard : public Dashboard {
private:
  CoordDashboard(ImageForm*, FL_OBJECT*);
  virtual ~CoordDashboard();
  virtual int show();
  virtual int hide();

  void buildmenu(Menu*, MenuAttribute);

  static void CB_center(FL_OBJECT*, long); // called by forms
  static CommandFn center_cmd(); // called by menus
  void center_gui();

  static double zoomfactor_dflt;
  double zoomfactor;
  static void CB_zoomfactor(FL_OBJECT*, long); // called by forms
  static void zoomfactor_cmd(const CharString&); // called by menus
  void set_zoomfactor_gui();
  void set_zoomfactor(); // sets form input & canvas
  double get_zoomfactor(); // reads from form
  double fix_zoomfactor(double); // makes sure it's > 1

  static double scalefactor_dflt;
  double scalefactor;
  void set_scalefactor(double);
  static void CB_input_scalefactor(FL_OBJECT*, long);
  static void scalefactor_cmd(const CharString&);
  void set_scalefactor_gui();
  void set_scalefactor();

  static void CB_redraw(FL_OBJECT*, long); 
  static CommandFn redraw_cmd();
  void redraw_gui();

  virtual void clickhandler(const MouseClick&);

  void show_range(const MeshCoord&, const MeshCoord&);

  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
  friend FD_image_drawer *create_form_image_drawer();
};

#endif
