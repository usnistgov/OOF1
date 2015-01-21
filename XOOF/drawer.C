// -*- C++ -*-
// $RCSfile: drawer.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:54 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "colorutils.h"
#include "dashboard.h"
#include "drawer.h"
#include "formdrawer.h"
#include "mouseclick.h"
#include "oofstipple.xbm"
#include "oofcanvas.h"
#include "robomouse.h"

DrawerRegistration::DrawerRegistration(const CharString &nm,
				       Drawer *(*func)(Grid*, FormDrawer*),
				       double ord)
  : the_name(nm), create(func), order_(ord)
{
  drawerregistry().grow(1, this);
  sorted = 0;
}

Vec<DrawerRegistration*> &drawerregistry() {
  static Vec<DrawerRegistration*> list;
  return list;
}

static int draworder(const void *d1, const void *d2) {
  double o1 = (*(DrawerRegistration**) d1)->order();
  double o2 = (*(DrawerRegistration**) d2)->order();
  if(o1 < o2) return -1;
  if(o1 > o2) return 1;
  return 0;
}

int DrawerRegistration::sorted = 0;

void DrawerRegistration::sort() {
  if(sorted) return;
  qsort(&(drawerregistry()[0]), drawerregistry().capacity(),
	sizeof(DrawerRegistration*), draworder);
  sorted = 1;
}

// -------------------------------------------------- //

Vec<Drawer*> Drawer::alldrawers;

Drawer::Drawer(Grid *g, FormDrawer *fd)
  : grid(g), 
    form(fd->the_form),
    formdrawer(fd),
    dashboard_menu(fd->current_dashboard_menu),
    current_dashboard(0),
    holdflag(0), holdchanged(0)
{
  fl_clear_choice(dashboard_menu);
  alldrawers.grow(1, this);
}

Drawer::~Drawer() {
  alldrawers.remove(this);
}

int Drawer::exists(long ptr) {
  for(int i=0; i<alldrawers.capacity(); i++)
    if(alldrawers[i] == (Drawer*) ptr)
      return 1;
  return 0;
}

// -------------------------------------------------- //

void Drawer::add_dashboard(Dashboard *dshbrd) {
  dashboard.grow(1, dshbrd);
  //  fl_addto_choice(dashboard_menu, dshbrd->name());
  dshbrd->hide_dashboard();
}

void Drawer::make_dashboard_menu() {
  fl_clear_choice(dashboard_menu);
  for(int i=0; i<dashboard.capacity(); i++)
    fl_addto_choice(dashboard_menu, dashboard[i]->name());
}

Dashboard *Drawer::set_current_dashboard(int which) {
  if(which < 0 || which >= dashboard.capacity()) {
    garcon()->msout << ms_error
		    << "Drawer::set_current_dashboard: no such dashboard "
		    << which << endl << ms_normal;
    return 0;
  }
  fl_freeze_form(form);
  if(current_dashboard)
    current_dashboard->hide_dashboard();
  current_dashboard = dashboard[which];
  current_dashboard->show_dashboard();
  fl_unfreeze_form(form);

  // replace old dashboard command menu
  formdrawer->menu->AddSubMenu("dashboard",
			       "simulate dashboard " +
			       current_dashboard->name(),
			       &current_dashboard->menu);
  return current_dashboard;
}

// -------------------------------------------------- //


/* hold() is called to find out if the Drawer is accepting redraws.
 * If it isn't accepting them, then it has to remember to redraw itself
 * when the hold is released.
 */ 

int Drawer::hold() {
  holdchanged = holdflag;
  return holdflag;
}

void Drawer::set_hold() {
  holdflag = 1;
}

void Drawer::release_hold() {
  holdflag = 0;
  if(holdchanged)
    formdrawer->draw();
  holdchanged = 0;
}

// -------------------------------------------------- //

void Drawer::new_grid(Grid *g) {
  grid = g;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void Drawer::wallpaper(const TrueFalse &use_it) {
  if(use_it) 
    formdrawer->canvas->setstipple(oofstipple_width, oofstipple_height,
				   oofstipple_bits,
				   wallpaper_fg(), wallpaper_bg());
  else
    formdrawer->canvas->setfillcolor(white());
}

