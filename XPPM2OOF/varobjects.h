// -*- C++ -*-
// $RCSfile: varobjects.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:50 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef VAROBJECTS_H
#define VAROBJECTS_H

#ifdef MENUGFX

class VarObject;
class DfltVarObject;
typedef enum {VO_REACTIVE, VO_PROACTIVE} VarObjectType;

#include "menuDef.h"
#include "menuform.h"

/* Each class NewType that appears in a variable list in a menu can be set by
 * graphical xforms objects (FL_OBJECTs). To do this, each type must
 *
 * (0) have overloaded iostream insertion and extraction operators.
 *
 * (1) define a new class (eg NewVarObject) derived from the VarObject class.
 * This class must have
 * 
 * (a) a constructor with arguments
 *          Variable *var, Form *form,VarObjectType type,
 *          int x, int y, int w, int h
 *
 * The first three arguments must be passed through to the VarObject
 * constructor. "var" is the Variable being set, "form" is the xform
 * that it's appearing in, and "type" determines how the variable
 * reacts to being set.  Proactive VarObjects (type==VO_PROACTIVE)
 * inform the waiter that they have been set. Reactive ones
 * (type==VO_REACTIVE) wait until asked. Generally, a proactive VarObject
 * will need an "ok" button, while a reactive one will not. Every
 * VarObject class must be able to be either proactive or reactive. 
 *
 * The last four arguments to the constructor are the position and
 * size of the area available for drawing the object.  The constructor
 * should create the FL_OBJECTs by making the appropriate
 * fl_add...() calls. It should *not* call fl_addto_form() or
 * fl_end_form(). It should use CharString var->name to show which
 * variable is being set.
 *
 * The constructor should assign callback routines to the active FL_OBJECTs.
 *
 * (b) a virtual void function NewVarObject::show() which displays the
 * FL_OBJECTs, using fl_show_object. It should display the current
 * value of the variable, which can be obtained in ascii with
 * CharString var->tostring().
 *
 * (c) a virtual void function NewVarObject::hide() which calls
 * fl_hide_object on all the FL_OBJECTs.
 *
 * (d) a virtual CharString function NewVarObject::get() which
 * returns the string representing the current value of the function.
 * This is the only way in which reactive VarObjects return their value.
 * 
 * (e) a function which is called when the variable has been set by the user,
 * and if type==VO_PROACTIVE, calls form->setvariable(var, string), where
 * string is a CharString representing the value of the variable. 
 *
 *
 * (2) specialize the create_varobject function in the Var<T> template:
 *
 *    VarObject* Var<NewType>::create_varobject(Variable *var, Form *form,
 *                                      VarObjectType type,
 *                                      int x, int y, int w, int h)
 *      {
 *         return new NewVarObject(var, form, type, x, y, w, h);
 *      }
 *
 * to return a new TVarObject object.
 */


class VarObject {
protected:
  Variable *var;
  Form *form;
  VarObjectType type;
  int vsbl;
public:
  VarObject(Variable *v, Form *f, VarObjectType t)
    : var(v), form(f), type(t), vsbl(0) {}
  virtual ~VarObject() {}
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual CharString get() = 0;
  int visible() const { return vsbl; }
  friend class CommandMForm;
  friend class MenuForm;
};

class DfltVarObject : public VarObject {
public:
  FL_OBJECT *input;
  FL_OBJECT *set;
  DfltVarObject(Variable*, Form*, VarObjectType, int, int, int, int);
  virtual void show();
  virtual void hide();
  virtual CharString get();
  void setvalue();
};

#endif
#endif
