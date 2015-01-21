// -*- C++ -*-
// $RCSfile: popupmenu.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:11 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


class PopUpMenu {
private:
  FL_PUP_ENTRY *items;
  Menu *menu;
  FL_OBJECT *flmenu;
  Vec<MenuOption*> menuoptions;
public:
  // create a popup menu from the commands in Menu, and attach it to
  // the given XForms object.
  PopUpMenu(const Menu&, FL_OBJECT*);
  void callback();
  static void CB_popupmenu(FL_OBJECT*, long);
};

