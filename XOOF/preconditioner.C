// -*- C++ -*-
// $RCSfile: preconditioner.C,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2005-02-17 21:34:53 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



// preconditioners without global stiffness matrix

#include "fatalerror.h"
#include "preconditioner.h"
#include "grid.h"
#include "mvvd.h"
#include "parameters.h"
#include "menuDef.h"
#include "node.h"
#include <iostream.h>

// ------------------------------------------------------------- //

#ifndef THERMAL

/* Simple diagonal preconditioner
 * Approximates K by its diagonal elements only
 * ie the xx and yy terms of the self interactions
 */
 
Diagonal_Precond::Diagonal_Precond(Grid *g)
    : diag(g->elastic_dof.nfree(), 0.0)
{
    if(verbose) cerr << "Constructing simple diagonal preconditioner" << endl;
    
    // find diagonal elements
    int i;
    for(i=0; i<g->interiorE.capacity(); i++)
	g->interiorE[i]->interiordiagonals(diag);
    for(i=0; i<g->boundaryE.capacity(); i++)
	g->boundaryE[i]->interiordiagonals(diag);

    // invert diagonals
    for(i=0; i<diag.size(); i++) {
	if(diag[i] != 0.0)
	    diag[i] = 1./diag[i];
	else {
	    cerr << "Zero diagonal element at position " << i << "!" << endl;
	    fatalerror();
	}
    }
}

MV_Vector_double Diagonal_Precond::solve(const MV_Vector_double &x) const {
    MV_Vector_double a(x.size());
    if(x.size() != diag.size()) {
	cerr << "Incompatible sizes in Diagonal_Precond!" << endl;
    }
    for(int i=0; i<x.size(); i++) {
	a[i] = diag[i]*x[i];
    }
    return a;
}

MV_Vector_double Diagonal_Precond::trans_solve(const MV_Vector_double &x) const {
    MV_Vector_double a(x.size());
    for(int i=0; i<x.size(); i++) {
	a[i] = diag[i]*x[i];
    }
    return a;
}

// ------------------------------------------------------------ //

/* Slightly more sophisticated diagonal preconditioner
 * Approximates K by its 2x2 and 1x1 diagonal blocks 
 * ie the xx, xy, yx, and yy terms of the self interactions
 *  -- 2x2 if both dofs are free, 1x1 if only one is free, 0x0 if none are.
 */

Block_Precond::Block_Precond(Grid *g)
    : block(g->Nnodes())
    // nodeindex set to -1 so that unused nodes are flagged
{
    if(verbose) cerr << "Constructing block diagonal preconditioner" << endl;
    int nnodes = g->Nnodes();
    
    /* construct one block for each node */
    int i;
    for(i=0; i<nnodes; i++)
	    block[i] = g->node[i]->bpblock();

    /* load stiffness matrix for each block */
    for(i=0; i<g->interiorE.capacity(); i++)
	g->interiorE[i]->interiorblocks(block);
    for(i=0; i<g->boundaryE.capacity(); i++)
	g->boundaryE[i]->interiorblocks(block);
    
    // compute inverses of blocks
    double d, a00, a01, a11, det;
    for(i=0; i<nnodes; i++) {
	BP_block *thisblock = block[i];
	if(thisblock)
	    switch(thisblock->dof.capacity()) {
		case 0:
		    break;
		case 1: {
		    d = thisblock->m(0, 0);
		    if(d == 0) {
			cerr << "Zero diagonal at node " << i << "!" << endl;
			fatalerror();
		    }
		    thisblock->m(0, 0) = 1./d;
		    }
		    break;
		case 2:
		    a00 = thisblock->m(0, 0);
		    a01 = thisblock->m(0, 1);
		    a11 = thisblock->m(1, 1);
		    det = a00*a11 - a01*a01;
		    if(det == 0) {
			cerr << "Zero determinant in Block_Precond!" << endl;
			fatalerror();
		    }
		    det = 1./det;
		    thisblock->m(0, 0) = a11*det;
		    thisblock->m(1, 1) = a00*det;
		    thisblock->m(0, 1) *= -det;
		    break;
	    }
    }
}

Block_Precond::~Block_Precond() {
    for(int i=0; i<block.capacity(); i++) delete block[i];
}

MV_Vector_double Block_Precond::solve(const MV_Vector_double &x) const {
    int i, j, k;
    MV_Vector_double a(x.size(), 0.0);
    for(i=0; i<block.capacity(); i++) {
	BP_block *thisblock = block[i];
	if(thisblock)
	    switch(thisblock->dof.capacity()) {
		case 0:
		    break;
		case 1:
		    j = thisblock->dof[0];
		    a[j] = thisblock->m(0, 0) * x[j];
		    break;
		case 2:
		    j = thisblock->dof[0];
		    k = thisblock->dof[1];
		    a[j] = thisblock->m(0, 0)*x[j] + thisblock->m(0, 1)*x[k];
		    a[k] = thisblock->m(0, 1)*x[j] + thisblock->m(1, 1)*x[k];
	    }
     }
    return a;
}

MV_Vector_double Block_Precond::trans_solve(const MV_Vector_double &x) const {
    return solve(x);
}

#endif // !THERMAL

// ------------------------------------------------------------ //

// assign names to preconditioner types in the pull down menu
void preconditioner_init() {
  Enum<PreconditionerType>::name(PC_NONE, "none");
#ifndef THERMAL
  Enum<PreconditionerType>::name(PC_DIAG, "diagonal");
  Enum<PreconditionerType>::name(PC_BLOCK, "block");
#endif // !THERMAL
  Enum<PreconditionerType>::name(PC_ILU, "ILU");
  Enum<PreconditionerType>::name(PC_ICP, "ICP");
}

template <>
VarObject* 
Var<Enum<PreconditionerType> >::create_varobject(Variable *var, Form *form,
						 VarObjectType t,
						 int x, int y, int w, int h)
{
  return new EnumVarObject<PreconditionerType>(var, form, t, x, y, w, h);
}

Preconditioner* preconditioner(
#ifdef THERMAL
			       Grid*
#else  // !THERMAL
			       Grid *g
#endif // THERMAL
			       )
{   // for local matrix
  switch(pctype) {
  case PC_NONE:
    return new NullPreconditioner();
#ifndef THERMAL
  case PC_DIAG:
    return new Diagonal_Precond(g);
  case PC_BLOCK:
    return new Block_Precond(g);
#endif // !THERMAL
  default:
    garcon()->msout << ms_error << "Preconditioner not implemented!" << endl
		    << ms_normal;
    return new NullPreconditioner();
  }
}

Preconditioner* preconditioner(CompRow_Mat_double &Kii) {   // for global matrix
  switch(pctype) {
  case PC_NONE:
    return new NullPreconditioner();
#ifndef THERMAL
  case PC_DIAG:
    return new SL_Diagonal_Precond(Kii);
#endif // !THERMAL
  case PC_ICP:
    return new SL_ICPreconditioner(Kii);
  case PC_ILU:
    return new SL_ILUPreconditioner(Kii);
  default:
    garcon()->msout << ms_error << "Preconditioner not implemented!" << endl
		    << ms_normal;
    return new NullPreconditioner();
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#include "enumio.C"
