// -*- C++ -*-
// $RCSfile: displacements.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:10:53 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef DISPLACEMENTS_H
#define DISPLACEMENTS_H

#include "vec.h"
#include "mvvd.h"

class DegreeOfFreedom;

MV_Vector_double displacements(const Vec<DegreeOfFreedom*> &);

MV_Vector_double appliedforces(const Vec<DegreeOfFreedom*> &);

void displacements(Vec<DegreeOfFreedom*> &, const MV_Vector_double &);

// void setforces(Vec<DegreeOfFreedom*> &, const MV_Vector_double &);

#endif
