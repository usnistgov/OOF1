// -*- C++ -*-
// $RCSfile: formdrawer.C,v $
// $Revision: 1.5 $
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


#include "colorutils.h"
#include "create_form_drawer.h"
#include "dashboard.h"
#include "formdrawer.h"
#include "graphics.h"
#include "griditerator.h"
#include "menucolor.h"
#include "node.h"
#include "oofcanvas.h"
#include "robomouse.h"
#include "select.h"
#include "stdlib.h"
#include <stdio.h>

static const double MARGINS = 0.05;

Vec<FormDrawer*> FormDrawer::allformdrawers;
int FormDrawer::formdrawercount = 0;

// call back for window manager close event

int FormDrawer::wm_close(FL_FORM*, void *ptr) {
  ((FormDrawer*) ptr)->close();
  return FL_IGNORE;
}

FormDrawer::FormDrawer(Grid *cg) :
  Form("OOF drawer"),
  grid(cg),
  current_drawer_number(0),
  current_dashboard_number(0),
  current_dashboard(0),
  formdrawer_id(to_charstring(formdrawercount++))
{
  allformdrawers.grow(1, this);
  menu = graphicsmenu()->AddSubMenu(formdrawer_id, "simulated mouse",
				    robomousemenu(),
#ifdef DEBUG
				    MOPT_NOPRINT
#else
				    MOPT_SECRET|MOPT_NOPRINT
#endif
				    );

  create_form_form_drawer(this);
  fl_set_object_color(background, COL_BACKGROUND, 0);

  xcolormap = gfxinfo.colormap();

  canvas = new OofCanvas(this, image_region->x, image_region->y,
			 image_region->w, image_region->h,
			 xcolormap, CB_scrollcallback);
  canvas->set_xyscrollcallbacks(CB_xscrollcallback, CB_yscrollcallback, this);
  canvas->set_zoombutton_callback(CB_zoombutton, this);
  canvas->set_resize_callback(CB_resize, this);

  fl_set_form_atclose(the_form, wm_close, this);

  // set coordinate system for canvas
  if(grid) {
    Rectangle rect = grid->query_extreme_pos();
    rect.swell(1+MARGINS);
    rect.aspect(canvas->vwidth(), canvas->vheight());
    canvas->setcoords(rect.xmin, rect.xmax, rect.ymin, rect.ymax);
  }
  else {
    canvas->setcoords(0, 1, 0, 1);
  }

  canvas->clearpixmap();

  // put choices of drawers in the menu
  DrawerRegistration::sort();
  int ndrawers = drawerregistry().capacity();
  for(int i=0; i<ndrawers; i++)
    fl_addto_choice(type_selector, drawerregistry()[i]->name());
  fl_set_choice_fontsize(type_selector, FL_MEDIUM_SIZE);
  fl_set_choice_fontstyle(type_selector, FL_BOLD_STYLE);

  // set up initial drawer
  fl_set_choice(type_selector, current_drawer_number+1);
  set_current_drawer(current_drawer_number);

  // there's too much overlap between this bit and set_current_drawer(int).
  // this should call that.   now it does...
//    drawer_list.resize(current_drawer_number+1, 0);
//    current_drawer = drawerregistry()[current_drawer_number]->create(grid, this);
//    drawer_list[current_drawer_number] = current_drawer;
//    current_drawer->make_dashboard_menu();
//    current_dashboard = 
//      current_drawer->set_current_dashboard(current_dashboard_number);
//    fl_set_button(hold_button, current_drawer->hold());

  fl_set_choice_fontstyle(current_dashboard_menu, FL_BOLD_STYLE);

  fl_set_object_shortcutkey(homebutton, XK_Home);

  garcon()->graphics_off();
  current_drawer->draw();
  garcon()->graphics_on();
  canvas->do_scrollcallback();
  show(NULL);
  current_dashboard->show_dashboard();
       // set_current_dashboard may not have been able to do
       // everything it wanted if the window didn't exist yet.
}

FormDrawer::~FormDrawer() {
  allformdrawers.remove(this);
  graphicsmenu()->RemoveOption(formdrawer_id);
  hide();
  delete canvas;
  for(int i=0; i<drawer_list.capacity(); i++)
    if(drawer_list[i])
      delete drawer_list[i];
  delete menu; // robomouse menu
  // individual objects in the form are deleted automatically by fl_free_form(),
  // which is called by ~Form()
  gfxinfo.freecolormap(xcolormap);
}

void FormDrawer::closeall() {	// static
  for(int i=allformdrawers.capacity()-1; i>=0; i--)
    allformdrawers[i]->close();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::set_current_drawer() {
  int which = fl_get_choice(type_selector) - 1;
  if(which != current_drawer_number) {
    set_current_drawer(which);
    draw();
    garcon()->log("graphics " + formdrawer_id + " drawer = \"" +
		  drawerregistry()[which]->name() + "\"");
  }
}

void FormDrawer::set_current_drawer(int which) {
  fl_freeze_form(the_form);
  /* dashboard is deleted when current_drawer is deleted, and if it's
   * not deactivated first, spurious callbacks can occur.  */
  if(current_dashboard)
    current_dashboard->hide_dashboard();

  // make sure there's a spot for this drawer in the drawer_list
  if(drawer_list.capacity() <= which) 
    drawer_list.grow(which-drawer_list.capacity()+1, 0);


  if(drawer_list[which] != 0)	// if drawer's already been created, use it...
    current_drawer = drawer_list[which];
  else {			// ... or else create it
    current_drawer = drawerregistry()[which]->create(grid, this);
    drawer_list[which] = current_drawer;
  }

  current_drawer->make_dashboard_menu();

  current_drawer_number = which;
  current_dashboard_number = 0;
  current_dashboard =
    current_drawer->set_current_dashboard(current_dashboard_number);
  fl_set_button(hold_button, current_drawer->hold());
  fl_unfreeze_form(the_form);
}

void FormDrawer::set_drawer(const CharString &which) {
  for(int i=0; i<drawerregistry().capacity(); i++) {
    if(drawerregistry()[i]->name() == which) {
      fl_set_choice(type_selector, i+1);
      set_current_drawer(i);
      draw();
      return;
    }
  }
  garcon()->msout << ms_error << "Can't change drawer to " << which << "!"
		  << endl << ms_normal;
}

void FDCB_set_current_drawer(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((FormDrawer*) ptr)->set_current_drawer();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::set_current_dashboard() {
  int which = fl_get_choice(current_dashboard_menu) - 1;
  if(which != current_dashboard_number) {
    set_current_dashboard(which);
    garcon()->log("graphics " + formdrawer_id + " dashboardnumber = " +
		  to_charstring(which));
  }
}

void FormDrawer::set_current_dashboard(int which) {
  Dashboard *db = current_drawer->set_current_dashboard(which);
  if(db) {
    current_dashboard_number = which;
    current_dashboard = db;
    fl_set_choice(current_dashboard_menu, current_dashboard_number+1);
  }
}

void FDCB_set_current_dashboard(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((FormDrawer*) ptr)->set_current_dashboard();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::hold() {
  TrueFalse state = fl_get_button(hold_button);
  set_hold(state);
  garcon()->log("graphics " + formdrawer_id + " hold = " +
		to_charstring(state));
}

void FormDrawer::set_hold(const TrueFalse &flag) {
  if(flag) {
    canvas->hold();
    current_drawer->set_hold();
    // this could be called to simulate a click, so we have to set the
    // state of the button.
    fl_set_button(hold_button, 1);
  }
  else {
    canvas->release_hold();
    current_drawer->release_hold();
    fl_set_button(hold_button, 0);
  }
}

void FDCB_set_hold_image(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((FormDrawer*) ptr)->hold();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::save() {
  static char *lastname = 0;
  const char *filename;
  if(lastname)
    filename = fl_show_fselector("Enter PPM or PS file name", ".", "*.ppm",
				 lastname);
  else
    filename = fl_show_fselector("Enter PPM or PS file name", ".", "*.ppm", "");
  if(filename) {
    lastname = strdup(filename);
    save(filename);
    garcon()->log("graphics " + formdrawer_id + " save filename=" + filename);
  }
}

void FormDrawer::save(const CharString &filename) const {
  FILE *file = fopen(filename, "w");
  if(file) {
    if(filename.tail(3) == ".ps" || filename.tail(4) == ".eps")
      draw_ps(file);
    else
      canvas->ppmfile(file);
    fclose(file);
  }
}

void FDCB_save(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((FormDrawer*) ptr)->save();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::close() {
  garcon()->log("graphics " + formdrawer_id + " close");
  current_dashboard->deactivate(); /* avoid xforms spurious callback bug */
  hide();
  delete this;
}

void FDCB_close(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((FormDrawer*) ptr)->close();
}


//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::home() {
  formwaiter().show_homeform();
}

void FDCB_home(FL_OBJECT*, long ptr) {
  if(Form::exists(ptr))
    ((FormDrawer*) ptr)->home();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// This is called whenever the coordinates of the visible region of
// the canvas change, for whatever reason.

// static
void FormDrawer::CB_scrollcallback(const MeshCoord &ul, const MeshCoord &lr,
				   void *ptr)
{
  ((FormDrawer*) ptr)->scrollcallback(ul, lr);
}

void FormDrawer::scrollcallback(const MeshCoord &ul, const MeshCoord &lr) const
{
  current_drawer->scrollcallback(ul, lr);
}

// These are called ONLY when the scroll bars are used on the canvas.

// static 
void FormDrawer::CB_xscrollcallback(int dx, void *ptr) {
  ((FormDrawer*) ptr)->xyscrollcallback(dx, 0);
}

// static
void FormDrawer::CB_yscrollcallback(int dy, void *ptr) {
  ((FormDrawer*) ptr)->xyscrollcallback(0, dy);
}

void FormDrawer::xyscrollcallback(int dx, int dy) {
  garcon()->log("graphics " + formdrawer_id +
		" scroll dx=" + to_charstring(dx) +
		" dy=" + to_charstring(dy));
}

// static
void FormDrawer::CB_zoombutton(int whichbutton, void *ptr) {
  ((FormDrawer*) ptr)->zoombutton_callback(whichbutton);
}

void FormDrawer::zoombutton_callback(int whichbutton) {
  garcon()->log("graphics " + formdrawer_id +
		" zoom button=" + to_charstring(whichbutton));
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Record window size change. The change is detected by the canvas,
// which calls CB_resize, but the actual window size is what's
// recorded in the log file, since that's what can easily be changed
// automatically.

void FormDrawer::CB_resize(int, int, void *ptr) {
  ((FormDrawer*) ptr)->resize_callback();
}

void FormDrawer::resize_callback() {
  garcon()->log("graphics " + formdrawer_id +
		" resize w=" + to_charstring(the_form->w) +
		" h=" + to_charstring(the_form->h));
}

// resize the form
void FormDrawer::resize(int w, int h) {
 // unless minsize is set explicitly, the window manager won't let the
 // form be shrunk below w by h.
  fl_set_form_minsize(the_form, 100, 100);

  fl_set_form_size(the_form, w, h);
  fl_redraw_form(the_form); // should be sufficient, but doesn't do anything

  // This is an ugly kludge. Changing the size of the form generates
  // some exposure events, which have to be handled so that all the
  // objects on the form are redrawn properly. I don't know how many
  // events have to be processed. 100 is probably too many.
  // Maybe this kludge won't be necessary when using the newest XForms.
  fl_deactivate_all_forms();
  for(int i=0; i<100; i++)
    fl_check_only_forms();
  fl_activate_all_forms();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::drag_rubberband() {
  canvas->drag_rubberband();
}

void FormDrawer::drag_pan() {
  canvas->drag_pan();
}

void FormDrawer::drag_nothing() {
  canvas->drag_nothing();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//


void FormDrawer::center_image() {
  canvas->fullzoom();
}


void FormDrawer::redraw_image() {  // redraw despite the hold flag
  int holding = current_drawer->hold();
  if(holding) current_drawer->release_hold();
  draw();
  if(holding) current_drawer->set_hold();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::GridUpdated(const Grid *g) { // static
  Rectangle gridrect(g->query_extreme_pos());
  gridrect.swell(1+MARGINS);
  for(int i=0; i<allformdrawers.capacity(); i++)
    if(allformdrawers[i]->grid == g) {
      FormDrawer *fd = allformdrawers[i];
      fd->gridchanged();
      fd->canvas->setcoords_resize(gridrect.xmin, gridrect.xmax,
				   gridrect.ymin, gridrect.ymax);
    }
}

void FormDrawer::gridchanged() const {
  for(int i=0; i<drawer_list.capacity(); i++)
    if(drawer_list[i])
      drawer_list[i]->gridchanged();
}

void FormDrawer::redraw(const Grid *g) { // static
  for(int i=0; i<allformdrawers.capacity(); i++)
    if(allformdrawers[i]->grid == g) {
      FormDrawer *fd = allformdrawers[i];
      fd->canvas->redraw();
      fd->canvas->refresh();
    }
}

void FormDrawer::hold_all() { // static
  for(int i=0; i<allformdrawers.capacity(); i++)
    allformdrawers[i]->set_hold(TF_TRUE);
}

void FormDrawer::release_hold_all() { // static
  for(int i=0; i<allformdrawers.capacity(); i++)
    allformdrawers[i]->set_hold(TF_FALSE);
}

void FormDrawer::ElementUpdated(const Grid *g, const Element *el) { // static
  for(int i=0; i<allformdrawers.capacity(); i++)
    if(allformdrawers[i]->grid == g) {
      allformdrawers[i]->current_drawer->elementchanged(el);
      allformdrawers[i]->current_drawer->drawelement(el);
      allformdrawers[i]->canvas->refresh();
    }
}

void FormDrawer::NodeUpdated(const Grid *g, const Node *node) {	// static
  for(int i=0; i<allformdrawers.capacity(); i++)
    if(allformdrawers[i]->grid == g) {
      allformdrawers[i]->current_drawer->nodechanged(node);
      allformdrawers[i]->current_drawer->drawnode(node);
      allformdrawers[i]->canvas->refresh();
    }
}

void FormDrawer::replace_grid(Grid *oldgrid, Grid *newgrid) { // static
  for(int i=0; i<allformdrawers.capacity(); i++)
    if(allformdrawers[i]->grid == oldgrid) {
      if(newgrid && !oldgrid)
	fl_hide_object(allformdrawers[i]->image_region);
      allformdrawers[i]->new_grid(newgrid); // draws
    }
}

void FormDrawer::new_grid(Grid *g) {
  grid = g;
  for(int i=0; i<drawer_list.capacity(); i++)
    if(drawer_list[i])
      drawer_list[i]->new_grid(g);
  Rectangle rect = grid->query_extreme_pos();
  rect.swell(1+MARGINS);
  rect.aspect(canvas->vwidth(), canvas->vheight());
  gridchanged();
  canvas->setcoords_reset(rect.xmin, rect.xmax, rect.ymin, rect.ymax); // draws
}

void FormDrawer::center_image(Grid *grid) { // static
  for(int i=0; i<allformdrawers.capacity(); i++)
    if(allformdrawers[i]->grid == grid)
      allformdrawers[i]->center_image();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

int FormDrawer::showingempties() const {
  return current_drawer->showingempties();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::wallpaper(const TrueFalse &use_it) {
  for(int i=0; i<Drawer::alldrawers.capacity(); i++)
    Drawer::alldrawers[i]->wallpaper(use_it);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

Vec<XPoint> FormDrawer::workspace;

Vec<XPoint> &FormDrawer::xpoints(const Element *el) const {
  int ncorners = el->corner.capacity();
  workspace.resize(ncorners);
  for(int i=0; i<ncorners; i++)
    workspace[i] = canvas->xpoint(el->corner[i]->position(grid->enhancement()));
  return workspace;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// draw element using specified pixel value

void FormDrawer::paintelement(const Element *e, unsigned long pixel) {
  int corners = e->corner.capacity();
  Vec<XPoint> &p = xpoints(e);
  XSetForeground(gfxinfo.display(), canvas->gc(), pixel);
  XFillPolygon(gfxinfo.display(), canvas->pixmap(), canvas->gc(),
	       &p[0], corners, Convex, CoordModeOrigin);
}

void FormDrawer::paintedges(const Element *e, unsigned long pixel) 
{
  int corners = e->corner.capacity();
  Vec<XPoint> &p = xpoints(e);
  
  p.grow(1, p[0]);  
  XSetForeground(gfxinfo.display(), canvas->gc(), pixel);
  XDrawLines(gfxinfo.display(), canvas->pixmap(), canvas->gc(),
	     &p[0], corners+1, CoordModeOrigin);
}

void FormDrawer::paintnode(const Node *node, int radius, unsigned long pixel) {
  XPoint p = canvas->xpoint(node->position(grid->enhancement()));
  XSetForeground(gfxinfo.display(), canvas->gc(), pixel);
  XFillArc(gfxinfo.display(), canvas->pixmap(), canvas->gc(),
	   p.x-radius, p.y-radius,
	   2*radius, 2*radius,
	   0, 23040);                           // 23040 = 360*64
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::draw() {
  if(grid) {
    garcon()->graphics_off();
    current_drawer->draw(); // draws to pixmap
    canvas->refresh();      // copies pixmap to screen
    garcon()->graphics_on();
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::setcursor(const XCursor &cursor) {
  if(the_form->visible && canvas->visible()) {
    Window w = canvas->window();
    if(w)
      XDefineCursor(gfxinfo.display(), w, cursor);
  }
}

void FormDrawer::unsetcursor() {
  if(the_form->visible && canvas->visible()) {
    Window w = canvas->window();
    if(w)
      XDefineCursor(gfxinfo.display(), w, None);
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

FormDrawer *FormDrawerIterator::operator()() {
  if(current == FormDrawer::allformdrawers.capacity()) return 0;
  FormDrawer *fd = FormDrawer::allformdrawers[current];
  current++;
  return fd;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void FormDrawer::robomouse(const MouseClick &click) {
  canvas->robomouse(click);
}

void FormDrawer::roboscroll(int dx, int dy) {
  canvas->scroll(dx, dy);
}

void FormDrawer::robozoom(int whichbutton) {
  canvas->zoom_button(whichbutton);
}
