// -*- C++ -*-
// $RCSfile: imagemoddash.C,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2001-04-19 01:39:35 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "imageform.h"
#include "imagemoddash.h"
#include "ppm2oof.h"
#include "stencil.h"

ImageModDashboard::ImageModDashboard(ImageForm *iform, FL_OBJECT *grp)
  : Dashboard("Modify_Image", iform, grp),
    smoothparamcmd(this),
    nonlinearparamcmd(this),
    stencilparamcmd(this),
    equalizeparamcmd(this),
    smooth_dt(smooth_dt_dflt),
    smooth_nsteps(smooth_nsteps_dflt),
    nonlinear_dt(nonlinear_dt_dflt),
    nonlinear_alpha(nonlinear_alpha_dflt),
    nonlinear_nsteps(nonlinear_nsteps_dflt),
    stenciltype(stenciltype_dflt),
    stencil_nsteps(stencil_nsteps_dflt),
    equalize_xrange(equalize_xrange_dflt),
    equalize_yrange(equalize_yrange_dflt),
    modified_(false)
{
  
  static int already_done = 0;
  if(!already_done) {
    already_done = 1;
    Enum<StencilType>::name(LAPLACIAN, "laplacian");
    Enum<StencilType>::name(BLUR, "blur");
    Enum<StencilType>::name(SMOOTH, "smooth");
    Enum<StencilType>::name(SHARPEN, "sharpen");
    Enum<StencilType>::name(ENHANCE1, "edge_enhance_1");
    Enum<StencilType>::name(ENHANCE2, "edge_enhance_2");
    Enum<StencilType>::name(FIND, "edge_find");
  }

  set_mod_undoredobuttons();

  buildmenu(menu, MOPT_NORMAL);
  buildmenu(imageformmenu(), MOPT_NOPRINT); // for backwards compatibility
}

void ImageModDashboard::buildmenu(Menu *carte, MenuAttribute attr) {
  carte->AddCommand("gray", "convert image to gray scale", gray_cmd, attr);
  carte->AddCommand("maxcontrast", "maximize constrast", maxcontrast_cmd, attr);
  carte->AddCommand("smooth", "apply diffusion equation to image",
		    smooth_cmd, attr);
  carte->AddCommand(&smoothparamcmd, attr);
  carte->AddCommand("nonlinear", "apply nonlinear diffusion equation to image",
		     nonlinear_cmd, attr);
  carte->AddCommand(&nonlinearparamcmd, attr);
  carte->AddCommand("undomod", "undo last image modification", undo_cmd, attr);
  carte->AddCommand("undoallmod", "undo all modifications", undoall_cmd, attr);
  carte->AddCommand("redomod", "redo last undone image modification",
		     redo_cmd, attr);
  carte->AddCommand("stencil", "apply stencil to image", stencil_cmd, attr);
  carte->AddCommand(&stencilparamcmd, attr);
  carte->AddCommand("equalize", "smooth out large scale variations", equalize_cmd, attr);
  carte->AddCommand(&equalizeparamcmd, attr);
}

ImageModDashboard::~ImageModDashboard() {
  menu->RemoveOption("gray");
  menu->RemoveOption("maxcontrast");
  menu->RemoveOption("smooth");
  menu->RemoveOption("nonlinear");
  menu->RemoveOption("stencil");
  menu->RemoveOption("undomod");
  menu->RemoveOption("undoallmod");
  menu->RemoveOption("redomod");
  menu->RemoveOption("equalize");
  // smooth_params doesn't appear here because it's a member of
  // ImageModDashboard, and not created by the AddCommand function.
  delete menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Maintain stack of modified images

void ImageModDashboard::initmod() {
  imagestack.dup();
  modified_ = true;
  set_mod_undoredobuttons();
}

void ImageModDashboard::set_mod_undoredobuttons() {
  if(textmode) return;
  fl_freeze_form(fdui()->image_drawer);
  if(imagestack.attop()) {
    fl_set_object_lcol(fdui()->button_redomod, FL_INACTIVE_COL);
    fl_deactivate_object(fdui()->button_redomod);
  }
  else {
    fl_set_object_lcol(fdui()->button_redomod, FL_BLACK);
    fl_activate_object(fdui()->button_redomod);
  }
  if(imagestack.atbottom()) {
    fl_set_object_lcol(fdui()->button_undomod, FL_INACTIVE_COL);
    fl_deactivate_object(fdui()->button_undomod);
    fl_set_object_lcol(fdui()->button_undoallmod, FL_INACTIVE_COL);
    fl_deactivate_object(fdui()->button_undoallmod);
  }
  else {
    fl_set_object_lcol(fdui()->button_undomod, FL_BLACK);
    fl_activate_object(fdui()->button_undomod);
    fl_set_object_lcol(fdui()->button_undoallmod, FL_BLACK);
    fl_activate_object(fdui()->button_undoallmod);
  }
  fl_unfreeze_form(fdui()->image_drawer);
}

void ImageModDashboard::saved_image() {
  // called when the image has been saved in the gallery, and is no
  // longer considered "modified".
  modified_ = false;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageModDashboard::undomod() {
  if(imagestack.prev()) {
    imageform()->draw();
    set_mod_undoredobuttons();
    modified_ = true;
  }
}

void ImageModDashboard::CB_button_undo(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->undo_gui();
}

void ImageModDashboard::undo_gui() {
  if(active()) {
    garcon()->log(menucommand() + "undomod");
    undomod();
  }
}

CommandFn ImageModDashboard::undo_cmd() {
  ((ImageModDashboard*) get_dashboard())->undomod();
}



void ImageModDashboard::undoallmod() {
  if(imagestack.prev()) {	// undo most recent modification
    while(imagestack.prev());	// undo rest of them
    imageform()->draw();
    set_mod_undoredobuttons();
  }
}

void ImageModDashboard::CB_button_undoall(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->undoall_gui();
}

void ImageModDashboard::undoall_gui() {
  if(active()) {
    garcon()->log(menucommand() + "undoallmod");
    undoallmod();
  }
}

CommandFn ImageModDashboard::undoall_cmd() {
  ((ImageModDashboard*) get_dashboard())->undoallmod();
}


void ImageModDashboard::redomod() {
  if(imagestack.next()) {
    imageform()->draw();
    set_mod_undoredobuttons();
    modified_ = true;
  }
}

void ImageModDashboard::CB_button_redo(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->redo_gui();
}

void ImageModDashboard::redo_gui() {
  if(active()) {
    garcon()->log(menucommand() + "redomod");
    redomod();
  }
}

CommandFn ImageModDashboard::redo_cmd() {
  ((ImageModDashboard*) get_dashboard())->redomod();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageModDashboard::gray() {
  initmod();
  imageform()->current_picture().makegray();
  imageform()->draw();
}

void ImageModDashboard::CB_button_gray(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->gray_gui();
}

void ImageModDashboard::gray_gui() {
  if(active()) {
    garcon()->log(menucommand() + "gray");
    gray();
  }
}

CommandFn ImageModDashboard::gray_cmd() { // called by menu
  ((ImageModDashboard*) get_dashboard())->gray();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageModDashboard::maxcontrast() {
  initmod();
  imageform()->current_picture().maxcontrast();
  imageform()->draw();
}

void ImageModDashboard::CB_button_maxcontrast(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->maxcontrast_gui();
}

void ImageModDashboard::maxcontrast_gui() {
  if(active()) {
    garcon()->log(menucommand() + "maxcontrast");
    maxcontrast();
  }
}

CommandFn ImageModDashboard::maxcontrast_cmd() {
  ((ImageModDashboard*) get_dashboard())->maxcontrast();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

double ImageModDashboard::smooth_dt_dflt = 0.1;
int ImageModDashboard::smooth_nsteps_dflt = 5;

void ImageModDashboard::smooth() {
  initmod();
  imageform()->current_picture().smooth(smooth_dt, smooth_nsteps);
  imageform()->draw();
}

void ImageModDashboard::CB_button_smooth(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->smooth_gui();
}

void ImageModDashboard::smooth_gui() {
  if(active()) {
    garcon()->log(menucommand() + "smooth");
    smooth();
  }
}

CommandFn ImageModDashboard::smooth_cmd() {
  ((ImageModDashboard*) get_dashboard())->smooth();
}

ImageModDashboard::SmoothParamCmd::SmoothParamCmd(ImageModDashboard *dash)
  : CommandM("smooth_params", "set parameters for image smoothing"),
    imd(dash)
{
  AddArgument(this, "dt", imd->smooth_dt);
  AddArgument(this, "nsteps", imd->smooth_nsteps);
}

CommandFn ImageModDashboard::SmoothParamCmd::func() {
  imd->smooth_nsteps_dflt = imd->smooth_nsteps;
  imd->smooth_dt_dflt = imd->smooth_dt;
}

CommandM *ImageModDashboard::SmoothParamCmd::clone() const {
  return new SmoothParamCmd(imd);
}

void ImageModDashboard::CB_button_smooth_params(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->smooth_params_gui();
}

void ImageModDashboard::smooth_params_gui() {
  WordList path(menucommand());
  smoothparamcmd.makecmdform(path)->show(0);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

double ImageModDashboard::nonlinear_dt_dflt = 0.1;
double ImageModDashboard::nonlinear_alpha_dflt = 1.0;
int ImageModDashboard::nonlinear_nsteps_dflt = 5;

void ImageModDashboard::nonlinear() {
  initmod();
  imageform()->current_picture().nonlinear_smooth(nonlinear_alpha,
						  nonlinear_dt,
						  nonlinear_nsteps);
  imageform()->draw();
}

void ImageModDashboard::CB_button_nonlinear(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->nonlinear_gui();
}

void ImageModDashboard::nonlinear_gui() {
  if(active()) {
    garcon()->log(menucommand() + "nonlinear");
    nonlinear();
  }
}

CommandFn ImageModDashboard::nonlinear_cmd() {
  ((ImageModDashboard*) get_dashboard())->nonlinear();
}

void ImageModDashboard::CB_button_nonlinear_params(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->nonlinear_params_gui();
}

void ImageModDashboard::nonlinear_params_gui() {
  WordList path(menucommand());
  nonlinearparamcmd.makecmdform(path)->show(0);
}

ImageModDashboard::NonlinearParamCmd::NonlinearParamCmd(ImageModDashboard *dash)
  : CommandM("nonlinear_params", "set parameters for nonlinear smoothing"),
    imd(dash)
{
  AddArgument(this, "dt", imd->nonlinear_dt);
  AddArgument(this, "alpha", imd->nonlinear_alpha);
  AddArgument(this, "nsteps", imd->nonlinear_nsteps);
}

CommandFn ImageModDashboard::NonlinearParamCmd::func() {
  imd->nonlinear_dt_dflt = imd->nonlinear_dt;
  imd->nonlinear_alpha_dflt = imd->nonlinear_alpha;
  imd->nonlinear_nsteps_dflt = imd->nonlinear_nsteps;
}

CommandM *ImageModDashboard::NonlinearParamCmd::clone() const {
  return new NonlinearParamCmd(imd);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int ImageModDashboard::stencil_nsteps_dflt = 1;
Enum<ImageModDashboard::StencilType>
   ImageModDashboard::stenciltype_dflt(ENHANCE1);

static Stencil laplace(-4, 1, 0, 1);
static Stencil blur(0, 1, 1, 8);
static Stencil smooth_stencil(5, 1, 1, 13);
static Stencil sharpen(32, -2, -2, 16);
static Stencil edge_find(8, -1, -1, 1);
static Stencil edge_enhance1(9, -1, -1, 1);
static Stencil edge_enhance2(10, -1, -1, 2);

void ImageModDashboard::stencil() {
  initmod();
  switch(stenciltype) {
  case LAPLACIAN:
    imageform()->current_picture().stencil(laplace, stencil_nsteps);
    break;
  case BLUR:
    imageform()->current_picture().stencil(blur, stencil_nsteps);
    break;
  case SMOOTH:
    imageform()->current_picture().stencil(smooth_stencil, stencil_nsteps);
    break;
  case SHARPEN:
    imageform()->current_picture().stencil(sharpen, stencil_nsteps);
    break;
  case FIND:
    imageform()->current_picture().stencil(edge_find, stencil_nsteps);
    break;
  case ENHANCE1:
    imageform()->current_picture().stencil(edge_enhance1, stencil_nsteps);
    break;
  case ENHANCE2:
    imageform()->current_picture().stencil(edge_enhance2, stencil_nsteps);
    break;
  }
  imageform()->draw();
}

void ImageModDashboard::CB_button_stencil(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->stencil_gui();
}

void ImageModDashboard::stencil_gui() {
  if(active()) {
    garcon()->log(menucommand() + "stencil");
    stencil();
  }
}

CommandFn ImageModDashboard::stencil_cmd() {
  ((ImageModDashboard*) get_dashboard())->stencil();
}

void ImageModDashboard::CB_button_stencil_params(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->stencil_params_gui();
}

void ImageModDashboard::stencil_params_gui() {
  WordList path(menucommand());
  stencilparamcmd.makecmdform(path)->show(0);
}

ImageModDashboard::StencilParamCmd::StencilParamCmd(ImageModDashboard *dash)
  : CommandM("stencil_params", "set parameters for stencil application"),
    imd(dash)
{
  AddArgument(this, "mask", imd->stenciltype);
  AddArgument(this, "iterations", imd->stencil_nsteps);
}

CommandFn ImageModDashboard::StencilParamCmd::func() {
  imd->stenciltype_dflt = imd->stenciltype;
  imd->stencil_nsteps_dflt = imd->stencil_nsteps;
}

CommandM *ImageModDashboard::StencilParamCmd::clone() const {
  return new StencilParamCmd(imd);
}

template <>
VarObject *Var<Enum<ImageModDashboard::StencilType> >::create_varobject(
  Variable *var, Form *form, VarObjectType t, int x, int y, int w, int h)
{
  return new EnumVarObject<ImageModDashboard::StencilType>(var, form,
							   t, x, y, w, h);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int ImageModDashboard::equalize_xrange_dflt = 10;
int ImageModDashboard::equalize_yrange_dflt = 10;

void ImageModDashboard::equalize() {
  initmod();
  imageform()->current_picture().averageSmooth(equalize_xrange,
					       equalize_yrange);
  imageform()->draw();
}

void ImageModDashboard::CB_button_equalize(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->equalize_gui();
}

void ImageModDashboard::equalize_gui() {
  if(active()) {
    garcon()->log(menucommand() + "equalize");
    equalize();
  }
}

CommandFn ImageModDashboard::equalize_cmd() {
  ((ImageModDashboard*) get_dashboard())->equalize();
}

void ImageModDashboard::CB_button_equalize_params(FL_OBJECT *obj, long) {
  ((ImageModDashboard*) ImageFormGfx::objdash(obj))->equalize_params_gui();
}

void ImageModDashboard::equalize_params_gui() {
  WordList path(menucommand());
  equalizeparamcmd.makecmdform(path)->show(0);
}

ImageModDashboard::EqualizeParamCmd::EqualizeParamCmd(ImageModDashboard *dash)
  : CommandM("equalize_params", "set parameters for image equalization"),
    imd(dash)
{
  AddArgument(this, "xrange", imd->equalize_xrange);
  AddArgument(this, "yrange", imd->equalize_yrange);
}

CommandFn ImageModDashboard::EqualizeParamCmd::func() {
  imd->equalize_xrange_dflt = imd->equalize_xrange;
  imd->equalize_yrange_dflt = imd->equalize_yrange;
}

CommandM *ImageModDashboard::EqualizeParamCmd::clone() const {
  return new EqualizeParamCmd(imd);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

#include "enumio.C"
