// -*- C++ -*-
// $RCSfile: groups.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-21 22:16:53 $

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
}

template <class TYPE> Group<TYPE>::~Group() {}

template <class TYPE>
bool Group<TYPE>::rename(const CharString &name) {
  label = name;
  return 1;
}

template <class TYPE>
void Group<TYPE>::Add(TYPE* i) {
  members.grow(1, i);
}

#endif
