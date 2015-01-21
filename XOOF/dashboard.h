// -*- C++ -*-
// $RCSfile: dashboard.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2006-01-12 16:59:53 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "drawer.h"
#include "forms.h"
#include "rubberband.h"
#include "charstring.h"

class Drawer;
class OofCanvas;
class XCursor;

// The Dashboard contains a group of FL_OBJECTs, and a Menu of
// commands that simulates the action of the objects (for use in log
// files). It has virtual functions that handle mouse clicks.

// The show() and hide() virtual functions can change the canvas
// cursor. 

// Each XForms object in a Dashboard needs the following functions:
// A static or global XForms callback (FL_OBJECT*, long)
// A local callback that is called by the global callback. This one
//    should perform the function and log it by calling
//    garcon()->log(menucommand() + "whatever");
// A Menu CommandFn or Variable callback, called when the Dashboard object is
//    invoked from a log file. This must also be global or static,
//    and must use get_formdrawer() to find out which FormDrawer
//    to use. This callback can call the local callback if convenient.
//
// The Dashboard's constructor has to initialize each object (put buttons
// in the right position, fill in input fields, etc). It also has to put the
// Menu or Variable callback into DashBoard::menu.

class Dashboard {
private:
  CharString name_;
  void activate() { active_ = 1; }
  void deactivate() { active_ = 0; }
  int active_;
protected:
  CharString menucommand() const;
  FormDrawer *formdrawer() const;
  OofCanvas *canvas() const; 
  void setcursor(const XCursor&);
  void unsetcursor();
  int hidden;
  virtual void show() = 0;
  virtual void hide() = 0;
  // callbacks (especially for input objects) should check active()
  // before doing anything. XForms input objects will invoke their
  // callbacks when the object loses input focus, so switching
  // dashboards will invoke callbacks. The dashboard is deactivate()d
  // before it is hidden to get around this problem.
  int active() const { return active_; }
public:
  Drawer *drawer;
  Dashboard(const CharString &nm, Drawer *drwr, RubberBandType rbt=RB_NONE)
    : name_(nm),
      drawer(drwr),
      rubberbandtype(rbt),
      menu(nm),
      active_(0)
  {}
  virtual ~Dashboard() {};
  void show_dashboard();
  void hide_dashboard();
  const CharString &name() const { return name_; }
  Menu menu; // menu of simulated mice
  RubberBandType rubberbandtype;

  // the default behavior is to ignore mouse clicks
  virtual void clickhandler(const MouseClick&) {} // simple click
  virtual void draghandler(const MouseClick&, const MouseClick&) {} // drag

  friend class FormDrawer;
};


#endif
