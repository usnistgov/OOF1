// -*- C++ -*-
// $RCSfile: groups.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-10-27 18:17:29 $

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
#include "goof.h"
#include "menuDef.h"
#include "ppm2oof.h"
#include "stdlib.h"

// have to define this elsewhere for each TYPE
// Vec<GroupMenu<TYPE>*> GroupMenu<TYPE>::menulist;

// maybe not...
template <class TYPE> Vec<GroupMenu<TYPE>*> GroupMenu<TYPE>::menulist;

// ---------------------------------------------------- //

template <class TYPE>
void Group<TYPE>::install() {
    for(int i=0; i<GroupMenu<TYPE>::menulist.capacity(); i++) {
	GroupMenu<TYPE>::menulist[i]->AddCommand(name, 
	    GroupMenu<TYPE>::menulist[i]->grouphelp + " " + name, 
	    GroupMenu<TYPE>::menulist[i]->groupcmd);
    }
}

template <class TYPE>
void Group<TYPE>::uninstall() {
    for(int i=0; i<GroupMenu<TYPE>::menulist.capacity(); i++) {
	GroupMenu<TYPE>::menulist[i]->RemoveOption(name);
    }
}

template <class TYPE>
Group<TYPE> &Group<TYPE>::operator=(const Group<TYPE> &g) {
    if(&g != this) {
	name = g.name;
	members = g.members;
	weeded = g.weeded;
    }
    return *this;
}

template <class TYPE>
void Group<TYPE>::clear_base() {
  members.resize(0);
  weeded = 1;
}

template <class TYPE>
void Group<TYPE>::clear() {
  clear_base();
}

template <class TYPE>
int Group<TYPE>::compare(const void *a, const void *b) {
    TYPE *aa = (TYPE*) a;
    TYPE *bb = (TYPE*) b;
    if(*aa < *bb) return -1;
    if(*aa == *bb) return 0;
    return 1;
}

template <class TYPE>
void Group<TYPE>::weed() {
  if(weeded) return;
  weeded = 1;
  if(members.capacity() == 0)
    return;
  ::qsort(members, members.capacity(), sizeof(TYPE), compare);
  int nunique = 1;
  for(int i=1; i<members.capacity(); i++)
    if(members[i] != members[i-1])
      members[nunique++] = members[i];
  members.resize(nunique);
}

template <class TYPE>
void Group<TYPE>::append_base(const Vec<TYPE> &list) {
  // add list of members to a group
  int oldsize = members.capacity();
  members.resize(oldsize + list.capacity());
  (void) memcpy(&members[oldsize], &list[0],
		list.capacity()*sizeof(TYPE));
  weeded = 0;
}

template <class TYPE>
void Group<TYPE>::append_base(const TYPE &x) {
  weeded = 0;
  members.grow(1, x);
}

template <class TYPE>
void Group<TYPE>::append(const TYPE &x) {
  append_base(x);
}

template <class TYPE>
void Group<TYPE>::append(const Vec<TYPE> &x) {
  append_base(x);
}

template <class TYPE>
void Group<TYPE>::append(const Group<TYPE> &grp) {
  append(grp.members);
}

template <class TYPE>
void Group<TYPE>::remove_base(const TYPE &x) {
  members.remove(x);
}

template <class TYPE>
void Group<TYPE>::remove(const TYPE &x) {
  remove_base(x);
}

template <class TYPE>
void Group<TYPE>::remove_base(const Vec<TYPE> &c_rejects) {
  weed();			// sort and weed the group
  // sort the list of elements to be rejected
  Vec<TYPE> rejects(c_rejects);	// non-const copy
  // don't want std::qsort 
  ::qsort(rejects, rejects.capacity(), sizeof(TYPE), compare);
  // flag elements to be rejected
  Vec<int> rejected(members.capacity(), 0);
  int g = 0;
  int r = 0;
  while(g < members.capacity() && r < rejects.capacity()) {
    if(r < rejects.capacity()-1 && rejects[r] == rejects[r+1])
      // could be duplicates in reject list
      r++;
    else if(members[g] == rejects[r]) {
      rejected[g] = 1;
      g++;
      r++;
    }
    else if(members[g] < rejects[r])
      g++;
    else
      r++;
  }
  members.remove_conditional(rejected);
}

template <class TYPE>
void Group<TYPE>::remove_base(const Group<TYPE> &grp) {
  remove_base(grp.members);
}

template <class TYPE>
void Group<TYPE>::remove(const Vec<TYPE> &x) {
  remove_base(x);
}

template <class TYPE>
void Group<TYPE>::remove(const Group<TYPE> &grp) {
  remove_base(grp);
}

template <class TYPE>
void Group<TYPE>::replace(const TYPE &obsolete, const TYPE &newfangled) {
  weeded = 0;
  members.replace(obsolete, newfangled);
}

//  template <class TYPE>
//  //Group<TYPE>::operator Vec<TYPE> &() {
//  Vec<TYPE> &Group<TYPE>::vec() {
//    return members;
//  }

//  template <class TYPE>
//  //Group<TYPE>::operator const Vec<TYPE> &() const {
//  const Vec<TYPE> &Group<TYPE>::vec() const {
//    return members;
//  }
	
template <class TYPE>
TYPE &Group<TYPE>::operator[](const int i) {
  return members[i];
}

template <class TYPE>
TYPE Group<TYPE>::operator[](const int i) const {
  return members[i];
}

// Sort a vector of groups alphabetically by name

template <class TYPE>
int Group<TYPE>::grpcompare(const void *one, const void *two) {
  Group<TYPE> *grp1 = *(Group<TYPE>**) one;
  Group<TYPE> *grp2 = *(Group<TYPE>**) two;
  return grp1->query_name() != grp2->query_name();
  // hack! operator!=(CharString, CharString) just returns strcmp()
}

template <class TYPE>
void Group<TYPE>::sortgroups(Vec<Group<TYPE>*> &grplist) {
  ::qsort(&grplist[0], grplist.capacity(), sizeof(Group<TYPE>*),
	Group<TYPE>::grpcompare);
}

#endif

