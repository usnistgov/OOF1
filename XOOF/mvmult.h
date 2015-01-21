// -*- C++ -*-
// $RCSfile: mvmult.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef MVMULT_H
#define MVMULT_H

#include "mvvd.h"
#include "mvmd.h"
class MeshCoord;

MV_Vector_double operator*(const MV_ColMat_double&, const MV_Vector_double&);
MV_Vector_double operator*(const MV_ColMat_double&, const MeshCoord&);

MV_Vector_double trans_mult(const MV_ColMat_double&, const MV_Vector_double&);

MV_ColMat_double operator+(const MV_ColMat_double&, const  MV_ColMat_double&);

MV_ColMat_double operator*(const MV_ColMat_double&, const MV_ColMat_double&);
    
MV_ColMat_double trans_mult(const MV_ColMat_double&, const MV_ColMat_double&);

#endif
