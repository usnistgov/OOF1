// -*- C++ -*-
// $RCSfile: imageform.C,v $
// $Revision: 1.19 $
// $Author: langer $
// $Date: 2001-04-19 01:39:35 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "adaptmesh.h"
#include "autodash.h"
#include "amtriangle.h"
#include "color.h"
#include "colorutils.h"
#include "coordinates.h"
#include "cursor.h"
#include "displaydash.h"
#include "filename.h"
#include "forms.h"
#include "gfx.h"
#include "goof.h"
#include "imagecanvas.h"
#include "imageform.h"
#include "image_drawer2.h"
#include "imagemandash.h"
#include "imagemoddash.h"
#include "infodash.h"
#include "menucolor.h"
#include "ppm2oof.h"
#include "rastack.h"
#include "selectdash.h"
#include "shape.h"
#include "stdlib.h"

#include "oofstipple.xbm"

Vec<ImageForm*> ImageForm::all_imageforms;
int ImageForm::imageformcount = 0;
int ImageForm::dflt_dashboard_no = 0;

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Pixel values used in RW colormaps. "pixel" here means "index into a
// colormap", not "coordinate in an image". That's the X definition.
// The actual RGB values are assigned to Color objects by the
// dashboards that control the color. Most of them are in
// displaydash.C.

unsigned long const ImageFormGfx::BLACKPIXEL = 0;
unsigned long const ImageFormGfx::WHITEPIXEL = 1;
unsigned long const ImageFormGfx::SELECTEDPIXEL = 2;
unsigned long const ImageFormGfx::FIREWALLPIXEL = 3;
unsigned long const ImageFormGfx::MESHPIXEL = 4;
unsigned long const ImageFormGfx::SELMESHPIXEL = 5;
unsigned long const ImageFormGfx::SELMESHFADEPIXEL = 6;
unsigned long const ImageFormGfx::NOMATERIALPIXEL = 7;
unsigned long const ImageFormGfx::RUBBERBANDPIXEL = 8;
unsigned long const ImageFormGfx::WALLPAPER_FG = 9;
unsigned long const ImageFormGfx::WALLPAPER_BG = 10;
// colors used in InfoDashboard:
unsigned long const ImageFormGfx::HIGHLIGHTPIXEL = 11; // for mesh triangle
unsigned long const ImageFormGfx::CLICKEDPIXEL = 12; // cell clicked on

int ImageFormGfx::nreserved_colors = 13; // number of colors listed above 
Color ImageFormGfx::rubberband_color_dflt(255, 255, 255);

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

ImageForm::ImageForm(Goof *gf, const Image &firstimage)
  : goof(gf),
    imageform_id(to_charstring(imageformcount)),
    current_dashboard_no(dflt_dashboard_no),
    beenclicked(0),
    inhibit_(0)
{
  imageformcount++;
  goof->imageforms.grow(1, this);
  all_imageforms.grow(1, this);

  // The menu must be created before the dashboards because the
  // dashboards install commands in the menu.
  menu = graphicsmenu()->AddSubMenu(imageform_id, "simulated mouse", "graphics",
#ifndef DEBUG
				    MOPT_SECRET |
#endif
				    MOPT_NOPRINT
				    );

  dashboardmenu = menu->AddSubMenu("dashboard", "simulated dashboard mouse");

  AddVariable(menu, "dashboardnumber", "set dashboard",
	      current_dashboard_no, setdashboard_cmd);
  menu->AddCommand(&robomousecmd);
  menu->AddCommand(&zoombuttoncmd);
  menu->AddCommand(&scrollcmd);
  menu->AddCommand(&resizecmd);
  menu->AddCommand("close", "close this window", close_cmd);
  menu->AddCommand(&savecmd);
}

ImageFormGfx::ImageFormGfx(Goof *gf, const Image &firstimage)
  : Form("PPM2OOF " + to_charstring(imageformcount)),
    ImageForm(gf, firstimage),
    //    logmotions(0),
    canvas(0), // so that it's not referred to until it's created
    rubberband_color(rubberband_color_dflt),
    black(0, 0, 0),
    white(255, 255, 255),
    wallpaper_fg(wallpaper_foreground),
    wallpaper_bg(wallpaper_background)
{
  // Create graphics
  fdui = create_form_image_drawer();
  fdui->vdata = this;
  the_form = fdui->image_drawer;

  fl_set_object_color(fdui->background, COL_BACKGROUND, 0);
  fl_set_object_shortcutkey(fdui->button_home, XK_Home);

  // The colormap must be created before the dashboards, because
  // colors are stored and set in the dashboards.
  xcolormap = gfxinfo.colormap();

  // The dashboards must be created before the canvas, because the
  // canvas has callbacks that refer to the dashboards (in particular,
  // when the pixmap is created the limits of the visible region are
  // displayed in the CoordDashboard).

  // The order in which the dashboards are installed with adddashboard
  // is the order in which they appear in the dashboard selector.

  coorddash = new CoordDashboard(this, fdui->coordinates);
  displaydash = new DisplayDashboard(this, fdui->display);
  selectdash = new SelectDashboard(this, fdui->selectmethod);
  pixelinfodash = new PixelInfoDashboard(this, fdui->pixelinfo);
  meshinfodash = new MeshInfoDashboard(this, fdui->meshinfo);
  imagemanager = new ImageManDashboard(this, fdui->imagemanager);
  imagemodification = new ImageModDashboard(this, fdui->imagemodification);
  automation = new AutomationDashboard(this, fdui->automation);

  adddashboard(displaydash);
  adddashboard(coorddash);
  adddashboard(selectdash);
  adddashboard(pixelinfodash);
  adddashboard(meshinfodash);
  adddashboard(imagemodification);
  adddashboard(imagemanager);
  adddashboard(automation);
  fl_set_choice_fontstyle(fdui->current_dashboard_menu, FL_BOLD_STYLE);
  fl_set_choice(fdui->current_dashboard_menu, current_dashboard_no+1);
  currentdashboard = dashboard[current_dashboard_no];

  // create the canvas

  canvas = new ImageCanvas(this, fdui->image_region->x, fdui->image_region->y,
			   fdui->image_region->w, fdui->image_region->h,
			   goof->query_width(), goof->query_height(),
			   coorddash->scalefactor,
			   &CB_scrollcallback);

  fl_set_form_atclose(the_form, wm_close, this);

  // allow form to be shrunk below original size
  fl_set_form_minsize(the_form, 100, 100);

  canvas->set_colormap(xcolormap);

  // These colors aren't set in the dashboards
  getpixel(white, WHITEPIXEL);
  getpixel(black, BLACKPIXEL);
  getpixel(wallpaper_bg, WALLPAPER_BG);
  getpixel(wallpaper_fg, WALLPAPER_FG);
  getpixel(rubberband_color, RUBBERBANDPIXEL);

  set_all_colors_(); // won't be needed once all colors defs are in dashboards ?

  canvas->set_zoombutton_callback(CB_zoombutton, this);
  canvas->set_xyscrollcallbacks(CB_xscroll, CB_yscroll, this);
  canvas->set_resize_callback(CB_resize, this);
  canvas->setstipple(oofstipple_width, oofstipple_height, oofstipple_bits,
		     wallpaper_fg.pixel, wallpaper_bg.pixel);

  // the dashboards can't be hidden or shown until after the canvas is
  // created, because switching dashboards changes how the canvas
  // handles mouse clicks.

  // when displaying on an SGI running IRIX6.4, an X error is
  // generated during the first hide_dashboard() ???

  for(int i=0; i<dashboard.capacity(); i++)
    dashboard[i]->hide_dashboard();

  // Put the given image on the stack.
  // This makes a copy of the image!
  imagemodification->imagestack.push(firstimage);
  update_gallery_listing(goof->gallery);
  fl_select_browser_line(fdui->browser_imagelist, goof->gallerysize());

  show(0);

  // do these after show() so that the window exists, and the cursor can be set
  currentdashboard->show_dashboard();
  currentdashboard->activate();
  draw();
  canvas->do_scrollcallback();
}

//----------------------

ImageFormTxt::ImageFormTxt(Goof *gf, const Image &firstimage)
  : ImageForm(gf, firstimage)
{
  // The order in which the dashboards are installed with adddashboard
  // is the order in which they appear in the dashboard selector.

  coorddash = new CoordDashboard(this, 0);
  displaydash = new DisplayDashboard(this, 0);
  selectdash = new SelectDashboard(this, 0);
  pixelinfodash = new PixelInfoDashboard(this, 0);
  meshinfodash = new MeshInfoDashboard(this, 0);
  imagemanager = new ImageManDashboard(this, 0);
  imagemodification = new ImageModDashboard(this, 0);
  automation = new AutomationDashboard(this, 0);

  adddashboard(displaydash);
  adddashboard(coorddash);
  adddashboard(selectdash);
  adddashboard(pixelinfodash);
  adddashboard(meshinfodash);
  adddashboard(imagemodification);
  adddashboard(imagemanager);
  adddashboard(automation);
  currentdashboard = dashboard[current_dashboard_no];

  // Put the given image on the stack.
  // This makes a copy of the image!
  imagemodification->imagestack.push(firstimage);
  update_gallery_listing(goof->gallery);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


// put a dashboard in the menu and in the list of dashboards
void ImageFormGfx::adddashboard(Dashboard *db) {
  dashboard.grow(1, db);
  fl_addto_choice(fdui->current_dashboard_menu, db->name_.charstar());
}

void ImageFormTxt::adddashboard(Dashboard *db) {
  dashboard.grow(1, db);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

ImageForm::~ImageForm() {
  for(int i=0; i<dashboard.capacity(); i++)
    delete dashboard[i];
  menu->RemoveOption("dashboardnumber");
  menu->RemoveOption("close");
  delete menu; // robomouse menu
  graphicsmenu()->RemoveOption(imageform_id);
  goof->imageforms.remove(this);
  // don't remove this from all_imageforms, or get_imageform() won't
  // work, but zero out the dangling pointer.
  for(int i=0; i<all_imageforms.capacity(); i++)
    if(all_imageforms[i] == this)
      all_imageforms[i] = 0;
}

ImageFormGfx::~ImageFormGfx() {
  gfxinfo.freecolormap(xcolormap);
  delete canvas;
  free(fdui);
}

ImageFormTxt::~ImageFormTxt() {}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int ImageFormGfx::wm_close(FL_FORM*, void *ptr) {  // Window-manager close
  ((ImageFormGfx*) ptr)->close_gui();
  return FL_IGNORE;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::change_goof(Goof *newgoof) {
  goof->imageforms.remove(this);
  newgoof->imageforms.grow(1, this);
  goof = newgoof;
  canvas->resizepixmap(goof->query_width(), goof->query_height());
  canvas->setcoords(0, goof->query_width(), 0, goof->query_height());
  set_picture(*goof->get_image(goof->gallerysize() - 1));
  update_gallery_listing(goof->gallery);
}

void ImageFormTxt::change_goof(Goof *newgoof) {
  goof->imageforms.remove(this);
  newgoof->imageforms.grow(1, this);
  goof = newgoof;
  update_gallery_listing(goof->gallery);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::mesh_created() const {
  displaydash->activate_mesh_controls();
  meshinfodash->mesh_created();
}

void ImageFormGfx::mesh_destroyed() const {
  displaydash->deactivate_mesh_controls();
  meshinfodash->mesh_destroyed();
}

void ImageFormGfx::triangle_destroyed(const AMTriangle *tri) const {
  meshinfodash->triangle_destroyed(tri);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Form an object is in, used by callbacks
ImageFormGfx *ImageFormGfx::objform(FL_OBJECT *obj) {
  return (ImageFormGfx*) ((FD_image_drawer*)(obj->form->fdui))->vdata;
}

// Dashboard an object is in, used by callbacks
Dashboard *ImageFormGfx::objdash(FL_OBJECT *obj) {
  return objform(obj)->current_dashboard();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Get a pointer to a dashboard, using information from a command path.

Dashboard *ImageForm::get_dashboard(const CharString &nm) const {
  for(int i=0; i<dashboard.capacity(); i++)
    if(nm == dashboard[i]->name())
      return dashboard[i];
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

CharString ImageForm::menucommand() const {
  return "graphics " + imageform_id + " ";
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::robomouse(const MouseClick &click) {
  canvas->robomouse(click);
}

void ImageFormTxt::robomouse(const MouseClick &click) {
  // This is an amalgam of ScrollCanvas::robomouse() and
  // ImageCanvas::mouse() to handle scripted mouse clicks in the
  // absence of a gui.
  static MouseClick lastclick;
  if(click.event == MOUSE_DN)
    lastclick = click;

  garcon()->log(menucommand() + "mouse click = " + to_charstring(click));
  if(click.event == MOUSE_DN)
    return;			// don't do anything until mouse goes up
  if(click.event == MOUSE_UP) {
    if(!lastclick) return;	// up but not down makes no sense
    if(click.window != lastclick.window) return;
    if(click.button != lastclick.button) return;
    if(abs(click.xpoint.x - lastclick.xpoint.x) > ImageCanvas::dragmove ||
       abs(click.xpoint.y - lastclick.xpoint.y) > ImageCanvas::dragmove)
      current_dashboard()->draghandler(lastclick, click);
    else
      current_dashboard()->clickhandler(click);
    return;
  }
  if(click.event == MOUSE_MV) {
    // does this have to be handled? I don't think so....
  }
  if(click.event == MOUSE_UP)
    lastclick.event = MOUSE_TRAP;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::draw() {
  if(inhibit_) return;
  current_dashboard()->update();

  // if the material is being displayed by triangles, special things
  // happen, so set a flag if this is the case.
  int trimatdisp = displaydash->material_display_method == MDM_TRIANGLE &&
       displaydash->background == BG_MATERIAL;

  // which image is being drawn?
  if(displaydash->background == BG_IMAGE)
    localimage.copy_image(current_picture());
  else { // BG_MATERIAL
    localimage.copy_image(goof->materialimage);
    goof->mark_nomaterial(localimage, displaydash->nomaterialcolor);
  }

  // mark selections and other modifications
  if(!trimatdisp) {
    if(displaydash->selected_pixels_visible)
      goof->mark_selected_pixels(localimage, displaydash->selectedcolor);
    if(displaydash->firewalls_visible)
      goof->mark_firewalls(localimage, displaydash->firewallcolor);
  }

  // fade the pixels outside the active region
  if(!trimatdisp) {
    ArrayIterator iter(localimage);
    Cell_coordinate pxl;
    while(iter(pxl))
      if(!current_goof->active(pxl))
	localimage[pxl] = localimage[pxl].fade(ActiveArea::fade);
  }

  if(!trimatdisp) {
    localimage.getpixels(reserved_colors(), colormap());
    canvas->loadimage(&localimage);	// copy image to pixmap
  }

  // remaining things are drawn directly onto the pixmap because they
  // should be drawn at the screen resolution, not the image
  // resolution.
  if(goof->mesh()) {
    if(trimatdisp) {
      goof->mesh()->inherit_pixel_materials();
      goof->mesh()->setcolors(displaydash->nomaterialcolor);
      // If drawing material and selection, it's easiest to do them
      // both at once. So pretend that the material color is the
      // selected color for the selected triangles.
      if(displaydash->selected_mesh_visible)
	goof->mesh()->setselectedcolors(displaydash->selectedmeshcolor);
      goof->mesh()->getpixels(reserved_colors(), colormap());
      goof->mesh()->draw_material(*canvas);
    }
    else {
      // Draw selection, but not material
      if(displaydash->selected_mesh_visible)
	goof->mesh()->draw_selected(*canvas, displaydash->selectedmeshcolor,
				    displaydash->selectedmeshfadecolor);
    }
  }
  current_dashboard()->draw_extras();
  if(goof->mesh() && displaydash->mesh_visible)	// draw mesh last!
    goof->mesh()->draw(*canvas, displaydash->meshcolor, displaydash->meshwidth);
  canvas->refresh();		// copy pixmap to screen
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int ImageFormGfx::reserved_colors() const {
  return nreserved_colors;
}

void ImageFormGfx::getpixel(Color &color, unsigned long pixelno) {
  switch(gfxinfo.c_class()) {
  case TrueColor:
    color.RO_pixel();
    break;
  case PseudoColor:
    color.RW_pixel(xcolormap, pixelno);
    break;
  }
}

void ImageFormGfx::set_all_colors_() {
  getpixel(white, WHITEPIXEL);
  getpixel(black, BLACKPIXEL);
  getpixel(wallpaper_fg, WALLPAPER_FG);
  getpixel(wallpaper_bg, WALLPAPER_BG);
  getpixel(displaydash->firewallcolor, FIREWALLPIXEL);
  getpixel(displaydash->selectedcolor, SELECTEDPIXEL);
  getpixel(displaydash->meshcolor, MESHPIXEL);
  getpixel(displaydash->selectedmeshcolor, SELMESHPIXEL);

  displaydash->selectedmeshfadecolor =
    displaydash->selectedmeshcolor.fade(ActiveArea::fade);
  getpixel(displaydash->selectedmeshfadecolor, SELMESHFADEPIXEL);

  getpixel(displaydash->nomaterialcolor, NOMATERIALPIXEL);
  getpixel(rubberband_color, RUBBERBANDPIXEL);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::center_image() {
  canvas->fullzoom();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::setcursor(const XCursor &cursor) {
  if(the_form->visible && canvas->visible()) {
    Window w = canvas->window();
    if(w)
      XDefineCursor(gfxinfo.display(), w, cursor);
  }
}

void ImageFormGfx::unsetcursor() {
  if(the_form->visible && canvas->visible()) {
    Window w = canvas->window();
    if(w)
      XDefineCursor(gfxinfo.display(), w, None);
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::CB_zoombutton(int button, void *ptr) {
  ((ImageFormGfx*) ptr)->zoombutton_callback(button);
}

void ImageFormGfx::zoombutton_callback(int button) {
  garcon()->log(menucommand() + "zoombutton button=" + to_charstring(button));
}

void ImageFormGfx::zoombutton(int button) {
  canvas->zoom_button(button);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::CB_scrollcallback(const MeshCoord &ul, const MeshCoord &lr,
			       void *ptr)
{
  ((ImageFormGfx*) ptr)->scrollcallback(ul, lr);
}

void ImageFormGfx::scrollcallback(const MeshCoord &ul, const MeshCoord &lr) {
  coorddash->show_range(ul, lr);
}

void ImageFormGfx::CB_xscroll(int dx, void *ptr) {
  ((ImageFormGfx*) ptr)->xscroll_callback(dx);
}

void ImageFormGfx::xscroll_callback(int dx) {
  if(dx == 0) return;
  garcon()->log(menucommand() + "scroll dx=" + to_charstring(dx) + " dy=0");
}

void ImageFormGfx::CB_yscroll(int dy, void *ptr) {
  ((ImageFormGfx*) ptr)->yscroll_callback(dy);
}

void ImageFormGfx::yscroll_callback(int dy) {
  if(dy == 0) return;
  garcon()->log(menucommand() + "scroll dx=0 dy=" + to_charstring(dy));
}

void ImageFormGfx::xyscroll(int dx, int dy) {
  canvas->scroll(dx, dy);
}

void ImageFormGfx::CB_resize(int, int, void *ptr) { // size has changed
  ((ImageFormGfx*) ptr)->resize_callback();
}

void ImageFormGfx::resize_callback() {
  garcon()->log(menucommand() + "resize w=" + to_charstring(the_form->w) +
		" h=" + to_charstring(the_form->h));
}

void ImageFormGfx::resize(int w, int h) {  // change size
  // unless minsize is set explicitly, the window manager won't let the
  // form be shrunk below w by h.
  fl_set_form_minsize(the_form, 100, 100);

  fl_set_form_size(the_form, w, h);

  // This is an ugly kludge. Changing the size of the form generates
  // some exposure events, which have to be handled so that all the
  // objects on the form are redrawn properly. I don't know how many
  // events have to be processed. 100 is probably too many.
  fl_deactivate_all_forms();
  for(int i=0; i<100; i++)
    fl_check_only_forms();
  fl_activate_all_forms(); 
}

//  double ImageFormGfx::scalefactor() const {
//    return coorddash->scalefactor;
//  }

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::CB_dashboard(FL_OBJECT *obj, long) {
  objform(obj)->set_dashboard_gui();
}

void ImageFormGfx::set_dashboard_gui() {
  int which = fl_get_choice(fdui->current_dashboard_menu) - 1;
  set_dashboard(which);
  garcon()->log(menucommand() + "dashboardnumber = " + to_charstring(which));
}

// called by menus, which sets current_dashboard_no
void ImageForm::setdashboard_cmd(const CharString&) { // static
  get_imageform()->set_dashboard();
}

// set dashboard to pulldown menu choice, and set default dashboard
// for new windows
void ImageFormGfx::set_dashboard() {
  dflt_dashboard_no = current_dashboard_no;
  fl_set_choice(fdui->current_dashboard_menu, dflt_dashboard_no+1);
  set_dashboard(current_dashboard_no);
}

void ImageFormTxt::set_dashboard() {
  dflt_dashboard_no = current_dashboard_no;
  currentdashboard = dashboard[current_dashboard_no];
}

void ImageFormGfx::set_dashboard(int which) {
  if(currentdashboard != dashboard[which]) {
    fl_freeze_form(the_form);
    dflt_dashboard_no = which;
    int redraw = currentdashboard->hide_dashboard();
    fl_redraw_object(fdui->dashboard_area); // cover up redrawing mistakes
    current_dashboard_no = which;
    currentdashboard = dashboard[current_dashboard_no];
    redraw |= currentdashboard->show_dashboard();
    if(redraw)
      draw();
    fl_unfreeze_form(the_form);
  }
}

Dashboard *ImageForm::current_dashboard() const {
  return currentdashboard;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::CB_close(FL_OBJECT *obj, long) { // static
  objform(obj)->close_gui();
}

void ImageFormGfx::close_gui() {
  // Close the form before logging the "close" command, so that
  // dashboards that log changes in their parameters log them before
  // the "close" command is logged. But the menucommand() function
  // won't get the proper index for the graphics window after the
  // window is closed, so it's necessary to call menucommand() before
  // closing the window.
  CharString logstring = menucommand() + "close";
  close();
  garcon()->log(logstring);
}

CommandFn ImageForm::close_cmd() { // static
  get_imageform()->close();
}

void ImageFormGfx::close() {
  fl_freeze_form(the_form);
  currentdashboard->hide_dashboard(); // give dashboard a chance to clean up
  hide();
  delete this;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::save_image(const FileName &filename) const {
  FILE *file = fopen(filename, "w");
  if(file) {
    if(filename.tail(3) == ".ps" || filename.tail(4) == ".eps")
      draw_ps(file);
    else
      canvas->ppmfile(file);
    fclose(file);
  }
}

void ImageFormGfx::CB_save(FL_OBJECT *obj, long) {
  objform(obj)->save_gui();
}

void ImageFormGfx::save_gui() const {
  static FileName lastname;
  FileName filename;
  if(lastname)
    filename = FileName(fl_show_fselector("Enter PPM or PS file name", ".",
					  "*.p[bgp]m", lastname.charstar()));
  else
    filename = FileName(fl_show_fselector("Enter PPM or PS file name", ".",
					  "*.p[bgp]m", ""));
  if(filename) {
    lastname = filename;
    save_image(filename);
    garcon()->log(menucommand() + "save filename=" + filename);
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::CB_home(FL_OBJECT *obj, long) {
  objform(obj)->home();
}

void ImageFormGfx::home() {
  formwaiter().show_homeform();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageFormGfx::set_undoredo_buttons(int undoable, int redoable) {
  fl_freeze_form(fdui->image_drawer);
  if(undoable) {
    fl_set_object_lcol(fdui->button_undo_selection, FL_BLACK);
    fl_activate_object(fdui->button_undo_selection);
  }
  else {
    fl_set_object_lcol(fdui->button_undo_selection, FL_INACTIVE_COL);
    fl_deactivate_object(fdui->button_undo_selection);
  }
  if(redoable) {
    fl_set_object_lcol(fdui->button_redo_selection, FL_BLACK);
    fl_activate_object(fdui->button_redo_selection);
  }
  else {
    fl_set_object_lcol(fdui->button_redo_selection, FL_INACTIVE_COL);
    fl_deactivate_object(fdui->button_redo_selection);
  }
  fl_unfreeze_form(fdui->image_drawer);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void ImageForm::update_gallery_listing(const Vec<NamedImage*> &gallery) {
  if(imagemanager)
    imagemanager->update_gallery_listing(gallery);
}

const Image &ImageForm::current_picture() const {
  return *imagemodification->imagestack.current();
}

Image &ImageForm::current_picture() {
  return *imagemodification->imagestack.current();
}

void ImageForm::store_current_picture(const CharString &name) const {
  goof->addimage(current_picture(), name);
  imagemodification->saved_image();
}

void ImageFormGfx::set_picture(const Image &image) {
  if(imagemodification->imagestack.size() > 1 && // img has been modified
     !imagemodification->imagestack.atbottom() && // and not undone
     imagemodification->modified() && // and not saved
     !garcon()->question("OK to overwrite modifide image in Window "
			 + imageform_id + "?", 0))
    return;
  imagemodification->imagestack.clear();
  imagemodification->imagestack.push(image);
  imagemodification->set_mod_undoredobuttons();
  draw();
}

void ImageFormTxt::set_picture(const Image &image) {
  if(imagemodification->imagestack.atbottom()) {
    imagemodification->imagestack.clear();
    imagemodification->imagestack.push(image);
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// configuration file stuff

// format:
// imageform   (starts parsing from imageformconfigmenu)
// image       (repeat for each image on stack)
//   ppm data  (repeat for each image on stack)
// currentimage = n
// end_imageform (stops parsing from imageformconfigmenu)

void ImageForm::save_imageforms(ostream &file) { // static
  for(int i=0; i<all_imageforms.capacity(); i++)
    if(all_imageforms[i])
      all_imageforms[i]->save(file);
}

void ImageForm::save(ostream &file) const {
  file << "imageform" << endl;
  // the actual images
  for(int i=0; i<imagemodification->imagestack.size(); i++) {
    file << "image" << endl;
    file << *imagemodification->imagestack[i] << endl;
  }
  // index of current image
  file << "currentimage=" << imagemodification->imagestack.current_index()
       << endl;

  save_extras(file);
  menu->print(file, "");
  file << "end_imageform" << endl;
}

void ImageFormGfx::save_extras(ostream &file) const {
  file << "resize w=" << the_form->w << " h=" << the_form->h << endl;
  if(beenclicked)
    file << "clicked=1" << endl;
  file << "scrollbars" << endl;
  canvas->save_scroll(file);
}


// static data used when creating imageform from the config file
istream *ImageForm::configfile = 0;// the configuration file
ImageForm *ImageForm::incipient_form = 0; // form being created
bool ImageForm::done_loading = 0; // are we done yet?
int ImageForm::currentimage = 0; // image currently displayed
Menu *ImageForm::configmenu = 0;

CommandFn ImageForm::set_end_imageform() { // static
  done_loading = 1;
}

CommandFn ImageForm::load_image() { // static
  Image image;
  *configfile >> image;
  if(*configfile) {
    if(incipient_form == 0) {
      // having read the first image, we can now create the form
      if(textmode)
	incipient_form = new ImageFormTxt(current_goof, image);
      else
	incipient_form = new ImageFormGfx(current_goof, image);
      // add the form's variables and commands to the menu
      configmenu->AddSubMenu("dashboard", "simulated dashboards",
			     incipient_form->dashboardmenu);
      AddVariable(configmenu, "clicked", "has the form been clicked on?",
		  incipient_form->beenclicked);
      AddVariable(configmenu, "dashboardnumber", "set dashboard",
		  incipient_form->current_dashboard_no, setdashboard_cmd);

      // don't let any of the commands have any effect until the form
      // is completely read
      incipient_form->inhibit_drawing();
    }
    else
      incipient_form->imagemodification->imagestack.push(image);
  }
}

void ImageForm::set_currentimage(const CharString&) { // static
  if(incipient_form)
    incipient_form->imagemodification->imagestack.go_to(currentimage);
}

CommandFn ImageForm::read_scrollbars() { // static
  if(incipient_form)
    incipient_form->readscrollbars();
}

void ImageFormGfx::readscrollbars() { // virtual
  canvas->read_scroll(*configfile);
}

void ImageFormTxt::readscrollbars() {
  char discard[1000];
  configfile->getline(discard, sizeof(discard));
}

void ImageForm::read_imageform(istream &file) { // static

  // ImageFormGfxs are read from the config file by parsing a menu. The
  // form is created only *after* the first image is read by the
  // "image" command.  A pointer to the ImageFormGfx is stored in
  // incipient_form so that subsequent menu commands can use it.  The
  // command "end_imageform" sets the variable done_loading, which
  // indicates that the parsing should stop.
  done_loading = 0;
  incipient_form = 0;
  currentimage = 0;
  configfile = &file;

  // Create a new menu. Since each form that's read adds local
  // variables to the menu, the menu has to be recreated for each new
  // form.  The commands that can be done before the form is created
  // are added here; the ones that depend on the form's existence are
  // added in localconfigmenu().
  if(configmenu)
    delete configmenu;
  configmenu = new Menu("imageformconfig");
  configmenu->AddCommand("image", "load an image", load_image);
  configmenu->AddCommand(new ResizeCmd);
  configmenu->AddCommand("scrollbars", "set scrollbars", read_scrollbars);
  configmenu->AddCommand("end_imageform", "stop reading imageform data",
		   set_end_imageform);
  AddVariable(configmenu, "currentimage", "set current image", currentimage,
	      set_currentimage);
  // run until end_imageform() returns 1, which is after the
  // "end_imageform" command has been executed.
  garcon()->runfile(file, configmenu, 0, end_imageform);

  if(incipient_form) {
    incipient_form->allow_drawing();
    incipient_form->imagemodification->set_mod_undoredobuttons();
    incipient_form->set_all_colors();
    incipient_form->draw();
  }
}
