// -*- C++ -*-
// $RCSfile: drawer.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-02-18 02:34:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef DRAWER_H
#define DRAWER_H

class Drawer;
class DrawerRegistration;
class Dashboard;

#include "color.h"
#include "forms.h"
#include "grid.h"
#include "menuDef.h"
#include "vec.h"

class Dashboard;
class MouseClick;
class FormDrawer;

class Drawer {
private:
  Vec<Dashboard*> dashboard;
  int holdchanged;   // is a redraw needed when hold is released?
  int holdflag;
  //  double xscale, yscale;    // computed when clip region changes
  void new_grid(Grid*);     // for replacing grid

protected:
  FormDrawer *formdrawer;
  Dashboard *current_dashboard;
  void add_dashboard(Dashboard*);
  void make_dashboard_menu();
  FL_OBJECT *dashboard_menu;  // copy of pointer in FormDrawer
  Grid *grid;

  // list of all drawers
  static Vec<Drawer*> alldrawers;

  virtual void draw() = 0;
  virtual void drawelements() = 0;
  virtual void drawedges() = 0;
  virtual void drawedges(const Element*) = 0;
  virtual void drawelement(const Element*) = 0;
  virtual void drawnodes() = 0;
  virtual void drawnode(const Node*) = 0;

  virtual void draw_ps(FILE*, const Rectangle&) = 0;
  virtual void drawelements_ps(FILE*, const Rectangle&) = 0;
  virtual void drawedges_ps(FILE*, const Rectangle&) const = 0;
  virtual void drawnodes_ps(FILE*, const Rectangle&) const = 0;
//   virtual void drawedges_ps(const Element*, FILE*, const Rectangle&) const = 0;
//   virtual void drawelement_ps(const Element*, FILE*, const Rectangle&) const=0;
//   virtual void drawnode(const Node*) const = 0;

  // colors used by all drawers, but pixel value may differ from
  // colormap to colormap
  virtual unsigned long wallpaper_fg() const = 0;
  virtual unsigned long wallpaper_bg() const = 0;
  virtual unsigned long black() const = 0;
  virtual unsigned long white() const = 0;
  void wallpaper(const TrueFalse&); // use wallpaper?

  // routines that alert the drawer that something has changed
  virtual void gridchanged() {}
  virtual void elementchanged(const Element*) {}
  virtual void nodechanged(const Node*) {}

  int hold();          // refrain from performing updates?
  void set_hold();
  void release_hold();
  Dashboard *set_current_dashboard(int);
  virtual unsigned long rubberbandpixel() const = 0;

  // called whenever the coordinates of the visible ragion change
  virtual void scrollcallback(const MeshCoord&, const MeshCoord&) {}
public:
  Drawer(Grid*, FormDrawer*);
  virtual ~Drawer();
  FL_FORM *form;
  static int exists(long);  // check before receiving events!
  virtual int showingempties() const = 0;

  friend class FormDrawer;
  friend class Dashboard;
  friend class OofCanvas;
};

// ----------------------------------------------------- //

class DrawerRegistration {
private:
  CharString the_name;
  Drawer *(*create)(Grid*, FormDrawer*);
  double order_; // used to order drawers in menu
  static int sorted;
  static void sort();
public:
  DrawerRegistration(const CharString &name,
		     Drawer *(*create)(Grid*, FormDrawer*),
		     double order);
  const CharString &name() const { return the_name; }
  double order() const { return order_; }
  friend class FormDrawer;
  friend class DrawerCmd;
};

extern Vec<DrawerRegistration*> &drawerregistry();


#endif


