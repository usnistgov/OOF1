// -*- C++ -*-
// $RCSfile: dashboard.h,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2001-02-18 02:29:34 $

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

#include "charstring.h"
#include "enum.h"
#include "forms.h"
#include "image_drawer2.h"
#include "rubberband.h"
#include <stdio.h>

class Goof;
class ImageForm;
class ImageCanvas;
class Menu;
class MouseClick;
class Rectangle;
class XCursor;

// The Dashboard contains a group of FL_OBJECTs, and a Menu of
// commands that simulates the action of the objects (for use in log
// files). It has virtual functions that handle mouse clicks.

// The actual FL_OBJECTs can all be stored elsewhere. Dashboard only
// needs to know a pointer to the XForms group to which the objects
// belong.

// The show() and hide() virtual functions can change the canvas
// cursor. Showing and hiding the objects is taken care of by
// show_dashboard() and hide_dashboard(), which call show() and
// hide(). show() and hide() just have to do things (if any) that are
// particular to the derived class.

// Each XForms object in a Dashboard needs the following functions:
// A static or global XForms callback (FL_OBJECT*, long)
//    It can use ImageForm::objdash(FL_OBJECT*) to find out which dashboard
//    the invoked object lives in.
// A local callback that is called by the global callback. This one
//    should perform the function and log it by calling
//    garcon()->log(menucommand() + "whatever"); 
// A Menu CommandFn or Variable callback, called when the Dashboard object is
//    invoked from a log file. This must also be global or static,
//    and must use get_formdrawer() or get_dashboard() to find out which
//    FormDrawer or Dashboard to use. This callback can call the local
//    callback if convenient.
// A button on the dashboard can call a Command with arguments (a
// CommandM) for the purpose of setting parameters for other dashboard
// actions.  In this case, the CommandM (or a pointer to it) should be
// a member of the Dashboard class, and the arguments should *live in
// the Dashboard*, not in the CommandM. This is because the menu
// routines execute a temporary copy of the command, so if the
// arguments live in the CommandM they won't be accessible to the
// Dashboard. If the command's arguments don't need to be accessible
// to other parts of the Dashboard, they can be stored within the
// Command. To display a CommandM window, use
//   CommandM cmd;
//   cmd.makecmdform(menucommand())->show(0);
// Dashboard::menucommand() generates a path to the command, so that it
// can be logged correctly.
//
// The Dashboard's constructor has to has to put the Menu or Variable
// callback into DashBoard::menu.
//
// The dashboards are all created in the ImageForm constructor in imageform.C.
// The ImageForm class contains pointers to all the dashboards.

class Dashboard {
private:
  CharString name_;
  void activate() { active_ = 1; }
  void deactivate() { active_ = 0; }
  int active_;
  FL_OBJECT *objectgroup;
  ImageForm *form;
protected:
  CharString menucommand() const;
  ImageForm *imageform() const;
  FD_image_drawer *fdui() const;
  ImageCanvas *canvas() const; 
  Goof *goof() const; 
  Menu *menu;
  Menu *imageformmenu() const;
  void setcursor(const XCursor&);
  void unsetcursor();
  int hidden;

  // show() and hide() return 1 if the canvas should be redrawn, eg if
  // the dashboard has drawn something that should go away when the
  // dashboard goes away.
  virtual int show() { return 0; }
  virtual int hide() { return 0; }

  // callbacks (especially for input objects) should check active()
  // before doing anything. XForms input objects will invoke their
  // callbacks when the object loses input focus, so switching
  // dashboards will invoke callbacks. The dashboard is deactivate()d
  // before it is hidden to get around this problem.
  int active() const { return active_; }

  // draw things on the canvas that should only be seen when this
  // dashboard is active.
  virtual void draw_extras() {} // not const, because changes a pixel value
  virtual void draw_extras_ps(FILE*, const Rectangle&) const {}
public:
  Dashboard(const CharString &nm, ImageForm *imf, FL_OBJECT *grp,
	    RubberBandType rbt=RB_NONE);
  virtual ~Dashboard() {};
  int show_dashboard();
  int hide_dashboard();
  virtual void update() {};
  const CharString &name() const { return name_; }
  Enum<RubberBandType> rubberbandtype;

  // the default behavior is to ignore mouse clicks
  // simple click
  virtual void clickhandler(const MouseClick&) {}
  // drag (down, move, up)
  virtual void draghandler(const MouseClick&, const MouseClick&) {}
  // drag in progress (down, move, but not up yet)
  virtual void movehandler(const MouseClick&, const MouseClick&) {}

  friend class ImageForm;
  friend class ImageFormGfx;
  friend class ImageFormTxt;
};


#endif
