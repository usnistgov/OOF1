// -*- C++ -*-
// $RCSfile: createelement.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-21 22:16:51 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef CREATEELEMENT_H
#define CREATEELEMENT_H

#include <stdio.h>
#include "symmmatrix.h"
#include "vec.h"

template <class ELEMENT>
ELEMENT *create_element(const ELEMENT*);

// rest are obsolete...

// create an element, given the node indices
template <class ELEMENT>
ELEMENT *create_element(const ELEMENT*, const int, const Vec<int>&);

#include "createelement.C"

#endif
