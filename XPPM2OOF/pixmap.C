// -*- C++ -*-
// $RCSfile: pixmap.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:44 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// pixmap code for ppm2oof

#include "pixmap.h"
#include "colorutils.h"

PixMap::PixMap(int w, int h)
  : width_(w), height_(h)
{
  pixmap_ = XCreatePixmap(gfxinfo.display(), gfxinfo.window(), width_, height_,
			  gfxinfo.depth());
  gc_ = XCreateGC(gfxinfo.display(), gfxinfo.window(), 0, 0);
  clear();
}

PixMap::~PixMap() {
  XFreePixmap(gfxinfo.display(), pixmap_);
  XFreeGC(gfxinfo.display(), gc_);
}

void PixMap::clear() const {
  XSetForeground(gfxinfo.display(), gc_,
		 WhitePixel(gfxinfo.display(), gfxinfo.screen()));
  clear(gc_);
}

void PixMap::clear(GC gc) const {
  XFillRectangle(gfxinfo.display(), pixmap_, gc, 0, 0, width_, height_);
}

void PixMap::resize(int w, int h) {
  //  cerr << "PixMap::resize  w=" << w << " h=" << h << endl;
  XFreePixmap(gfxinfo.display(), pixmap_);
  width_ = w;
  height_ = h;
  pixmap_ = XCreatePixmap(gfxinfo.display(), gfxinfo.window(), width_, height_,
			  gfxinfo.depth());
  clear();
}
