// -*- C++ -*-
// $RCSfile: displacements.C,v $
// $Revision: 1.3 $
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


#include "freedom.h"
#include "vec.h"
#include "displacements.h"
#include "mvvd.h"

MV_Vector_double displacements(const Vec<DegreeOfFreedom*> &dof) {
    int n = dof.capacity();
    MV_Vector_double u(n);
    for(int i=0; i<n; i++)
	u[i] = dof[i]->disp();
    return u;
}

MV_Vector_double appliedforces(const Vec<DegreeOfFreedom*> &dof) {
  int n = dof.capacity();
  MV_Vector_double f(n);
  for(int i=0; i<n; i++)
    f[i] = dof[i]->force();
  return f;
}

void displacements(Vec<DegreeOfFreedom*> &dof, const MV_Vector_double &u) {
    int n = dof.capacity();
    for(int i=0; i<n; i++)
	dof[i]->disp() = u[i];
}

// void setforces(Vec<DegreeOfFreedom*> &dof, const MV_Vector_double &f) {
//     int n = dof.capacity();
//     for(int i=0; i<n; i++)
// 	dof[i]->force() = f[i];
// }
