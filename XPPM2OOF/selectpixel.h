// -*- C++ -*-
// $RCSfile: selectpixel.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:47 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef SELECTPIXEL_H
#define SELECTPIXEL_H

#include "rubberband.h"

class Cell_coordinate;
class MeshCoord;

void select_pixel(const Cell_coordinate&, int);
void select_pixels(const MeshCoord&, const MeshCoord&,
		   int, const RubberBandType&);

#endif
