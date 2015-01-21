// -*- C++ -*-
// $RCSfile: restrict.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-30 19:23:29 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// restrict operations to a set of pixels

#include "goof.h"
#include "imageform.h"
#include "pixelgroups.h"
#include "restrict.h"

static CommandFn activate_all() {
  if(current_goof) {
    current_goof->remove_restrictions();
    current_goof->redraw();
  }
}

static CommandFn activate_selection() {
  if(current_goof) {
    current_goof->remove_restrictions();
    current_goof->activate_selection(Goof::AM_INTERSECT);
    current_goof->redraw();
  }
}

static CommandFn activate_intersect_selection() {
  if(current_goof) {
    current_goof->activate_selection(Goof::AM_INTERSECT);
    current_goof->redraw();
  }
}

static CommandFn activate_union_selection() {
  if(current_goof) {
    current_goof->activate_selection(Goof::AM_UNION);
    current_goof->redraw();
  }
}

static CommandFn activate_group() {
  if(current_goof) {
    CharString whichgroup = garcon()->get_command();
    PixelGroup *grp = current_goof->get_group(whichgroup);
    if(grp) {
      current_goof->remove_restrictions();
      current_goof->activate_group(*grp, Goof::AM_INTERSECT);
      current_goof->redraw();
    }
  }
}

static CommandFn activate_intersect_group() {
  if(current_goof) {
    CharString whichgroup = garcon()->get_command();
    PixelGroup *grp = current_goof->get_group(whichgroup);
    if(grp) {
      current_goof->activate_group(*grp, Goof::AM_INTERSECT);
      current_goof->redraw();
    }
  }
}

static CommandFn activate_union_group() {
  if(current_goof) {
    CharString whichgroup = garcon()->get_command();
    PixelGroup *grp = current_goof->get_group(whichgroup);
    if(grp) {
      current_goof->activate_group(*grp, Goof::AM_UNION);
      current_goof->redraw();
    }
  }
}

static CommandFn select_active_area() { // select the active area
  if(current_goof) {
    current_goof->select_active_area();
    current_goof->redraw();
  }
}

static CommandFn unselect_inactive_area() {
  if(current_goof) {
    current_goof->unselect_inactive_area();
    current_goof->redraw();
  }
}

CommandFn activerecall() {
  // This cmd is put into activerecallmenu by Goof::store_active_area
  if(current_goof) {
    CharString name = garcon()->get_command();
    if(!current_goof->recall_active_area(name))
      garcon()->msout << ms_error << "Can't recall area " << name << endl
		      << ms_normal;
    else
      current_goof->redraw();
  }
}

class StoreActiveCmd : public CommandM {
private:
  CharString name;
  static int count;
public:
  StoreActiveCmd()
    : CommandM("store", "store current active area"),
      name("area" + to_charstring(count++))
  {
    AddArgument(this, "name", name);
  }
  virtual CommandFn func() {
    if(current_goof)
      current_goof->store_active_area(name);
  }
  virtual CommandM *clone() const {
    StoreActiveCmd *sac = new StoreActiveCmd;
    //sac->name = name;
    return sac;
  }
};

int StoreActiveCmd::count = 0;

double ActiveArea::fade = 0.3;

static void redraw(const CharString&) {
  if(current_goof) {
    current_goof->apply_all_forms(&ImageForm::set_all_colors);
    current_goof->redraw();
  }
}

Menu *activatemenu() {
  static Menu *menu = 0;
  if(!menu) {
    menu = new Menu("active_area");

    Menu *setmenu =
      menu->AddSubMenu("set", "set active region to a group",
		       new GroupMenu<Cell_coordinate>(
				      "set active region",
				      "allow activities only within group",
				      activate_group));
    setmenu->AddCommand("selection",
			"allow activities only within selected pixels",
			activate_selection);


    Menu *intersectmenu =
      menu->AddSubMenu("reduce", "forbid operations outside a group",
		       new GroupMenu<Cell_coordinate>("reduce active region",
					    "forbid operations outside group",
					    activate_intersect_group));
    intersectmenu->AddCommand("selection",
			      "forbid operations outside selected pixels",
			      activate_intersect_selection);

    Menu *unionmenu =
      menu->AddSubMenu("expand", "allow operations within a group",
		       new GroupMenu<Cell_coordinate>("expand active region",
				    "allow operations within group",
				    activate_union_group));
    unionmenu->AddCommand("selection",
			  "allow operations within selected pixels",
			  activate_union_selection);

    menu->AddCommand("all", "remove all restrictions", activate_all);
    menu->AddCommand("select", "select active area", select_active_area);
    menu->AddCommand("unselect", "unselect inactive area",
		     unselect_inactive_area);
    menu->AddCommand(new StoreActiveCmd);
    menu->AddSubMenu("recall", "recall a stored active area",
		     activerecallmenu());
    AddVariable(menu, "overrride", "activate all pixels?",
		Goof::active_override, redraw);
    AddVariable(menu, "fade", "amount to fade inactive area",
		ActiveArea::fade, redraw);
  }
  return menu;
}

Menu *activerecallmenu() {
  static Menu *menu = 0;
  if(!menu) {
    menu = new Menu("recall");
  }
  return menu;
}
