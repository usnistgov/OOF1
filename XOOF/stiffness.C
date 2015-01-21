// -*- C++ -*-
// $RCSfile: stiffness.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:57 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/*
 * stiffness matrix class for IML++
 * this class only does the multiplication by the displacement vectors,
 * and doesn't store the global matrix. Therefore no preconditioner
 * can be used, but errors in global matrix construction won't matter.
 */
 
#include "stiffness.h"

// declaring as a member function doesn't work: 
//MV_Vector_double Stiffness::operator*(const MV_Vector_double& u) {

MV_Vector_double operator*(const Stiffness &s, const MV_Vector_double& u) {
    Grid *g = s.g;
    int nresult;
    int i;
    if(s.fixedrows)
	nresult = g->elastic_dof.nfixed();
    else
	nresult = g->elastic_dof.nfree();
    MV_Vector_double result(nresult, 0.0);
    
    // interior elements only contribute to Kii * ui
    if(!s.fixedrows && !s.fixedcols)
	for(i=0; i<g->interiorE.capacity(); i++)
	    g->interiorE[i]->Ku_mult(u, result);

    // boundary elements can contribute to all K's
    for(i=0; i<g->boundaryE.capacity(); i++)
	g->boundaryE[i]->Ku_mult(u, result, s.fixedrows, s.fixedcols);
    
    return result;
}
