// -*- C++ -*-
// $RCSfile: groups.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-09-06 20:11:00 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef GROUPS_C
#define GROUPS_C

#include "groups.h"

template <class TYPE>
Group<TYPE>::Group(const CharString &nm)
  : label(nm),
    members(0, BlockSize(100))
{
  for(int i=0; i<GroupMenu<TYPE>::menulist.capacity(); i++)
    GroupMenu<TYPE>::menulist[i]->AddCommand(label,
	    GroupMenu<TYPE>::menulist[i]->grouphelp + " " + label,
	    GroupMenu<TYPE>::menulist[i]->groupcmd);
  
  for(int i=0; i<GAMenu<TYPE>::menulist.capacity(); i++)
    GAMenu<TYPE>::menulist[i]->addcmd(label);
}

template <class TYPE>
Group<TYPE>::~Group() {
  for(int i=0; i<GroupMenu<TYPE>::menulist.capacity(); i++)
    GroupMenu<TYPE>::menulist[i]->RemoveOption(label);
  for(int i=0; i<GAMenu<TYPE>::menulist.capacity(); i++)
    GAMenu<TYPE>::menulist[i]->RemoveOption(label);
}

template <class TYPE>
bool Group<TYPE>::rename(const CharString &name) {
  if(name == label) return 1;
  for(int i=0; i<GroupMenu<TYPE>::menulist.capacity(); i++)
    if(!GroupMenu<TYPE>::menulist[i]->RenameOption(label, name))
       return 0;
  for(int i=0; i<GAMenu<TYPE>::menulist.capacity(); i++)
    if(!GAMenu<TYPE>::menulist[i]->RenameOption(label, name))
      return 0;
  label = name;
  return 1;
}

template <class TYPE>
TrueFalse Group<TYPE>::contains(const TYPE* which) const {
  for(int i=0; i<members.capacity(); i++)
    if(members[i] == which) return TF_TRUE;
  return TF_FALSE;
}

template <class TYPE>
void Group<TYPE>::Add(TYPE* i) {
  if(!contains(i))
    members.grow(1, i); // too slow?
}

template <class TYPE>
void Group<TYPE>::Remove(TYPE* i) { 
  members.remove(i); 
}

#endif
