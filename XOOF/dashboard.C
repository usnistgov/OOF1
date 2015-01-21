// -*- C++ -*-
// $RCSfile: dashboard.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:52 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "dashboard.h"
#include "drawer.h"
#include "formdrawer.h"
#include "cursor.h"

CharString Dashboard::menucommand() const {
  return "graphics " + formdrawer()->formdrawer_id + " dashboard ";
}

// These routines are defined just so that the Dashboard subclasses
// don't all have to be friends of the FormDrawer class.

FormDrawer *Dashboard::formdrawer() const {
  return drawer->formdrawer;
}

OofCanvas *Dashboard::canvas() const {
  return drawer->formdrawer->canvas;
}

void Dashboard::setcursor(const XCursor &cursor) {
  drawer->formdrawer->setcursor(cursor);
}

void Dashboard::unsetcursor() {
  drawer->formdrawer->unsetcursor();
}

void Dashboard::hide_dashboard() {
  deactivate();
  hide();  // virtual
}

void Dashboard::show_dashboard() {
  show(); // virtual
  activate();
}
