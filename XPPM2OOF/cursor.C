// -*- C++ -*-
// $RCSfile: cursor.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:27 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "colorutils.h"
#include "cursor.h"
#include "pixmap.h"

XCursor::XCursor(unsigned char *shapebits, unsigned char *maskbits,
	 int w, int h, int xhot, int yhot)
{
    Pixmap shape =
    XCreatePixmapFromBitmapData(gfxinfo.display(),
				RootWindow(gfxinfo.display(), gfxinfo.screen()),
				(char*) shapebits, w, h, 1, 0, 1);
  Pixmap mask =
    XCreatePixmapFromBitmapData(gfxinfo.display(),
				RootWindow(gfxinfo.display(), gfxinfo.screen()),
				(char*) maskbits, w, h, 1, 0, 1);
  XColor fg, bg;
  fg.red = fg.green = fg.blue = 0;
  bg.red = bg.green = bg.blue = 65535;
  XAllocColor(gfxinfo.display(),
	      DefaultColormap(gfxinfo.display(),
			      DefaultScreen(gfxinfo.display())),
	      &fg);
  XAllocColor(gfxinfo.display(),
	      DefaultColormap(gfxinfo.display(),
			      DefaultScreen(gfxinfo.display())),
	      &bg);

  cursor =  XCreatePixmapCursor(gfxinfo.display(), shape, mask,
				  &fg, &bg, xhot, yhot);
  XFreePixmap(gfxinfo.display(), shape);
  XFreePixmap(gfxinfo.display(), mask);
}

XCursor::XCursor(unsigned int which) {
  cursor = XCreateFontCursor(gfxinfo.display(), which);
}

XCursor::~XCursor() {
  XFreeCursor(gfxinfo.display(), cursor);
}
