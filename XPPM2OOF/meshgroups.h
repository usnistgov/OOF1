// -*- C++ -*-
// $RCSfile: meshgroups.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-04-24 21:04:37 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef MESHGROUPS_H
#define MESHGROUPS_H

#include "groups.h"

class AMTriangle;

typedef Group<AMTriangle*> MeshGroup;

// declare template specialization
template <> void Group<AMTriangle*>::append(AMTriangle* const&);
template <> void Group<AMTriangle*>::append(const Vec<AMTriangle*>&);
template <> void Group<AMTriangle*>::clear();
template <> void Group<AMTriangle*>::remove(AMTriangle*const&);

#endif
