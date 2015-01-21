// -*- C++ -*-
// $RCSfile: cursor.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:51 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef CURSOR_H
#define CURSOR_H

#include <X11/Xlib.h>

class XCursor {
private:
  Cursor cursor;
public:
  XCursor(unsigned char *shape, unsigned char *mask,  // bitmap cursor
	 int w, int h, int xhot, int yhot);
  XCursor(unsigned int);                              // X font cursor
  ~XCursor();
  operator Cursor() const { return cursor; }
};

#endif
