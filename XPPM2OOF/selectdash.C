// -*- C++ -*-
// $RCSfile: selectdash.C,v $
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

#include "burn.h"
#include "gfx.h"
#include "goof.h"
#include "imagecanvas.h"
#include "imageform.h"
#include "menuDef.h"
#include "meshcmds.h"
#include "ppm2oof.h"
#include "rubberband.h"
#include "selectdash.h"
#include "select_by_color.h"
#include "selectpixel.h"

Enum<SelectionMethod> SelectDashboard::method_dflt(SM_BURN);
Enum<RubberBandType> SelectDashboard::pixeldragshape_dflt(RB_RECTANGLE);

class RepeatSelectCmd  : public CommandM {
private:
  Cell_coordinate click;
  int shiftkey;
public:
  RepeatSelectCmd();
  virtual CommandFn func();
  virtual CommandM *clone() const;
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


SelectDashboard::SelectDashboard(ImageForm *iform, FL_OBJECT *grp)
  : Dashboard("Select", iform, grp, pixeldragshape_dflt),
    method(method_dflt),
    burncmd(new BurnCmd(&burner)),
    demographycmd(new DemogCmd(this)),
    demograyfycmd(new DemograyfyCmd(iform)),
    demorgbcmd(new DemoRGBCmd(iform)),
    rgbburncmd(new RGBBurnCmd(&rgbburner)),
    repeatselectcmd(new RepeatSelectCmd),
    dev_gray(dev_gray_dflt)
{
  static int dothisonce = 1;
  if(dothisonce) {
    dothisonce = 0;
    Enum<SelectionMethod>::name(SM_BURN, "burn");
    Enum<SelectionMethod>::name(SM_RGBBURN, "rgb_burn");
    Enum<SelectionMethod>::name(SM_DEMOGRAPHY, "demography");
    Enum<SelectionMethod>::name(SM_PIXEL, "pixel");

    Enum<RubberBandType>::name(RB_NONE, "none");
    Enum<RubberBandType>::name(RB_RECTANGLE, "rectangle");
    Enum<RubberBandType>::name(RB_CIRCLE, "circle");
    Enum<RubberBandType>::name(RB_ELLIPSE, "ellipse");
  }

  if(!textmode) {
    fl_set_button(fdui()->button_burn, 0);
    fl_set_button(fdui()->button_rgbburn, 0);
    fl_set_button(fdui()->button_demography, 0);
    fl_set_button(fdui()->button_pixel, 0);
    setbuttons();
    // Create pulldown menu for pixel selection drag shapes.
    fl_addto_group(grp);
    choice_pixeldragshape = fl_add_choice(FL_NORMAL_CHOICE,
					  fdui()->box_pixelparams->x,
					  fdui()->box_pixelparams->y,
					  fdui()->box_pixelparams->w,
					  fdui()->box_pixelparams->h, "");
    fl_set_object_gravity(choice_pixeldragshape, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(choice_pixeldragshape, CB_pixeldragshape, 0);
    fl_set_object_boxtype(choice_pixeldragshape, FL_DOWN_BOX);
    fl_delete_object(fdui()->box_pixelparams);
    fl_free_object(fdui()->box_pixelparams);
    fl_end_group();

    // Put names in pulldown menu for pixel drag shapes. This is like
    // what's done in EnumVarObject in enum.h.  The EnumVarObject can't
    // simply be used to put a Variable on a Form outside of a menu,
    // unfortunately.
    for(int i=0; i<rubberbandtype.names().capacity(); i++)
      fl_addto_choice(choice_pixeldragshape,
		      rubberbandtype.names()[i]->charstar());
    fl_set_choice(choice_pixeldragshape, rubberbandtype+1);

    goof()->set_imageform_undoredo_buttons();
  }
  buildmenu(menu, MOPT_NORMAL);
  buildmenu(imageformmenu(), MOPT_NOPRINT); // for backwards compatibility
}

void SelectDashboard::buildmenu(Menu *carte, MenuAttribute attr) {
  AddVariable(carte, "method", "set selection method", method,
	      set_selectmethod_cmd, attr);
  AddVariable(carte, "pixeldragshape", "shape for drag-selecting pixels",
	      rubberbandtype, set_pixeldragshape_cmd, attr);
  carte->AddCommand(repeatselectcmd, attr);
  carte->AddCommand(burncmd, attr);
  carte->AddCommand(rgbburncmd, attr);
  carte->AddCommand(demographycmd, attr);
  carte->AddCommand(demograyfycmd, attr);
  carte->AddCommand(demorgbcmd, attr);
//  carte->AddCommand(&pixelparamcmd);
  carte->AddCommand("undo", "undo last selection", undo_selection_cmd, attr);
  carte->AddCommand("redo", "redo last selection", redo_selection_cmd, attr);
  carte->AddCommand("select_none", "unselect all pixels", select_none_cmd,
		    attr);
  carte->AddCommand("select_complement", "toggle all selections",
 		     select_complement_cmd, attr);
  carte->AddCommand("select_info", "get information on current selection",
		    select_info_cmd, attr);
}

SelectDashboard::~SelectDashboard() {
  menu->RemoveOption("method");
  menu->RemoveOption(*repeatselectcmd);
  menu->RemoveOption(*burncmd); // deletes BurnCmd
  menu->RemoveOption(*rgbburncmd);
  menu->RemoveOption(*demographycmd);
  menu->RemoveOption(*demograyfycmd);
  menu->RemoveOption(*demorgbcmd);
  menu->RemoveOption("undo");
  menu->RemoveOption("redo");
  menu->RemoveOption("pixeldragshape");
  delete menu;
}

void SelectDashboard::setbuttons() {
  if(textmode) return;
  if(method == SM_BURN)
    fl_set_button(fdui()->button_burn, 1);
  else if(method == SM_RGBBURN)
    fl_set_button(fdui()->button_rgbburn, 1);
  else if(method == SM_DEMOGRAPHY)
    fl_set_button(fdui()->button_demography, 1);
  else if(method == SM_PIXEL)
    fl_set_button(fdui()->button_pixel, 1);
}

void SelectDashboard::setrubberband() {
  if(textmode) return;
  if(method == SM_PIXEL)
    canvas()->drag_rubberband();
  else
    canvas()->drag_nothing();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int SelectDashboard::show() {
  setrubberband();
  setcursor(*checkcursor);
  return 0;
}

int SelectDashboard::hide() {
  // turn off rubberband & revert cursor
  canvas()->drag_nothing();
  unsetcursor();
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::set_selectmethod(Enum<SelectionMethod> &m) {
  method = method_dflt = m;
  setrubberband();
}

void SelectDashboard::CB_select(FL_OBJECT *obj, long arg) {
  Enum<SelectionMethod> m((SelectionMethod) arg);
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->set_selectmethod_gui(m);
}

void SelectDashboard::set_selectmethod_gui(Enum<SelectionMethod> m) {
  if(active()) {
    garcon()->log(menucommand() + "method=" + to_charstring(m));
    set_selectmethod(m);
  }
}

void SelectDashboard::set_selectmethod_cmd(const CharString&) {
  ((SelectDashboard*) get_dashboard())->set_selectmethod();
}

void SelectDashboard::set_selectmethod() {
  set_selectmethod(method);
  setbuttons();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::clickhandler(const MouseClick &click) {
  if(textmode) return;
  Cell_coordinate where(canvas()->mouselocation(click));
  if( where.x < 0 || where.x >= goof()->query_width() ||
      where.y < 0 || where.y >= goof()->query_height()) return;
  fl_set_input(fdui()->input_selectclick, to_charstring(where));
  clickhandler(where, click.shiftkey);
  fl_set_input(fdui()->input_selectclick, to_charstring(where));
  fl_set_button(fdui()->button_repeat_shifted, click.shiftkey);
}

void SelectDashboard::clickhandler(const Cell_coordinate &where, int shiftkey)
{
  if(textmode) return;
  switch(method) {
  case SM_BURN:
    burner.ignite(imageform()->current_picture(), where, shiftkey);
    garcon()->msout << ms_info << "Burned " << burner.nburned()
		    << " pixels." << endl << ms_normal;
    break;
  case SM_RGBBURN:
    rgbburner.ignite(imageform()->current_picture(), where, shiftkey);
    garcon()->msout << ms_info << "Burned " << rgbburner.nburned() << " pixels."
		    << endl << ms_normal;
    break;
  case SM_DEMOGRAPHY:
    demographycmd->select(imageform()->current_picture(), where, shiftkey);
    break;
  case SM_PIXEL:
    select_pixel(where, shiftkey);
    break;
  }
  goof()->redraw();
}

void SelectDashboard::draghandler(const MouseClick &dn, const MouseClick &up)
{
  if(textmode) return;
  // Don't check that clicks are inside image -- dragging outside
  // image is ok. Only the region inside the image will be used.
  if(method == SM_PIXEL) {
    //    Cell_coordinate upwhere(canvas()->mouselocation(up));
    //    Cell_coordinate dnwhere(canvas()->mouselocation(dn));
    select_pixels(up.coord, dn.coord, dn.shiftkey, rubberbandtype);
  }
  goof()->redraw();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::repeatselection() {
  if(textmode) return;
  const char *clickstr(fl_get_input(fdui()->input_selectclick));
  istrstream buf(clickstr, strlen(clickstr));
  Cell_coordinate where;
  buf >> where;
  if(!buf) return;
  int shifted = fl_get_button(fdui()->button_repeat_shifted);
  clickhandler(where, shifted);
}

void SelectDashboard::CB_repeat(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->repeat_gui();
}

void SelectDashboard::repeat_gui() {
  if(active()) {
    garcon()->log(menucommand() + "repeat click=" +
		  fl_get_input(fdui()->input_selectclick) +
		  " shiftkey=" +
		  to_charstring(fl_get_button(fdui()->button_repeat_shifted)));
    repeatselection();
  }
}

RepeatSelectCmd::RepeatSelectCmd()
  : CommandM("repeat", "repeat last mouse click"),
    click(0,0), shiftkey(0)
{
  AddArgument(this, "click", click);
  AddArgument(this, "shiftkey", shiftkey);
}

CommandFn RepeatSelectCmd::func() {
  ((SelectDashboard*) get_dashboard())->clickhandler(click, shiftkey);
}

CommandM *RepeatSelectCmd::clone() const {
  RepeatSelectCmd *rsc = new RepeatSelectCmd;
  rsc->click = click;
  rsc->shiftkey = shiftkey;
  return rsc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::undo_selection() {
  goof()->undo_selection();
  goof()->redraw();
}

void SelectDashboard::undo_selection_gui() {
  if(active()) {
    undo_selection();
    garcon()->log(menucommand() + "undo");
  }
}

void  SelectDashboard::CB_undo_selection(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->undo_selection_gui();
}

CommandFn  SelectDashboard::undo_selection_cmd() {
  ((SelectDashboard*) get_dashboard())->undo_selection();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::redo_selection() {
  goof()->redo_selection();
  goof()->redraw();
}

void SelectDashboard::redo_selection_gui() {
  if(active()) {
    redo_selection();
    garcon()->log(menucommand() + "redo");
  }
}

void  SelectDashboard::CB_redo_selection(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->redo_selection_gui();
}

CommandFn  SelectDashboard::redo_selection_cmd() {
  ((SelectDashboard*) get_dashboard())->redo_selection();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::select_none() {
  goof()->unselect_all();
  goof()->redraw();
}

void SelectDashboard::select_none_gui() {
  if(active()) {
    select_none();
    garcon()->log(menucommand() + "select_none");
  }
}

void  SelectDashboard::CB_select_none(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->select_none_gui();
}

CommandFn  SelectDashboard::select_none_cmd() {
  ((SelectDashboard*) get_dashboard())->select_none();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::select_complement() {
  goof()->select_complement();
  goof()->redraw();
}

void SelectDashboard::select_complement_gui() {
  if(active()) {
    select_complement();
    garcon()->log(menucommand() + "select_complement");
  }
}

void  SelectDashboard::CB_select_complement(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->select_complement_gui();
}

CommandFn  SelectDashboard::select_complement_cmd() {
  ((SelectDashboard*) get_dashboard())->select_complement();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::select_info() {
  goof()->selection_info();
}

void SelectDashboard::select_info_gui() {
  if(active()) {
    select_info();
    garcon()->log(menucommand() + "select_info");
  }
}

void SelectDashboard::CB_select_info(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->select_info_gui();
}

CommandFn SelectDashboard::select_info_cmd() {
  ((SelectDashboard*) get_dashboard())->select_info();
}
//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::CB_burn_params(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->set_burn_params_gui();
}

void SelectDashboard::set_burn_params_gui() {
  WordList path(menucommand());
  burncmd->makecmdform(path)->show(0);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::CB_rgbburn_params(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->set_rgbburn_params_gui();
}

void SelectDashboard::set_rgbburn_params_gui() {
  WordList path(menucommand());
  rgbburncmd->makecmdform(path)->show(0);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::CB_demography_params(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->set_demography_params_gui();
}

void SelectDashboard::set_demography_params_gui() {
  WordList path(menucommand());
  demographycmd->makecmdform(path)->show(0);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::CB_demograyfy(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->demograyfy_gui();
}

void SelectDashboard::demograyfy_gui() {
  WordList path(menucommand());
  demograyfycmd->makecmdform(path)->show(0);
}

void SelectDashboard::CB_RGB(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->demorgb_gui();
}

void SelectDashboard::demorgb_gui() {
  WordList path(menucommand());
  demorgbcmd->makecmdform(path)->show(0);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void SelectDashboard::CB_pixeldragshape(FL_OBJECT *obj, long) {
  ((SelectDashboard*) ImageFormGfx::objdash(obj))->pixeldragshape_gui();
}

void SelectDashboard::pixeldragshape_gui() {
  int which = fl_get_choice(choice_pixeldragshape) - 1;
  if(which >= 0) {
    rubberbandtype = (RubberBandType) which;
    garcon()->log(menucommand() + "pixeldragshape="
		  + to_charstring(rubberbandtype));
  }
}

void SelectDashboard::set_pixeldragshape_cmd(const CharString&) {
  ((SelectDashboard*) get_dashboard())->set_pixeldragshape();
}

void SelectDashboard::set_pixeldragshape() {
  if(!textmode)
    fl_set_choice(choice_pixeldragshape, rubberbandtype+1);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// include template definitions

#include "enumio.C"
