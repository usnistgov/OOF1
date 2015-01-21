// -*- C++ -*-
// $RCSfile: graphics.h,v $
// $Revision: 1.3 $
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


#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "cursor.h"
#include "menuDef.h"

#include "beginheader.h"

EXTERN TrueFalse use_wallpaper DFLT(1);

EXTERN XCursor *selectcursor DFLT(0);
EXTERN XCursor *zoomcursor DFLT(0);
EXTERN XCursor *infocursor DFLT(0);
EXTERN XCursor *crosshaircursor DFLT(0);
void init_cursors();
void free_cursors();

Menu *graphicsmenu();

#include "endheader.h"

#endif
