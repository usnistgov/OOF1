// -*- C++ -*-
// $RCSfile: selectpixel.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:03:12 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "goof.h"
#include "selectpixel.h"
#include "rubberband.h"
#include "adaptmesh.h"
#include "amtriangle.h"
#include "shape.h"
#include <iostream.h>

void select_pixel(const Cell_coordinate &where, int shiftkey) {
  if(current_goof->active(where)) {
    current_goof->init_selection(shiftkey);
    if(current_goof->is_selected(where))
      current_goof->unselect(where);
    else {
      current_goof->select(where);
    }
  }
}

void select_pixels(const MeshCoord &up, const MeshCoord &dn,
		   int shiftkey, const RubberBandType &rbt)
{
  if(rbt == RB_NONE) return;
  current_goof->init_selection(shiftkey);
  switch(rbt) {
  case RB_RECTANGLE:
    current_goof->select(Rectangle(dn, up), shiftkey);
    break;
  case RB_CIRCLE:
    current_goof->select(Circle(dn, up), shiftkey);
    break;
  case RB_ELLIPSE:
    current_goof->select(Ellipse(dn, up), shiftkey);
    break;
  }
}
