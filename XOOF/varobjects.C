// -*- C++ -*-
// $RCSfile: varobjects.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:15 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifdef MENUGFX

#include "varobjects.h"
#include "menucolor.h"

// default fl object for Variable input

void DfltVarObject::show() {
  fl_set_input(input, var->tostring());
  if(type == VO_PROACTIVE) {
    fl_set_input_selected(input, 1);
    fl_show_object(set);
  }
  fl_show_object(input);
}

void DfltVarObject::hide() {
  fl_hide_object(input);
  if(type == VO_PROACTIVE)
    fl_hide_object(set);
}

CharString DfltVarObject::get() {
  return forminputstring(input);
}

// callback for input field 
static void CB_input(FL_OBJECT*, long) {
  // this doesn't do anything
}

// callback for "set" button
static void CB_set(FL_OBJECT*, long ptr) {
  ((DfltVarObject *) ptr)->setvalue();
}

void DfltVarObject::setvalue() {
  if(type == VO_PROACTIVE) {
    CharString string(forminputstring(input));
    form->setvariable(var, string);
    fl_set_input(input, var->tostring());
  }
}

DfltVarObject::DfltVarObject(Variable *v, Form *f, VarObjectType t,
			     int x, int y, int w, int h)
  : VarObject(v, f, t)
{
  int winput = w/2; // width of input field
  const int wbuf = 10;    // pixels between input field and set button
  const int wset = 65;    // width of set button
  CharString label(var->name + " =");
  int xpos;
  if(type == VO_PROACTIVE)   // allow space for "set" button
    xpos = x + w - winput - wset - wbuf;
  else  // no space for button
    xpos = x + w - winput;
  input = fl_add_input(FL_NORMAL_INPUT, xpos, y, winput, h, label);
  fl_set_object_lsize(input,11);
  if(type == VO_REACTIVE)
    fl_set_object_gravity(input, FL_North, FL_NorthEast);
  else
    fl_set_object_gravity(input, FL_South, FL_SouthEast);
  fl_set_object_resize(input, FL_RESIZE_X);
  fl_set_object_callback(input,CB_input,(long) this);

  fl_set_object_color(input, COL_INPUT_UNSELECTED, COL_INPUT_SELECTED);

  if(type == VO_PROACTIVE) {
    set = fl_add_button(FL_RETURN_BUTTON,x+w-wset,y,wset,h,"Set");
    fl_set_object_lsize(set,11);
    fl_set_object_lstyle(set,FL_TIMESBOLD_STYLE);
    fl_set_object_callback(set,CB_set,(long) this);
    fl_set_object_gravity(set, FL_SouthEast, FL_SouthEast);
    fl_set_object_resize(set, FL_RESIZE_NONE);
    fl_set_object_color(set, COL_INPUT_BUTTON_UP, COL_INPUT_BUTTON_DOWN);
  }
}

// ----------------------------------------- //

// input object for a TrueFalse

class TFVarObject : public VarObject {
private:
  FL_OBJECT *labelbox;
  FL_OBJECT *truebutton;
  FL_OBJECT *falsebutton;
public:
  TFVarObject(Variable*, Form*, VarObjectType t, int, int, int, int);
  virtual void show();
  virtual void hide();
  virtual CharString get();
  void set();
};

void TF_set(FL_OBJECT*, long ptr) {
  ((TFVarObject*) ptr)->set();
}

TFVarObject::TFVarObject(Variable *v, Form *mf, VarObjectType t,
			 int x, int y, int w, int h)
  : VarObject(v, mf, t)
{
  const int width = 60;  // width of buttons
  const int buffer = 3;  // space between buttons
  CharString label(var->name + " =");

  fl_bgn_group();

  labelbox = fl_add_box(FL_NO_BOX, x, y, w-2*(width+buffer), h, label);
  fl_set_object_lalign(labelbox, FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fl_set_object_lsize(labelbox, 11);
  fl_set_object_resize(labelbox, FL_RESIZE_X);

  falsebutton = fl_add_lightbutton(FL_RADIO_BUTTON,
				   x+w-width, y, width, h, "false");
  fl_set_object_lsize(falsebutton, 11);
  fl_set_object_color(falsebutton, FL_COL1, FL_RED);
  fl_set_object_callback(falsebutton, TF_set, (long) this);
  fl_set_object_resize(falsebutton, FL_RESIZE_NONE);

  truebutton = fl_add_lightbutton(FL_RADIO_BUTTON,
				  x+w-2*width-buffer, y, width, h, "true");
  fl_set_object_lsize(truebutton, 11);
  fl_set_object_color(truebutton, FL_COL1, FL_GREEN);
  fl_set_object_callback(truebutton, TF_set, (long) this);
  fl_set_object_resize(truebutton, FL_RESIZE_NONE);

  if(type == VO_PROACTIVE) {
    fl_set_object_gravity(truebutton, FL_SouthEast, FL_SouthEast);
    fl_set_object_gravity(falsebutton, FL_SouthEast, FL_SouthEast);
    fl_set_object_gravity(labelbox, FL_SouthWest, FL_SouthEast);
  }
  else {
    fl_set_object_gravity(truebutton, FL_NorthEast, FL_NorthEast);
    fl_set_object_gravity(falsebutton, FL_NorthEast, FL_NorthEast);
    fl_set_object_gravity(labelbox, FL_NorthWest, FL_NorthEast);
  }

  fl_end_group();
}

void TFVarObject::show() {
  int val = var->tostring() == "true";
  fl_set_button(truebutton, val);
  fl_set_button(falsebutton, !val);
  fl_show_object(labelbox);
  fl_show_object(truebutton);
  fl_show_object(falsebutton);
}

void TFVarObject::hide() {
  fl_hide_object(labelbox);
  fl_hide_object(truebutton);
  fl_hide_object(falsebutton);
}

void TFVarObject::set() {
  if(type == VO_PROACTIVE)
    form->setvariable(var, get());
}

CharString TFVarObject::get() {
  int val = fl_get_button(truebutton);
  if(val)
    return CharString("true");
  else
    return CharString("false");
}

template<>
VarObject* Var<TrueFalse>::create_varobject(Variable *var, Form *form,
					    VarObjectType type,
					    int x, int y, int w, int h)
{
  return new TFVarObject(var, form, type, x, y, w, h);
}


#endif  // MENUGFX
