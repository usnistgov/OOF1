// -*- C++ -*-
// $RCSfile: filename.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-30 19:23:25 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "filename.h"
#include "varobjects.h"
#include "menucolor.h"
#include "word.h"
#include <iostream.h>

#ifdef MENUGFX

// This is the GUI code

class FileNameVarObject : public VarObject {
private:
  FL_OBJECT *browsebutton;
  FL_OBJECT *okbutton;
  FL_OBJECT *inputarea;
  void set();
  void browse();
public:
  FileNameVarObject(Variable *var, Form *form, VarObjectType type,
		    int x, int y, int w, int h);
  virtual void show();
  virtual void hide();
  virtual CharString get();
  static void CB_filenameokbutton(FL_OBJECT*, long);
  static void CB_filenamebrowse(FL_OBJECT*, long);
};


// callback routines invoked by buttons in the object
void FileNameVarObject::CB_filenameokbutton(FL_OBJECT*, long ptr) {
  ((FileNameVarObject *) ptr)->set();
}

void FileNameVarObject::set() {
  if(type == VO_PROACTIVE)
    form->setvariable(var, get());
}

void FileNameVarObject::CB_filenamebrowse(FL_OBJECT*, long ptr) {
  ((FileNameVarObject*) ptr)->browse();
}

void FileNameVarObject::browse() {
  fl_use_fselector(0);
  const char *picked = fl_show_fselector(var->name, 0, 0, 0);
  if(picked) {
    fl_set_input(inputarea, picked);
    fl_set_input_cursorpos(inputarea, strlen(picked), 0); // scroll to end
    if(type == VO_PROACTIVE) {
      form->setvariable(var, picked);
    }
  }
  fl_raise_form(form->the_form); // because the cmd form seems to get lost...
}

static void CB_input(FL_OBJECT*, long) {} // does nothing!

FileNameVarObject::FileNameVarObject(Variable *v, Form *mf, VarObjectType t,
		    int x, int y, int w, int h)
  : VarObject(v, mf, t)
{
  const int browsewidth = (type == VO_PROACTIVE? 70 : 30);
  const int okwidth = (type == VO_PROACTIVE? 65 : 0); // only used if proactive
  const int inputwidth = w/2;
  CharString label(var->name + " =");

  fl_bgn_group();
  if(type == VO_PROACTIVE) {
    okbutton = fl_add_button(FL_RETURN_BUTTON,
			     x+w-okwidth, y, okwidth, h, "Set");
    fl_set_object_lstyle(okbutton, FL_TIMESBOLD_STYLE);
    fl_set_object_lsize(okbutton, 11);
    fl_set_object_resize(okbutton, FL_RESIZE_NONE);
    fl_set_object_callback(okbutton, CB_filenameokbutton, (long) this);
    fl_set_object_color(okbutton, COL_INPUT_BUTTON_UP, COL_INPUT_BUTTON_DOWN);
  }
  browsebutton = fl_add_button(FL_NORMAL_BUTTON,
			       x+w-okwidth-browsewidth, y, browsewidth, h,
			       (type == VO_PROACTIVE? "Browse?" : "?"));
  fl_set_object_lstyle(browsebutton, FL_BOLD_STYLE);
  fl_set_object_lsize(browsebutton, 11);
  fl_set_object_resize(browsebutton, FL_RESIZE_NONE);
  fl_set_object_callback(browsebutton, CB_filenamebrowse, (long) this);
  fl_set_object_color(browsebutton, COL_INPUT_BUTTON_UP, COL_INPUT_BUTTON_DOWN);
  inputarea = fl_add_input(FL_NORMAL_INPUT,
			   x+w-okwidth-browsewidth-inputwidth, y,
			   inputwidth, h, label);
  fl_set_object_lsize(inputarea, 11);
  fl_set_object_resize(inputarea, FL_RESIZE_X);
  fl_set_object_callback(inputarea, CB_input, (long) this);
  fl_set_object_color(inputarea, COL_INPUT_UNSELECTED, COL_INPUT_SELECTED);

  if(type == VO_PROACTIVE) { // appearing in main menu form
    fl_set_object_gravity(okbutton, FL_SouthEast, FL_SouthEast);
    fl_set_object_gravity(browsebutton, FL_SouthEast, FL_SouthEast);
    fl_set_object_gravity(inputarea, FL_SouthWest, FL_SouthEast);
  }
  else { // appearing in command form
    fl_set_object_gravity(browsebutton, FL_NorthEast, FL_NorthEast);
    fl_set_object_gravity(inputarea, FL_NorthWest, FL_NorthEast);
  }
  fl_end_group();
}

void FileNameVarObject::hide() {
  fl_hide_object(inputarea);
  fl_hide_object(browsebutton);
  if(type == VO_PROACTIVE)
    fl_hide_object(okbutton);
}

void FileNameVarObject::show() {
  fl_set_input(inputarea, var->tostring());
  if(type == VO_PROACTIVE) {
    fl_set_input_selected(inputarea, 1);
    fl_show_object(okbutton);
  }
  fl_show_object(inputarea);
  fl_show_object(browsebutton);
}

CharString FileNameVarObject::get() {
  return forminputstring(inputarea);
}

// This is called by the menu routines to create the GUI for the FileName.

template <>
VarObject *Var<FileName>::create_varobject(Variable *var, Form *form,
					   VarObjectType type,
					   int x, int y, int w, int h)
{
  return new FileNameVarObject(var, form, type, x, y, w, h);
}

#endif // MENUGFX

// these specializations probably shouldn't be necessary, but they're
// more efficient than the template... See the same thing for Var<CharString>

template <>
int Var<FileName>::fromstring(const CharString &str, Waiter&) const {
  *(FileName*) address = str;
  return 1;
}

template <>
void Var<FileName>::fromstring(TextWaiter &w) const {
  Word str;
  *w.inbuf >> str;
  *(FileName*) address = str;
}
