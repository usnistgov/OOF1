// -*- C++ -*-
// $RCSfile: enum.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2001-06-04 19:30:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef ENUM_C
#define ENUM_C

#include <iostream.h>
#include "enum.h"
#include "menuDef.h"
#include "varobjects.h"

template <class ENUMTYPE>
Vec<const CharString*> &Enum<ENUMTYPE>::names() {
  static Vec<const CharString*> nms;
  return nms;
}

template <class ENUMTYPE>
void Enum<ENUMTYPE>::name(const ENUMTYPE &x, const CharString &nm) {
  static CharString unassigned("unassigned");
  Vec<const CharString*> &nms = names();
  int ix = x;
  if(x >= nms.capacity()) nms.resize(ix+1, &unassigned);
  nms[ix] = new CharString(nm);
}

// --------------------------------------------- //


/********** this is illegal!
template <class ENUMTYPE>
VarObject* Var< Enum<ENUMTYPE> >::create_varobject(Variable *var,
						   Form *form,
						   VarObjectType t,
						   int x, int y, int h, int w)
{
  return new EnumVarObject<ENUMTYPE>(var, form, t, x, y, w, h);
}
************/


template <class ENUMTYPE>
void EnumVarObject<ENUMTYPE>::CB_choice(FL_OBJECT*, long ptr) {
  ((EVO*) ptr)->set();
}

template <class ENUMTYPE>
EnumVarObject<ENUMTYPE>::EnumVarObject(Variable *v, Form *f,
				       VarObjectType t,
                                       int x, int y, int w, int h)
  : EVO(v, f, t)
{
  const int width = 150;
  CharString label(var->name + " =");
  choice = fl_add_choice(FL_NORMAL_CHOICE, x+w-width, y, width, h, label);
  fl_set_object_boxtype(choice, FL_UP_BOX);
  fl_set_object_lsize(choice, 11);
  fl_set_choice_fontsize(choice, 11);
  fl_set_choice_align(choice, FL_ALIGN_LEFT);
  EVO *ptr = this;
  fl_set_object_callback(choice, CB_choice, (long) ptr);
  fl_set_object_resize(choice, FL_RESIZE_NONE);
  if(type == VO_REACTIVE)
    fl_set_object_gravity(choice, FL_NorthEast, FL_NorthEast);
  else
    fl_set_object_gravity(choice, FL_SouthEast, FL_SouthEast);
  Vec<const CharString*> &names = Enum<ENUMTYPE>::names();
  for(int i=0; i<names.capacity(); i++)
    if(names[i]) {
      value.grow(1, (ENUMTYPE) i);
      fl_addto_choice(choice, *names[i]);
    }
}

template <class ENUMTYPE>
void EnumVarObject<ENUMTYPE>::set() {
  if(type == VO_PROACTIVE)
    form->setvariable(var, get());
}

template <class ENUMTYPE>
CharString EnumVarObject<ENUMTYPE>::get() {
  return CharString(fl_get_choice_text(choice));
}

template <class ENUMTYPE>
void EnumVarObject<ENUMTYPE>::show() {
  fl_set_choice_text(choice, var->tostring());
  fl_show_object(choice);
}

template <class ENUMTYPE>
void EnumVarObject<ENUMTYPE>::hide() {
  fl_hide_object(choice);
}


#endif
