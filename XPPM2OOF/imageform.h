// -*- C++ -*-
// $RCSfile: imageform.h,v $
// $Revision: 1.8 $
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

#ifndef IMAGEFORM_H
#define IMAGEFORM_H

class ImageForm;
class ImageFormGfx;
class ImageFormTxt;

#include "color.h"
#include "forms.h"
#include "image.h"
#include "image_drawer2.h"
#include "menuform.h"
#include "mouseclick.h"
#include "robomouse.h"

class AutomationDashboard;
class AdaptiveMesh;
class AMTriangle;
class CoordDashboard;
class Dashboard;
class DisplayDashboard;
class FileName;
class Goof;
class Image;
class ImageCanvas;
class ImageManDashboard;
class ImageModDashboard;
class PixelInfoDashboard;
class MeshInfoDashboard;
class NamedImage;
class SelectDashboard;
class XCursor;

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// The base class ImageForm doesn't actually do any graphics. It has
// empty virtual functions for all of the graphics operations.  It
// does store Images and has all of the dashboards and menus, so that
// the rest of the program doesn't have to know if it's dealing with a
// functioning ImageForm (ImageFormGfx) or a fake one (ImageFormTxt).

class ImageForm {
protected:
  Goof *goof;

  void inhibit_drawing() { inhibit_ = 1; }
  void allow_drawing() { inhibit_ = 0; }
  bool inhibit_;


  // list of all imageforms ever created
  static Vec<ImageForm*> all_imageforms;
  static int imageformcount;
  CharString imageform_id;  // unique identifier for this form

  virtual void set_all_colors_() {}

public:
  ImageForm(Goof*, const Image&);
  virtual ~ImageForm();

  Dashboard *current_dashboard() const;
  Dashboard *get_dashboard(const CharString &) const;
  virtual void change_goof(Goof*) = 0;
  void update_gallery_listing(const Vec<NamedImage*> &gallery);

  Menu *menu;			// menu for simulating mouse clicks
  Menu *dashboardmenu;		// submenu of the main menu
  RoboMouseCmd robomousecmd;
  ZoomButtonCmd zoombuttoncmd;
  ScrollCommand scrollcmd;
  ResizeCmd resizecmd;
  SaveCmd savecmd;
  virtual void robomouse(const MouseClick&) = 0; // simulate a click
  CharString menucommand() const;

  virtual void mesh_created() const {}
  virtual void mesh_destroyed() const {}
  virtual void triangle_destroyed(const AMTriangle*) const {}
  virtual void set_picture(const Image&) = 0;
  const Image &current_picture() const;
  void store_current_picture(const CharString&) const;
  Image &current_picture();

  // Some things have to be in the base class even though they're not
  // used except in the Gfx derived class, so that all kinds of
  // ImageForms appear to work, as far as the outside world is
  // concerned.
  bool beenclicked;		// has either info dashboard been clicked on?
  virtual void draw() {}
  virtual void draw_ps(FILE*) const {}
  virtual void center_image() {}
  virtual void set_undoredo_buttons(int, int) {}
  virtual void getpixel(Color&, unsigned long) {} // assign pixel to color
  virtual void setcursor(const XCursor&) {}
  virtual void unsetcursor() {}
  virtual FL_FORM *xform() { return 0; }

  // We need to be able to take the address of this function (to use
  // it in Goof::apply_all_forms()), so I don't think it can be
  // virtual. But it can call a virtual function.
  void set_all_colors() { set_all_colors_(); }

  int current_dashboard_no;
  Dashboard *currentdashboard;
  // We need both current_dashboard_no and current_dashboard because
  // when changing dashboards, we need to be able to clean up
  // current_dashboard before switching to the new one. The new
  // dashboard number is already in current_dashboard_no.
  static int dflt_dashboard_no;
  Vec<Dashboard*> dashboard; // list of all dashboards
  virtual void adddashboard(Dashboard*) = 0; // install dashboard
  DisplayDashboard *displaydash;
  CoordDashboard *coorddash;
  SelectDashboard *selectdash;
  PixelInfoDashboard *pixelinfodash; 
  MeshInfoDashboard *meshinfodash;
  ImageManDashboard *imagemanager;
  ImageModDashboard *imagemodification;
  AutomationDashboard *automation;


  // ImageForm routines called by robomouse have to be virtual and
  // (mostly) do nothing by default
  virtual void zoombutton(int) {}
  virtual void xyscroll(int, int) {}
  virtual void resize(int, int) {}
  virtual void save_image(const FileName&) const {}
  virtual void close() {}
  virtual void set_dashboard() = 0;

  // Routines put into the menus by ImageForm::ImageForm have to be
  // static and in the base class.  They call the virtual functions.
  static CommandFn close_cmd();
  static void setdashboard_cmd(const CharString &); // called by menus

  // Stuff for writing config files
  static void save_imageforms(ostream&);
  void save(ostream&) const;
  virtual void save_extras(ostream&) const {} // called by save(ostream&)

  // Stuff for reading from config files
  static void read_imageform(istream&);
  static ImageForm *incipient_form; // form being loaded, used by read_imageform
  static Menu *configmenu;
  static bool done_loading;	// indicates end of parsing menu
  static bool end_imageform() { return done_loading; }
  static CommandFn set_end_imageform();
  static CommandFn load_image();
  static istream *configfile;
  static int currentimage;
  static void set_currentimage(const CharString&);
  static CommandFn read_scrollbars();
  virtual void readscrollbars() = 0;

  friend class Goof;
  friend class Dashboard;
  friend class PixelInfoDashboard;
  friend class MeshInfoDashboard;
  friend class DisplayDashboard;
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class ImageFormGfx : public Form, public ImageForm {

public:
  ImageFormGfx(Goof*, const Image&);
  virtual ~ImageFormGfx();

  virtual FL_FORM *xform() { return the_form; }
  virtual void draw();		// creates image from goof
  virtual void draw_ps(FILE*) const; // creates postscript file from goof
  virtual void change_goof(Goof*);

  virtual void adddashboard(Dashboard*); // install dashboard

  virtual void mesh_created() const;
  virtual void mesh_destroyed() const;
  virtual void triangle_destroyed(const AMTriangle*) const;

  Colormap colormap()  const { return xcolormap; }
  int reserved_colors() const;

  // record mouse motion events in log file?  [Never used...]
  //  void logmotion(const TrueFalse &flag) { logmotions = flag; }

  virtual void robomouse(const MouseClick&); // simulate a click

  virtual void center_image();
  //  double scalefactor() const;

  // activate & deactivate undo and redo buttons for pixel selections
  // and image modifications
  virtual void set_undoredo_buttons(int, int);

  virtual void set_picture(const Image&);

  static ImageFormGfx *objform(FL_OBJECT *obj);
  static Dashboard *objdash(FL_OBJECT *obj);

private:

  FD_image_drawer *fdui;

  ImageCanvas *canvas;
  Image localimage;

  // stuff for saving and loading configuration files
  virtual void save_extras(ostream&) const;
  virtual void readscrollbars();
  static MouseClick savedclick;
  static void setclick_cmd(const CharString&);

  // Most colors are stored in the dashboards (mainly
  // displaydashboard) where they are set.  The colors which are
  // stored here are the ones that aren't ever changed.

  Color rubberband_color;
  Color white;
  Color black;
  Color wallpaper_fg;
  Color wallpaper_bg;

  // When a dashboard changes a color, it has to call getpixel to
  // assign a pixel value (for TrueColor) or to put the color in the
  // colormap (for PseudoColor).  The second argument is the pixel
  // value defined below for each color.
  virtual void getpixel(Color&, unsigned long); // assign pixel to color

  static Color rubberband_color_dflt;

  static int nreserved_colors;
  virtual void set_all_colors_();

  // These are the "pixel" values of the colors -- ie indices into the
  // X colormap.  They are defined in imageform.[Ch] just to localize
  // the definitions so that their values are unique.
  static unsigned long const BLACKPIXEL;
  static unsigned long const WHITEPIXEL;
  static unsigned long const SELECTEDPIXEL;
  static unsigned long const HIGHLIGHTPIXEL;
  static unsigned long const CLICKEDPIXEL;
  static unsigned long const FIREWALLPIXEL;
  static unsigned long const MESHPIXEL;
  static unsigned long const SELMESHPIXEL;
  static unsigned long const SELMESHFADEPIXEL;
  static unsigned long const NOMATERIALPIXEL;
  static unsigned long const RUBBERBANDPIXEL;
  static unsigned long const WALLPAPER_FG;
  static unsigned long const WALLPAPER_BG;

  //  TrueFalse logmotions;

  virtual void setcursor(const XCursor&);
  virtual void unsetcursor();

  Colormap xcolormap;

  static int wm_close(FL_FORM*, void*); // handle window manager event

  static void CB_zoombutton(int, void*); // called by scrollcanvas
  void zoombutton_callback(int);         // called by CB_zoombutton
  virtual void zoombutton(int);                  // called by ZoomButtonCmd
  friend class ZoomButtonCmd;

  static void CB_xscroll(int, void*); // called by scrollcanvas
  void xscroll_callback(int);
  static void CB_yscroll(int, void*); // called by scrollcanvas
  void yscroll_callback(int);
  virtual void xyscroll(int, int); // called by ScrollCommand
  friend class ScrollCommand;

  static void CB_scrollcallback(const MeshCoord&, const MeshCoord&, void*);
  void scrollcallback(const MeshCoord&, const MeshCoord&);

  static void CB_resize(int, int, void*);
  void resize_callback();
  virtual void resize(int, int);
  friend class ResizeCmd;

  // routines to switch dashboards
  static void CB_dashboard(FL_OBJECT*, long);  // called by forms
  void set_dashboard_gui(); // called by CB_dashboard
  virtual void set_dashboard(); // set dashboard to current pulldown menu choice
  void set_dashboard(int); // called by CB_dashboard & menu, does real work

  static void CB_close(FL_OBJECT*, long);
  void close_gui();
  virtual void close();

  virtual void save_image(const FileName&) const;
  static void CB_save(FL_OBJECT*, long);
  void save_gui() const;

  static void CB_home(FL_OBJECT*, long); /* bring up home menu */
  void home();

  friend class ImageCanvas;
  friend class RoboMouseCmd;
  friend class SaveCmd;
  friend class PixelInfoDashboard;
  friend class MeshInfoDashboard;
  friend class DisplayDashboard;
  friend ImageForm *get_imageform();
  friend FD_image_drawer *create_form_image_drawer();

  friend class Dashboard;
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class ImageFormTxt : public ImageForm {
public:
  ImageFormTxt(Goof*, const Image&);
  virtual ~ImageFormTxt();

  virtual void change_goof(Goof*);
  virtual void set_picture(const Image&);
  virtual void adddashboard(Dashboard*); // install dashboard
  virtual void robomouse(const MouseClick&); // simulate a click
  virtual void set_dashboard();
  virtual void readscrollbars();
};

#endif

