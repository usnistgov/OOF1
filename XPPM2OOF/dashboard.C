// -*- C++ -*-
// $RCSfile: dashboard.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-09-06 20:11:28 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "dashboard.h"
#include "imageform.h"
#include "cursor.h"
#include "goof.h"

Dashboard::Dashboard(const CharString &nm, ImageForm *imf, FL_OBJECT *grp,
		     RubberBandType rbt)
  : name_(nm),
    form(imf),
    objectgroup(grp),
    rubberbandtype(rbt),
    active_(0)
{
  menu = imageform()->dashboardmenu->AddSubMenu(name(), name());
}


CharString Dashboard::menucommand() const {
  return imageform()->menucommand() + " dashboard " + name() + " ";
}

// These routines are defined just so that the Dashboard subclasses
// don't all have to be friends of the ImageForm class.

ImageForm *Dashboard::imageform() const {
  return form;
  // this isn't inlined because then dashboard.h would have to include
  // imageform.h. Since time won't be critical, there's no need to
  // inline.
}

Menu *Dashboard::imageformmenu() const {
 return imageform()->menu; 
}

Goof *Dashboard::goof() const {
  ImageForm *imf = imageform();
  if(!imf) return 0;
  return imf->goof;
}

FD_image_drawer *Dashboard::fdui() const {
  ImageFormGfx *imf = dynamic_cast<ImageFormGfx*>(imageform());
  if(!imf) return 0;
  return (FD_image_drawer*) imf->fdui;
}

//  Menu *Dashboard::menu() const {
//    ImageForm *imf = imageform();
//    if(!imf) return 0;
//    return imf->menu;
//  }

ImageCanvas *Dashboard::canvas() const {
  return dynamic_cast<ImageFormGfx*>(imageform())->canvas;
}

void Dashboard::setcursor(const XCursor &cursor) {
  imageform()->setcursor(cursor);
}

void Dashboard::unsetcursor() {
  imageform()->unsetcursor();
}

int Dashboard::hide_dashboard() {
  int status = hide();  // virtual
  deactivate();
  fl_hide_object(objectgroup);
  return status;
}

int Dashboard::show_dashboard() {
  fl_show_object(objectgroup);
  int status = show(); // virtual
  activate();
  return status;
}
