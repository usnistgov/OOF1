// -*- C++ -*-
// $RCSfile: pixmap.h,v $
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


#ifndef PPM2OOFPIXMAP_H
#define PPM2OOFPIXMAP_H

#include <X11/Xlib.h>
#include "forms.h"

class PixMap {
private:
  int width_, height_;		// size in pixels
  Pixmap pixmap_;
  GC gc_;
public:
  PixMap(int w, int h);
  PixMap() {}
  ~PixMap();

  Pixmap pixmap() const { return pixmap_;}
  GC gc() const { return gc_; }
  int height() const { return height_; }
  int width() const { return width_; }

  void resize(int, int);
  void clear() const;
  void clear(GC) const;
};


#endif
