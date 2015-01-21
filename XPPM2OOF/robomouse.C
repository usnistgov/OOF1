// -*- C++ -*-
// $RCSfile: robomouse.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:46 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "menuDef.h"
#include "mouseclick.h"
#include "robomouse.h"
#include "imageform.h"
#include "stdlib.h"

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// The functions get_imageform() and get_dashboard() find the correct
// ImageForm and Dashboard for commands executed by scripts, either
// log files or configuration files.  They extract the information
// they need from the command path.  The process is complicated
// because the menu structure changed at the introduction of
// configuration files, but we don't want to make old log files
// obsolete.

// The old menu structure looked like this:
// graphics
//    imageform_id (a number)
//       dashboardnumber = <n>
//       commands and variables
// The current value of dashboard number was used in get_dashboard().
// The bottom menu level is stored in Menu *ImageForm::menu.

// The new menu structure keeps the old entries, but adds a dashboard
// submenu to ImageForm::menu.  This menu looks like
//    dashboard
//       display
//          commands and variables specific to the display dashboard
//       coordinates
//          commands and variables specific to the coordinates dashboard
//       etc.

// When writing configuration files, the NEW version of the
// ImageForm::menu is used.  It's possible to tell which of the three
// possible modes is in use by examining the command path:

//  (1) Old log files.  The first word in the path is "graphics" and
//  the third word is not "dashboard".  Deduce the imageform from the
//  second word and the dashboard from the imageform's current
//  dashboard.

//  (2) New log files.  The first word in the path is "graphics" and
//  the third word is "dashboard".  Deduce the imageform from the
//  second word and the dashboard from the fourth.

//  (3) Configuration files.  Parsing is done from ImageForm::menu
//  directly, so the first word in the path is not "graphics".  Use
//  the incipient_form variable to get the imageform and the second
//  word of the path to deduce the dashboard.

ImageForm *get_imageform() {
  int depth = garcon()->get_depth();
  if(garcon()->get_command(depth-1) == "graphics") {
    // reading an old or new log file
    CharString form_id(garcon()->get_command(depth-2));
    return ImageForm::all_imageforms[atoi(form_id)];
  }
  // first word != "graphics", are reading a configuration file
  return ImageForm::incipient_form;
}

Dashboard *get_dashboard() {
  ImageForm *imageform = get_imageform();
  int depth = garcon()->get_depth();
  if(garcon()->get_command(depth-1) == "graphics") {
    // reading an old or new log file
    if(garcon()->get_command(depth-3) == "dashboard")
      // reading a new log file
      return imageform->get_dashboard(garcon()->get_command(depth-4));
    else
      // reading an old log file
      return imageform->current_dashboard();
  }
  // reading a configuration file
  return imageform->get_dashboard(garcon()->get_command(depth-2));
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

RoboMouseCmd::RoboMouseCmd()
  : CommandM("mouse", "simulate a mouse click")
{
  AddArgument(this, "click", click);
}

CommandFn RoboMouseCmd::func() {
  garcon()->inhibit_logging(); // prevent mouse() from re-logging this command
  get_imageform()->robomouse(click);
  garcon()->disinhibit_logging();
}

CommandM *RoboMouseCmd::clone() const {
  RoboMouseCmd *rmc = new RoboMouseCmd();
  rmc->click = click;
  return rmc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


ZoomButtonCmd::ZoomButtonCmd()
  : CommandM("zoombutton", "simulate the scrollcanvas zoom button"),
    whichbutton(1)
{
  AddArgument(this, "button", whichbutton);
}

void ZoomButtonCmd::func() {
  get_imageform()->zoombutton(whichbutton);
}

CommandM* ZoomButtonCmd::clone() const {
  ZoomButtonCmd *zc = new ZoomButtonCmd;
  zc->whichbutton = whichbutton;
  return zc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

ScrollCommand::ScrollCommand()
  : CommandM("scroll", "simulate scroll bars"),
    dx(0),
    dy(0)
{
  AddArgument(this, "dx", dx);
  AddArgument(this, "dy", dy);
}

void ScrollCommand::func() {
  get_imageform()->xyscroll(dx, dy);
}

CommandM *ScrollCommand::clone() const {
  ScrollCommand *sc = new ScrollCommand;
  sc->dx = dx;
  sc->dy = dy;
  return sc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

ResizeCmd::ResizeCmd()
  : CommandM("resize", "change the size of the Drawer")
{
  AddArgument(this, "w", w);
  AddArgument(this, "h", h);
}

void ResizeCmd::func() {
  // resize_callback() will attempt to log this command, so prevent
  // it. The command has already been logged by the parser.
  garcon()->inhibit_logging();
  get_imageform()->resize(w, h);
  garcon()->disinhibit_logging();
}

CommandM* ResizeCmd::clone() const {
  ResizeCmd *rc = new ResizeCmd;
  rc->w = w;
  rc->h = h;
  return rc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

SaveCmd::SaveCmd()
  : CommandM("save", "save the picture")
{
  AddArgument(this, "filename", filename);
}

void SaveCmd::func() {
  get_imageform()->save_image(filename);
}

CommandM *SaveCmd::clone() const {
  SaveCmd *sc = new SaveCmd;
  sc->filename = filename;
  return sc;
}
