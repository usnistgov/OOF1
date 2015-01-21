// -*- C++ -*-
// $RCSfile: groups.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-12-22 15:15:25 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Groups are named lists of pointers to things.

#ifndef GROUPS_H
#define GROUPS_H

#include "iterator.h"
#include "menuDef.h"
#include "truefalse.h"
#include "vec.h"

template <class TYPE>
class Group {
protected:
  CharString label;
public:
  Group(const CharString&);
  virtual ~Group();
  bool rename(const CharString &);
  
  Vec<TYPE *> members;
  // should be private, but iterator needs access
  
  virtual void Add(TYPE* i);
  TrueFalse contains(const TYPE* i) const;
  int size() const { return members.capacity(); }
  const CharString &name() const { return label; }
  TYPE *operator[](int i) const { return members[i]; }
};

template <class TYPE>
class GroupIterator : public Iterator<TYPE> {
    private:
	int index;
	const Group<TYPE> &group;
    public:
	GroupIterator(const Group<TYPE> &grp) : index(0), group(grp) {}
	virtual TYPE* operator()() {
	    if(index < group.size())
		return group.members[index++];
	    else
		return 0;
	}
	virtual void reset() { index = 0; }
};

#include "groups.C"

#endif
