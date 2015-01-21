// -*- C++ -*-
// $RCSfile: robomouse.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:13 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Commands that simulate the mouse clicks in the form drawers. These
// commands are in the robomouse menus. One menu is installed in the
// "graphics" menu each time a FormDrawer is created. The name of the
// menu is the CharString FormDrawer::formdrawer_id. It's a hidden
// menu, unless OOF is compiled with -DDEBUG. See formdrawer.C.

#include "filename.h"
#include "formdrawer.h"
#include "mouseclick.h"
#include "robomouse.h"

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// look at the command path to find which formdrawer should be used
FormDrawer *get_formdrawer() {
  int depth = garcon()->get_depth();
  CharString cmd(garcon()->get_command(depth - 2));
  return FormDrawer::allformdrawers[atoi(cmd)];
}

Dashboard *get_dashboard() {
  return get_formdrawer()->current_dashboard;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// mouse click on the canvas
class ClickCommand : public CommandM {
private:
  MouseClick click;
public:
  ClickCommand()
    : CommandM("mouse", "simulate a click on the canvas")
  {
    AddArgument(this, "click", click);
  }
  virtual void func() {
    // by the time we get here, this command has been logged already,
    // so the simulated mouse click shouldn't be logged.
    garcon()->inhibit_logging();
    get_formdrawer()->robomouse(click);
    garcon()->disinhibit_logging();
  }
  virtual CommandM *clone() const {
    ClickCommand *cc = new ClickCommand;
    cc->click = click;
    return cc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// scroll bars

class ScrollCommand : public CommandM {
private:
  int dx, dy;
public:
  ScrollCommand()
    : CommandM("scroll", "simulate scroll bars"),
      dx(0), dy(0)
  {
    AddArgument(this, "dx", dx);
    AddArgument(this, "dy", dy);
  }
  virtual void func() {
    get_formdrawer()->roboscroll(dx, dy);
  }
  virtual CommandM *clone() const {
    ScrollCommand *sc = new ScrollCommand;
    sc->dx = dx;
    sc->dy = dy;
    return sc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// zoom button

class ZoomButtonCmd : public CommandM {
private:
  int whichbutton;
public:
  ZoomButtonCmd()
    : CommandM("zoom", "simulate the scrollcanvas zoom button"),
      whichbutton(1)
  {
    AddArgument(this, "button", whichbutton);
  }
  virtual void func() {
    get_formdrawer()->robozoom(whichbutton);
  }
  virtual CommandM* clone() const {
    ZoomButtonCmd *zc = new ZoomButtonCmd;
    zc->whichbutton = whichbutton;
    return zc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// window resize

class ResizeCmd : public CommandM {
private:
  int w, h;
public:
  ResizeCmd()
    : CommandM("resize", "change the size of the Drawer")
  {
    AddArgument(this, "w", w);
    AddArgument(this, "h", h);
  }
  virtual void func() {
    garcon()->inhibit_logging();
    get_formdrawer()->resize(w, h);
    garcon()->disinhibit_logging();
  }
  virtual CommandM* clone() const {
    ResizeCmd *rc = new ResizeCmd;
    rc->w = w;
    rc->h = h;
    return rc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Drawer selector
static CharString drawer;

static void setdrawer(const CharString&) {
  get_formdrawer()->set_drawer(drawer);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// close button

static CommandFn close() {
  get_formdrawer()->close();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// save button

class SaveCmd : public CommandM {
private:
  FileName filename;
public:
  SaveCmd() : CommandM("save", "save the picture")
  {
    AddArgument(this, "filename", filename);
  }
  virtual void func() {
    get_formdrawer()->save(filename);
  }
  virtual CommandM *clone() const {
    SaveCmd *sc = new SaveCmd;
    sc->filename = filename;
    return sc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// hold button

static TrueFalse holding = 0;

static void sethold(const CharString&) {
  get_formdrawer()->set_hold(holding);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Dashboard selector
static int dashboard = 0;

static void setdashboard(const CharString &) {
  get_formdrawer()->set_current_dashboard(dashboard);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Each FormDrawer gets its own copy of the robomouse menu, so this
// routine does NOT return a pointer to a static Menu.

// THIS IS  A MEMORY LEAK! When  the FormDrawer  is deleted, it should
// clean up the menu!

Menu *robomousemenu() {
  Menu *menu = new Menu("robomouse");
  menu->AddCommand(new ClickCommand);
  menu->AddCommand(new ScrollCommand);
  menu->AddCommand(new ZoomButtonCmd);
  menu->AddCommand(new SaveCmd);
  menu->AddCommand(new ResizeCmd);
  menu->AddCommand("close", "close the drawer", close);
  AddVariable(menu, "drawer", "set drawer", drawer, setdrawer);
  AddVariable(menu, "dashboardnumber", "set dashboard", dashboard,
	      setdashboard);
  AddVariable(menu, "hold", "set drawer's hold button", holding, sethold);
  return menu;
}
