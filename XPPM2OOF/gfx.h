// -*- C++ -*-
// $RCSfile: gfx.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:32 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef GFX_H
#define GFX_H

#include "cursor.h"
class Menu;

#include "beginheader.h"

Menu *graphicsmenu();
void open_window();
void init_cursors();

EXTERN double selected_opacity DFLT(0.3); // opacity of selected pixels

EXTERN XCursor *selectcursor DFLT(0);
EXTERN XCursor *zoomcursor DFLT(0);
EXTERN XCursor *infocursor DFLT(0);
EXTERN XCursor *checkcursor DFLT(0);
void init_cursors();
void free_cursors();

#include "endheader.h"

#endif
