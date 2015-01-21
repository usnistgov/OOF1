// -*- C++ -*-
// $RCSfile: popupmenu.C,v $
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

PopUpMenu::PopUpMenu(const Menu &mnu, FL_OBJECT *obj)
  : menu(mnu),
    flmenu(obj),
    items(0)
{
  fl_set_object_callback(obj, CB_popupmenu, (long) this);
  for(int i=0; i<menu.
}

void PopUpMenu::callback() {
  int which = fl_get_menu(flmenu);
  if(which > 0)
    menuoptions[which]->?;
}

void PopUpMenu::CB_popupmenu(FL_OBJECT *obj, long arg) {
  ((PopUpMenu*) arg)->callback();
}

