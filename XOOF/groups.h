// -*- C++ -*-
// $RCSfile: groups.h,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2001-06-04 14:05:27 $


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

template <class TYPE> class GroupIterator;

template <class TYPE>
class Group {
protected:
  Vec<TYPE*> members;
  CharString label;
public:
  Group(const CharString&);
  virtual ~Group();
  bool rename(const CharString &);
  
  virtual void Add(TYPE* i);
  virtual void Remove(TYPE* i);
  TrueFalse contains(const TYPE* i) const;
  int size() const { return members.capacity(); }
  const CharString &name() const { return label; }
  TYPE *operator[](int i) const { return members[i]; }

  friend class GroupIterator<TYPE>;
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


template <class TYPE>
class GroupMenu : public Menu {
  /* a menu which lists groups.
   * all entries in the menu call the same function (groupcmd)
   * but are called with different names (the name of the group)
   * so they can use Waiter::get_command to find out what they're
   * supposed to do.
   */
public:
  CharString grouphelp;
  CommandFn (*groupcmd)();
  static Vec<GroupMenu<TYPE> *> menulist;
  
  GroupMenu(const CharString &prmpt, const CharString &hs, CommandFn (*f)())
    : Menu(prmpt), grouphelp(hs), groupcmd(f)
  {
    menulist.grow(1, this);
  }
  virtual ~GroupMenu() {
    menulist.remove(this);
  }
};

// Just like GroupMenu<>, but the command is a CommandM.  Different
// CommandM's, taking different constructor arguments, have to
// override addcmd in different ways.

template <class TYPE>
class GAMenu : public Menu {
public:
  static Vec<GAMenu<TYPE> *> menulist;
  CharString grouphelp;
  GAMenu(const CharString &prmpt, const CharString &hs)
    : Menu(prmpt), grouphelp(hs)
  {
    menulist.grow(1, this);
  }
  virtual ~GAMenu() {
    menulist.remove(this);
  }
  virtual void addcmd(const CharString &nm) = 0;
};

// COMMAND takes a single string as an argument. The string is the
// name of the group.
template <class TYPE, class COMMAND>
class GroupArgMenu : public GAMenu<TYPE> {
public:
  COMMAND *cmd;
  GroupArgMenu(const CharString &prmpt, const CharString &hs)
    : GAMenu<TYPE>(prmpt, hs)
  { }
  virtual void addcmd(const CharString &nm) {
    AddCommand(new COMMAND(nm));
  }
};

#include "groups.C"

#endif
