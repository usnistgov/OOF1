// -*- C++ -*-
// $RCSfile: tocharstring.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-24 14:35:17 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef TOCHARSTRING_H
#define TOCHARSTRING_H

#include "charstring.h"

template <class TYPE> 
CharString to_charstring(const TYPE &);

#include "tocharstring.C"

#endif
