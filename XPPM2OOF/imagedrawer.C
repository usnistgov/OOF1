// -*- C++ -*-
// $RCSfile: imagedrawer.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:34 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// wrapper for image_drawer.c so that callback definitions are in scope

#include "autodash.h"
#include "colorutils.h"
#include "coordinates.h"
#include "displaydash.h"
#include "imageform.h"
#include "imagemandash.h"
#include "imagemoddash.h"
#include "infodash.h"
#include "quit.h"
#include "selectdash.h"

#include "image_drawer2.c"
