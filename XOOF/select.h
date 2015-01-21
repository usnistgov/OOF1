// -*- C++ -*-
// $RCSfile: select.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:14 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef SELECT_H
#define SELECT_H

#include "mvvd.h"
#include "menuDef.h"
#include "shape.h"

Menu *selectelemmenu();
Menu *mainselectmenu();
CommandFn unselectallelements();

#endif
