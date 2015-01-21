// -*- C++ -*-
// $RCSfile: formdrawer.h,v $
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


#ifndef FD_form_drawer_h
#define FD_form_drawer_h

class FormDrawer;

#include "drawer.h"
#include "grid.h"
#include "menuDef.h"
#include "menuform.h"
#include "shape.h"
#include <X11/Xlib.h>

class Dashboard;
class MeshDrawer;
class MouseClick;
class OofCanvas;
class XCursor;

class FormDrawer : public Form {
private:
  static int formdrawercount; // for creating unique formdrawer_id
  CharString formdrawer_id;
  Menu *menu; // menu containing commands for simulated mouse events, etc.
  void new_grid(Grid*);
  int select(const MV_Vector_double&, int);
  int select(const Rectangle&, int);
  static Vec<XPoint> workspace; // for X drawing, used by xpoints()
  void setcursor(const XCursor&);  // set cursor for drawing region
  void unsetcursor();

  // constant objects in the form
  FL_OBJECT *background;
  FL_OBJECT *image_region;
  FL_OBJECT *hold_button;
  FL_OBJECT *type_selector;
  FL_OBJECT *current_dashboard_menu;
  FL_OBJECT *dashboard_area;
  FL_OBJECT *closebutton;
  FL_OBJECT *savebutton;
  FL_OBJECT *homebutton;

  OofCanvas *canvas;
  Colormap xcolormap;

  Grid *grid;
  Drawer *current_drawer;
  Vec<Drawer*> drawer_list;
  int current_drawer_number;
  int current_dashboard_number;
  Dashboard *current_dashboard;
  void gridchanged() const;

  void hold();
  void set_current_drawer();    // uses forms to find which drawer
  void set_current_drawer(int); // is told which drawer
  void set_current_dashboard();
  void save();
  void home();			// bring up a root menu

  static void CB_scrollcallback(const MeshCoord&, const MeshCoord&, void*);
  void scrollcallback(const MeshCoord&, const MeshCoord&) const;

  // call back routines used for logging scroll events in canvas
  static void CB_xscrollcallback(int, void*);
  static void CB_yscrollcallback(int, void*);
  void xyscrollcallback(int, int);
  static void CB_zoombutton(int, void*);
  void zoombutton_callback(int);

  static void CB_resize(int, int, void*);
  void resize_callback();

public:
  FormDrawer(Grid*);
  ~FormDrawer();
  static Vec<FormDrawer*> allformdrawers;

  // actions on all FormDrawers, or all corresponding to a specified Grid:
  static void closeall();
  static void GridUpdated(const Grid*);
  static void redraw(const Grid*); // redraw all formdrawers with this grid
  static void hold_all();	// inhibit drawing on all formdrawers
  static void release_hold_all();
  static void ElementUpdated(const Grid*, const Element*);
  static void NodeUpdated(const Grid*, const Node*);
  static void replace_grid(Grid*, Grid*);
  static void center_image(Grid*);

  static int wm_close(FL_FORM*, void*); // handle window manager event

  void draw();
  void draw_ps(FILE*) const;
  void close();

  Grid *whichgrid() { return grid; }

  int showingempties() const;

  void paintelement(const Element*, unsigned long);
  void paintedges(const Element*, unsigned long);
  void paintnode(const Node*, int, unsigned long);

//   // paint with given enhancement of displacements
  // NOT IMPLEMENTED THIS WAY
//   void paintelement(const Element*, unsigned long, double);
//   void paintedges(const Element*, unsigned long, double);
//   void paintnode(const Node*, int, unsigned long, double);

  void paintelement_ps(const Element*, const Color&, FILE*, const Rectangle&)
    const;
  void paintedges_ps(const Element*, FILE*, const Rectangle&)
    const;
  void paintnode_ps(const Node*, int, FILE*, const Rectangle&)
    const;

  // convert grid coordinates of all corners of element to window coords
  Vec<XPoint> &xpoints(const Element*) const;
  Vec<XPoint> &xpoints(const Element*, double) const; // enhance displacement

  // set what to do when the mouse is dragged
  void drag_rubberband();
  void drag_pan();
  void drag_nothing();

  void robomouse(const MouseClick&);  // simulate a mouse click
  void roboscroll(int, int);    // simulate scroll bars
  void robozoom(int);           // simulate zoom button
  void set_drawer(const CharString&); // simulate Drawer Selector
  void set_current_dashboard(int); // simulate Dashboard Selector
  void save(const CharString&) const;  // simulate save button
  void set_hold(const TrueFalse&);
  void center_image();
  void redraw_image();
  void resize(int, int); // change size of form

  // cosmetics
  void wallpaper(const TrueFalse &);

  /**** Callback routines ****/
  friend void FDCB_set_current_drawer(FL_OBJECT*, long);
  friend void FDCB_set_current_dashboard(FL_OBJECT*, long);
  friend void FDCB_set_hold_image(FL_OBJECT*, long);
  friend void FDCB_close(FL_OBJECT*, long);
  friend void FDCB_save(FL_OBJECT*, long);
  friend void FDCB_home(FL_OBJECT*, long);

  friend class OofCanvas;
  friend class Drawer;
  friend class Dashboard;
  friend class MeshDrawer;
  friend class SelectDrawer;
  friend class ColorDrawer;
  friend void create_form_form_drawer(FormDrawer*);
  friend class FormDrawerIterator;
  friend Dashboard *get_dashboard();
};

class FormDrawerIterator {
private:
  int current;
public:
  FormDrawerIterator() : current(0) {}
  FormDrawer *operator()();
};

#endif
