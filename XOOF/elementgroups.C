// -*- C++ -*-
// $RCSfile: elementgroups.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2005-02-17 22:22:47 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "elementgroups.h"
#include "grid.h"
#include "griditerator.h"
#include "menuDef.h"
#include "parameters.h"
#include "select.h"
#include <iostream.h>

template <class Element> Vec<GroupMenu<Element>*> GroupMenu<Element>::menulist;
template <class Element> Vec<GAMenu<Element>*> GAMenu<Element>::menulist;

ElementGroup::~ElementGroup() {
    for(int i=0; i<size(); i++)	// remove this group from group list in elements
	members[i]->groups.remove(this);
    grid->elementgroup.remove(this);	// remove this group from list in grid
}

void ElementGroup::Add(Element *el) {
  if(!contains(el)) {
    members.grow(1, el);       // put this element in group
    el->groups.grow(1, this);  // put this group in element's grouplist
  }
}

void ElementGroup::Remove(Element *el) {
  members.remove(el);
  el->groups.remove(this);
}

ostream &operator<<(ostream &os, const ElementGroup &eg) {
    int i = eg.size();
    os << eg.name() << " (" << i << (i == 1? " element" : " elements") << ")";
    return os;
}

// ----------------------------------------------------------------- //

// get the group from the command name

ElementGroup *Egroupname() {
    CharString whichgroup = garcon()->get_command();
    ElementGroup *n = grid->getEgroup(whichgroup, TF_FALSE);
    if(!n)
      garcon()->msout << ms_error << "There is no group " << whichgroup << "!"
		      << endl << ms_normal;
    return n;
}

// ----------------------------------------------------------------- //

// group management commands

class NewElementGroupCmd : public CommandM {   // create a new group
private:
  CharString name;
public:
  NewElementGroupCmd() : CommandM("new", "create a new element group"), 
  name("groupname") {
    AddArgument(this, "name", name);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      char *c;
      int modified = 0;
      while(c = (char*) strchr(name, ' ')) { // cast required by Sun
	*c = '_';
	modified = 1;
      }
      if(modified) garcon()->msout << ms_info << "Group name changed to \""
				   << name << "\"" << endl << ms_normal;
      (void) grid->getEgroup(name, TF_TRUE);
    }
  }
  virtual CommandM *clone() const {
    NewElementGroupCmd *negc = new NewElementGroupCmd;
    negc->name = name;
    return negc;
  }
};

class RenameEGroupCmd : public CommandM {
private:
  CharString newname;
public:
  RenameEGroupCmd(const CharString group)
    : CommandM(group, "rename an element group"),
      newname("newname")
  {
    AddArgument(this, "newname", newname);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      ElementGroup *eg = Egroupname();
      if(!eg->rename(newname))
	garcon()->msout << ms_error << "There already is a group called "
			<< newname << "!" << endl << ms_normal;
    }
  }
  virtual CommandM *clone() const {
    RenameEGroupCmd *regc = new RenameEGroupCmd(name);
    regc->newname = newname;
    return regc;
  }
};

static CommandFn deletegroup() {    // remove a group
    if(gridinit()) {
	ElementGroup *eg = Egroupname();
	if(eg) delete eg;
    }
}

static CommandFn showgroup() {
    if(gridinit()) {
	ElementGroup *eg = Egroupname();
	if(eg)
	    garcon()->msout << *eg << endl;
    }
}

static CommandFn addselectedelems() { // add the selected elements to the group
    ElementGroup *eg = Egroupname();
    if(gridinit() && eg) {
	ElementIterator iter(EI_SELECTED, grid);
	Element *e;
	while(e = iter())
	    eg->Add(e);
    }
}

static CommandFn removeselectedelems() {
    ElementGroup *eg = Egroupname();
    if(gridinit() && eg) {
	ElementIterator iter(EI_SELECTED, grid);
	Element *e;
	while(e = iter())
	    eg->Remove(e);
    }
}

CommandFn selectelemgroup() {	// select all elements in the group
  ElementGroup *eg = Egroupname();
  if(gridinit() && eg) {
    GroupIterator<Element> iter(*eg);
    Element *e;
    while(e = iter())
      e->select();
  }
  grid->redraw();
}

static CommandFn unselectelemgroup() {	// unselect all elements in the group
  ElementGroup *eg = Egroupname();
  if(gridinit() && eg) {
    GroupIterator<Element> iter(*eg);
    Element *e;
    while(e = iter())
      e->unselect();
  }
  grid->redraw();
}

// ----------------------------------------------------------- //

Menu *elemgroupmenu() {
    static Menu *em = 0;
    if(em) return em;
    em = new Menu("element groups");
    em->AddCommand(new NewElementGroupCmd);
    em->AddSubMenu("show", "show size of element groups", 
		   new GroupMenu<Element>("show", "show size of element group",
					  showgroup));
    em->AddSubMenu("delete", "delete an element group", 
	new GroupMenu<Element>("delete", "delete element group", deletegroup));
    em->AddSubMenu("rename", "rename an element group",
		   new GroupArgMenu<Element, RenameEGroupCmd>("rename group",
							      "rename group"));
    em->AddSubMenu("add_elements",
        "add selected elements to an element group",
        new GroupMenu<Element>("add elements", "add selected elements to group",
            addselectedelems));
    em->AddSubMenu("remove_elements",
		   "remove selected elements from an element group",
		   new GroupMenu<Element>("remove elements",
					  "remove selected elements from group",
					  removeselectedelems));
    em->AddSubMenu("select", "select and unselect elements", selectelemmenu());
    Menu *unselectmenu =
      em->AddSubMenu("unselect", "unselect all elements in a group",
		     new GroupMenu<Element>("unselect",
					    "unselect all elements in group",
					    unselectelemgroup));
    unselectmenu->AddCommand("all", "unselect all elements",
			     unselectallelements);
    return em;
}

