// -*- C++ -*-
// $RCSfile: gfx.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-08-27 20:56:08 $

#include "gfx.h"
#include "goof.h"
#include "imageform.h"
#include "ppm2oof.h"

static CommandFn openwindow() {
  open_window();
}

void open_window() {
  if(!goofinit()) return;
  // Create a new ImageForm with a copy of the most recently stored
  // image from the image gallery.
  if(textmode)
    (void) new
      ImageFormTxt(current_goof,
		   *current_goof->get_image(current_goof->gallerysize()-1));
  else
    (void) new
      ImageFormGfx(current_goof,
		   *current_goof->get_image(current_goof->gallerysize()-1));
}

static CommandFn closewindows() {
  if(!goofinit()) return;
  current_goof->close_all_forms();
}

static void rdrw(const CharString&) {
  if(current_goof)
    current_goof->redraw();
}

Menu *graphicsmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("graphics");
  menu->AddCommand("open", "open a graphics window", openwindow);
  menu->AddCommand("close", "close all graphics windows", closewindows);
  AddVariable(menu, "opacity", "opacity of selected pixels",
	      selected_opacity, rdrw);
  return menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

#include "zoomcursor.h"
#include "zoommask.h"
#include "infocursor.h"
#include "infomask.h"
#include "checkcursor.h"
#include "checkmask.h"

void init_cursors() {
  selectcursor = new XCursor(XC_hand1);
  zoomcursor = new XCursor(zoomcursor_bits, zoommask_bits,
                        zoomcursor_width, zoomcursor_height,
                        zoomcursor_x_hot, zoomcursor_y_hot);
  infocursor = new XCursor(infocursor_bits, infomask_bits,
                           infocursor_width, infocursor_height,
                           infocursor_x_hot, infocursor_y_hot);
  checkcursor = new XCursor(checkcursor_bits, checkmask_bits,
                           checkcursor_width, checkcursor_height,
                           checkcursor_x_hot, checkcursor_y_hot);
}

void free_cursors() {
  if(selectcursor) {
    delete selectcursor;
    delete zoomcursor;
    delete infocursor;
    delete checkcursor;
  }
}
