// -*- C++ -*-
// $RCSfile: menuDef.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:10 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef MENUDEF_C
#define MENUDEF_C

#include "menuDef.h"
#include "tocharstring.h"

/* name and helpstring types are parametrized so they can be either
 * const char* or const CharString&.
 */

template <class TYPE, class NTYPE, class HTYPE>
void AddVariable(Menu *m, NTYPE nm, HTYPE help, 
		 TYPE &x, void (*f)(const CharString&)) 
{
  Var<TYPE> *newvar = new Var<TYPE>(nm, help, x, f);
  m->AddOption(newvar);
}

template <class TYPE, class NTYPE, class HTYPE>
void AddVariable(Menu *m, NTYPE nm, HTYPE help, TYPE &x) 
{
  Var<TYPE> *newvar = new Var<TYPE>(nm, help, x);
  m->AddOption(newvar);
}

template <class TYPE, class NTYPE, class HTYPE>
void AddVariable(Menu *m, NTYPE nm, HTYPE help, 
		 TYPE &x, void (*f)(const CharString&),
		 const MenuAttribute &att) 
{
  Var<TYPE> *newvar = new Var<TYPE>(nm, help, x, f, att);
  m->AddOption(newvar);
}

template <class TYPE, class NTYPE, class HTYPE>
void AddVariable(Menu *m, NTYPE nm, HTYPE help, TYPE &x,
		 const MenuAttribute &att) 
{
  Var<TYPE> *newvar = new Var<TYPE>(nm, help, x, att);
  m->AddOption(newvar);
}

// --- 

template <class TYPE, class NTYPE>
void AddArgument(CommandM *cmd, NTYPE nm, TYPE &x) {
    Var<TYPE> *arg = new Var<TYPE>(nm, x);
    cmd->arg.grow(1, arg);
}

template <class TYPE, class NTYPE>
void AddArgument(CommandM *cmd, NTYPE nm, TYPE &x, const MenuAttribute &att) {
    Var<TYPE> *arg = new Var<TYPE>(nm, x, att);
    cmd->arg.grow(1, arg);
}


// ---------------------------------------------------------- //

template <class TYPE>
Var<TYPE>::Var(const CharString &nm, const CharString &hs, TYPE &x,
					void (*f)(const CharString&))
    : Variable(nm, hs, (void*)&x, f)
{}

template <class TYPE>
Var<TYPE>::Var(const CharString &nm, const CharString &hs, TYPE &x,
	       void (*f)(const CharString&), const MenuAttribute &att)
    : Variable(nm, hs, (void*)&x, f, att)
{}

template <class TYPE>
Var<TYPE>::Var(const CharString &nm, const CharString &hs, TYPE &x)
    : Variable(nm, hs, (void*)&x)
{}

template <class TYPE>
Var<TYPE>::Var(const CharString &nm, const CharString &hs, TYPE &x,
	       const MenuAttribute &att)
    : Variable(nm, hs, (void*)&x, 0, att)
{}

template <class TYPE>
Var<TYPE>::Var(const CharString &nm, TYPE &x)
  : Variable(nm, CharString(""), (void*)&x)
{}

template <class TYPE>
Var<TYPE>::Var(const CharString &nm, TYPE &x, const MenuAttribute &att)
  : Variable(nm, CharString(""), (void*)&x, 0, att)
{}

template <class TYPE>
Var<TYPE>::~Var() {
  if(oldvalue)
    delete (TYPE*) oldvalue;
}

template <class TYPE>
int Var<TYPE>::fromstring(const CharString &str, Waiter&) const {
    istrstream iss(str);
    iss >> *(TYPE*) address;
    if(iss) return 1;
    return 0;
}

// read from TextWaiter's input buffer
template <class TYPE>
void Var<TYPE>::fromstring(TextWaiter &w) const {
    *w.inbuf >> *(TYPE*) address;
}

template <class TYPE>
CharString Var<TYPE>::tostring() const {
    return to_charstring(*(TYPE*) address);
}

template <class TYPE>
void Var<TYPE>::copyvalue(const Variable *v) {
    *(TYPE*) address = *((TYPE*)(v->address));
}

template <class TYPE>
void Var<TYPE>::savevalue() {
    if(oldvalue) delete (TYPE*)oldvalue;
    oldvalue = new TYPE;
    *(TYPE*)oldvalue = *(TYPE*)address;
}

template <class TYPE>
void Var<TYPE>::restorevalue() {
    *(TYPE*) address = *(TYPE*) oldvalue;
}

// -------------------------------------------------------- //

#ifdef MENUGFX
#include "varobjects.h"

template <class TYPE>
VarObject *Var<TYPE>::create_varobject(Variable *var, Form *form,
				       VarObjectType type,
				       int x, int y, int w, int h) {
  return new DfltVarObject(var, form, type, x, y, w, h);
}
#endif // MENUGFX

#endif
