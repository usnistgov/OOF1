// -*- C++ -*-
// $RCSfile: selectdash.h,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-09-06 20:11:47 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef SELECTDASH_H
#define SELECTDASH_H_H

#include "burn.h"
#include "dashboard.h"
#include "enum.h"
#include "menuDef.h"
#include "rubberband.h"
#include "select_by_color.h"

class MouseClick;
class RepeatSelectCmd;
class DemoGrayfyCmd;

enum SelectionMethod {
  SM_BURN,
  SM_RGBBURN,
  SM_DEMOGRAPHY,
  SM_PIXEL
};

class SelectDashboard : public Dashboard {
private:
  SelectDashboard(ImageForm*, FL_OBJECT*);
  virtual ~SelectDashboard();
  virtual int show();
  virtual int hide();

  void buildmenu(Menu*, MenuAttribute);

  void setbuttons();
  void setrubberband();

  static void CB_select(FL_OBJECT*, long);
  Enum<SelectionMethod> method;
  static Enum<SelectionMethod> method_dflt;
  void set_selectmethod_gui(Enum<SelectionMethod>);
  static void set_selectmethod_cmd(const CharString&);
  void set_selectmethod(Enum<SelectionMethod>&);
  void set_selectmethod();

  virtual void clickhandler(const MouseClick&);
  virtual void draghandler(const MouseClick&, const MouseClick&);
  void clickhandler(const Cell_coordinate&, int);
  void triangleclickhandler(const MeshCoord&, int) const;

  RepeatSelectCmd *repeatselectcmd;

  void repeatselection();
  static void CB_repeat(FL_OBJECT*, long);
  void repeat_gui();
  friend class RepeatSelectCmd;

  // the burncmd needs a pointer to the burner, so it has to come first
  Burn burner;
  BurnCmd *burncmd;
  static void CB_burn_params(FL_OBJECT*, long);
  void set_burn_params_gui();

  // the burncmd needs a pointer to the burner, so it has to come first
  RGBBurn rgbburner;
  RGBBurnCmd *rgbburncmd;
  static void CB_rgbburn_params(FL_OBJECT*, long);
  void set_rgbburn_params_gui();

  int dev_gray;
  static int dev_gray_dflt;
  DemogCmd *demographycmd;
  static void CB_demography_params(FL_OBJECT*, long);
  void set_demography_params_gui();
  friend class DemogCmd;

  static Enum<RubberBandType> pixeldragshape_dflt;
  FL_OBJECT *choice_pixeldragshape;
  static void CB_pixeldragshape(FL_OBJECT*, long);
  void pixeldragshape_gui();
  static void set_pixeldragshape_cmd(const CharString&);
  void set_pixeldragshape();

  DemograyfyCmd *demograyfycmd;
  static void CB_demograyfy(FL_OBJECT*, long);
  void demograyfy_gui();

  DemoRGBCmd *demorgbcmd;
  static void CB_RGB(FL_OBJECT*, long);
  void demorgb_gui();

  void undo_selection();
  static void CB_undo_selection(FL_OBJECT*, long);
  void undo_selection_gui();
  static CommandFn undo_selection_cmd();

  void redo_selection();
  static void CB_redo_selection(FL_OBJECT*, long);
  void redo_selection_gui();
  static CommandFn redo_selection_cmd();

  void select_none();
  static void CB_select_none(FL_OBJECT*, long);
  void select_none_gui();
  static CommandFn select_none_cmd();

  void select_complement();
  static void CB_select_complement(FL_OBJECT*, long);
  void select_complement_gui();
  static CommandFn select_complement_cmd();

  void select_info();
  static void CB_select_info(FL_OBJECT*, long);
  void select_info_gui();
  static CommandFn select_info_cmd();

  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
  friend FD_image_drawer *create_form_image_drawer();
};

#endif
