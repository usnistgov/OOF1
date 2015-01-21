// -*- C++ -*-
// $RCSfile: graphics.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:10:59 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// output routines

#include <iostream.h>
#include "graphics.h"
#include "menuDef.h"
#include "formdrawer.h"
#include "grid.h"
#include "parameters.h"

static CommandFn opendrawer() {
  /* The FormDrawer constructor stores a pointer in the global list of
   * FormDrawers, so we don't need to save the pointer here. 
   */
  new FormDrawer(grid);
}

static CommandFn closedrawers() {
  FormDrawer::closeall();
}

static void repaper(const CharString &) {
  FormDrawerIterator iter;
  FormDrawer *fd;
  while(fd = iter()) {
    fd->wallpaper(use_wallpaper);
    fd->draw();
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#include "zoomcursor.h"
#include "zoommask.h"
#include "infocursor.h"
#include "infomask.h"

void init_cursors() {
  selectcursor = new XCursor(XC_hand1);
  zoomcursor = new XCursor(zoomcursor_bits, zoommask_bits,
			zoomcursor_width, zoomcursor_height,
			zoomcursor_x_hot, zoomcursor_y_hot);
  infocursor = new XCursor(infocursor_bits, infomask_bits,
			   infocursor_width, infocursor_height,
			   infocursor_x_hot, infocursor_y_hot);
  crosshaircursor = new XCursor(XC_crosshair);
}

void free_cursors() {
  if(selectcursor) {
    delete selectcursor;
    delete zoomcursor;
    delete infocursor;
    delete crosshaircursor;
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

Menu *graphicsmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("graphics");
  menu->AddCommand("open", "open a drawer", opendrawer);
  menu->AddCommand("close_all", "close all drawers", closedrawers);
  AddVariable(menu, "wallpaper", "use patterned background?", use_wallpaper,
	      repaper);
  AddVariable(menu, "enhancement", "display enhanced positions",
	      Grid::enhancement_dflt, Grid::set_enhancement_cmd);
  return menu;
}
