// -*- C++ -*-
// $RCSfile: groups.h,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2001-05-09 21:59:35 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef GROUPS_H
#define GROUPS_H

#include "vec.h"
#include "charstring.h"
#include "cell_coordinate.h"
#include "menuDef.h"
#include <string.h>

// Pixel and mesh groups are instances of this templated class. They
// need to specialize some of the methods. Common code to the
// specialized methods is in "base" methods. Eg. append(const TYPE&)
// is meant to be specialized, but the specialized version should call
// append_base(const TYPE&).

template <class TYPE>
class Group {
protected:
  CharString name;
  Vec<TYPE> members;
  void install(void);	// put in group menus
  void uninstall(void);
  int weeded; // has weed() been called recently?
public:
  Group() : members(0, BlockSize(10)), weeded(1) {
    // only install named groups! the "selected" group is unnamed.
  }
  
  Group(const CharString &n) : name(n), members(0, BlockSize(10)), weeded(1)
  {
    install();
  }
  
  virtual ~Group() {
    if(name != "") uninstall();	// only named groups were installed
  }
  
  const Vec<TYPE> &membership() const { return members; }
  void append_base(const TYPE &);
  void append_base(const Vec<TYPE>&);

  // These three can be specialized if necessary. The specialization can
  // call append_base(). The non-specialized versions only call
  // append_base().
  virtual void append(const TYPE &);
  virtual void append(const Vec<TYPE>&);
  virtual void append(const Group<TYPE>&);
  
  void remove_base(const TYPE&);
  void remove_base(const Vec<TYPE>&);
  void remove_base(const Group<TYPE>&);
  void clear_base();
  virtual void trim(const TYPE &x) { members.trim(x); }

  void remove_conditional(int (*f)(TYPE&)) {
    members.remove_conditional(f);
  }

  virtual void remove(const TYPE &x);
  virtual void remove(const Vec<TYPE>&x);
  virtual void remove(const Group<TYPE> &grp);
  virtual void clear();

  virtual void replace(const TYPE&, const TYPE&);

  int size() const { return members.capacity(); }
  void setphysicalsize(int n) { members.setphysicalsize(n); }
  
  // get rid of duplicate members
  // this requires that TYPE has operator== and operator<
  void weed();
  static int compare(const void*, const void*);
 
  Group<TYPE> &operator=(const Group<TYPE> &);	// assignment
  
  // conversion from Group<TYPE> to Vec<TYPE> reference
  //operator Vec<TYPE> &();
  //operator const Vec<TYPE> &() const;
//    Vec<TYPE> &vec();
//    const Vec<TYPE> &vec() const;
  
  TYPE &operator[](const int i);
  TYPE operator[](const int i) const;
  
  const CharString &query_name() const { return name; }

// Sort a vector of groups alphabetically by name
  static void sortgroups(Vec<Group<TYPE>*> &);
  static int grpcompare(const void*, const void*); // used by sortgroups
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

/* a menu which lists all the groups. Each group invokes the same
 * function, which must use Waiter::get_command() to find out which
 * group to operate on. The Group constructor and destructor have to
 * update all the menus in the menulist.
 */
 
template <class TYPE>
class GroupMenu : public Menu {
private:
  CharString grouphelp;
  CommandFn (*groupcmd)();
  static Vec<GroupMenu<TYPE>*> menulist; // define this somewhere!
public:
  GroupMenu(const CharString &prmpt, const CharString &hs,
	    CommandFn (*f)())
    : Menu(prmpt), grouphelp(hs), groupcmd(f)
  {
    menulist.grow(1, this);
  }
  virtual ~GroupMenu() {
    menulist.remove(this);
  }
  friend class Group<TYPE>;
};

#include "groups.C"

#endif
