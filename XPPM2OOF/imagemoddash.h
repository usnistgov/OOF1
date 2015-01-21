// -*- C++ -*-
// $RCSfile: imagemoddash.h,v $
// $Revision: 1.8 $
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

#ifndef IMAGEMODDASH_H
#define IMAGEMODDASH_H

#include "forms.h"
#include "dashboard.h"
#include "menuDef.h"
#include "rastack.h"

class ImageModDashboard : public Dashboard {
public:
  enum StencilType {
    LAPLACIAN,
    BLUR,
    SMOOTH,
    SHARPEN,
    FIND,
    ENHANCE1,
    ENHANCE2
  };
private:
  ImageModDashboard(ImageForm*, FL_OBJECT*);
  ~ImageModDashboard();

  RAStack<Image> imagestack;
  bool modified_;
  bool modified() const { return modified_; }

  void buildmenu(Menu*, MenuAttribute);
  
  void initmod();		// call this before modifying image
  void undomod();		// undo modification
  void undoallmod();		// undo all modifications
  void redomod();		// redo modification
  void set_mod_undoredobuttons();
  void saved_image();		// called when current image has been saved

  void gray();			// convert image to gray
  static CommandFn gray_cmd();	// called by menus
  static void CB_button_gray(FL_OBJECT*, long);	// called by forms
  void gray_gui();		// called by CB_button_gray

  void maxcontrast();		// maximize contrast in image
  static CommandFn maxcontrast_cmd();
  static void CB_button_maxcontrast(FL_OBJECT*, long);
  void maxcontrast_gui();

  void smooth();		// diffuse image
  static CommandFn smooth_cmd();
  static void CB_button_smooth(FL_OBJECT*, long);
  void smooth_gui();

  class SmoothParamCmd : public CommandM {
  private:
    ImageModDashboard *imd;
    SmoothParamCmd(ImageModDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageModDashboard;
  };
  friend class SmoothParamCmd;
  // The parameters for the modification commands can't be local to
  // the parameter-setting CommandM classes, because a *copy* of the
  // command is executed. They should be members of the
  // ImageModDashboard class, so that different graphics windows can
  // have different sets of parameters.
  double smooth_dt;
  int smooth_nsteps;
  static double smooth_dt_dflt;
  static int smooth_nsteps_dflt;
  SmoothParamCmd smoothparamcmd;
  static void CB_button_smooth_params(FL_OBJECT*, long);
  void smooth_params_gui();

  void nonlinear();		// non-linear diffusion
  static CommandFn nonlinear_cmd();
  static void CB_button_nonlinear(FL_OBJECT*, long);
  void nonlinear_gui();

  class NonlinearParamCmd : public CommandM {
  private:
    ImageModDashboard *imd;
    NonlinearParamCmd(ImageModDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageModDashboard;
  };
  friend class NonlinearParamCmd;
  double nonlinear_dt;
  double nonlinear_alpha;
  int nonlinear_nsteps;
  static double nonlinear_dt_dflt;
  static double nonlinear_alpha_dflt;
  static int nonlinear_nsteps_dflt;
  NonlinearParamCmd nonlinearparamcmd;
  static void CB_button_nonlinear_params(FL_OBJECT*, long);
  void nonlinear_params_gui();

  void stencil();
  static CommandFn stencil_cmd();
  static void CB_button_stencil(FL_OBJECT*, long);
  void stencil_gui();

  class StencilParamCmd : public CommandM {
  private:
    ImageModDashboard *imd;
    StencilParamCmd(ImageModDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageModDashboard;
  };
  friend class StencilParamCmd;
  int stencil_nsteps;
  Enum<StencilType> stenciltype;
  static int stencil_nsteps_dflt;
  static Enum<StencilType> stenciltype_dflt;
  StencilParamCmd stencilparamcmd;
  static void CB_button_stencil_params(FL_OBJECT*, long);
  void stencil_params_gui();

  void equalize();
  static CommandFn equalize_cmd();
  static void CB_button_equalize(FL_OBJECT*, long);
  void equalize_gui();

  class EqualizeParamCmd : public CommandM {
  private:
    ImageModDashboard *imd;
    EqualizeParamCmd(ImageModDashboard*);
    virtual CommandFn func();
    virtual CommandM *clone() const;
    friend class ImageModDashboard;
  };
  friend class EqualizeParamCmd;
  int equalize_xrange;
  int equalize_yrange;
  static int equalize_xrange_dflt;
  static int equalize_yrange_dflt;
  EqualizeParamCmd equalizeparamcmd;
  static void CB_button_equalize_params(FL_OBJECT*, long);
  void equalize_params_gui();

  static void CB_button_undo(FL_OBJECT*, long);
  void undo_gui();
  static CommandFn undo_cmd();
  static void CB_button_undoall(FL_OBJECT*, long);
  void undoall_gui();
  static CommandFn undoall_cmd();
  static void CB_button_redo(FL_OBJECT*, long);
  void redo_gui();
  static CommandFn redo_cmd();

  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
  friend FD_image_drawer *create_form_image_drawer();
};

#endif
