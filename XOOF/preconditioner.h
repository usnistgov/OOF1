// -*- C++ -*-
// $RCSfile: preconditioner.h,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2001-10-26 01:24:46 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


class Preconditioner;
class Diagonal_Precond;
class Block_Precond;
class BP_block;
class NullPreconditioner;

#ifndef PRECONDITIONER_H
#define PRECONDITIONER_H

#include "enum.h"
#include "grid.h"
#include "symmmatrix.h"
#include "vec.h"
#include "menuDef.h"
#include "mvvd.h"
#include "mvmd.h"
#include "diagpre_double.h"
#include "icpre_double.h"
#include "ilupre_double.h"

#include "beginheader.h"

enum PreconditionerType {
  PC_NONE,
#ifndef THERMAL
  PC_DIAG, PC_BLOCK,
#endif // !THERMAL
  PC_ILU, PC_ICP
};

EXTERN Enum<PreconditionerType> pctype DFLT(PC_ILU);
void preconditioner_init();

// #ifdef sun
#include "varobjects.h"
template <>
VarObject* 
Var<Enum<PreconditionerType> >::create_varobject(Variable *var, Form *form,
						 VarObjectType t,
						 int x, int y, int w, int h);
// #endif

class Preconditioner {
    public:
	virtual ~Preconditioner() {}
	virtual MV_Vector_double solve(const MV_Vector_double &) const = 0;
	virtual MV_Vector_double trans_solve(const MV_Vector_double &) const = 0;
};

#ifndef THERMAL

class Diagonal_Precond : public Preconditioner {
    private:
	MV_Vector_double diag;
    public:
	Diagonal_Precond(Grid *g);
	virtual MV_Vector_double solve(const MV_Vector_double &) const;
	virtual MV_Vector_double trans_solve(const MV_Vector_double &) const;
};

// block matrix for block preconditioner

class BP_block {
    friend class Block_Precond;
    private:
	Vec<int> dof;	// dof's in this block
	SymmMatrix m;
    public:  	
	BP_block(int i) : dof(1), m(1) { dof[0] = i; }
	BP_block(int i, int j) : dof(2), m(2) { dof[0] = i; dof[1] = j; }
	void add(double k) {	    // add to m for 1x1
	    m(0, 0) += k;
	}
	void add(double k00, double k01, double k11) { // add to m for 2x2
	    m(0, 0) += k00;
	    m(0, 1) += k01;
	    m(1, 1) += k11;
	}
};


class Block_Precond : public Preconditioner {
    private:
	Vec<BP_block*> block;	// array of blocks on the diagonal
    public:
	Block_Precond(Grid *g);
	~Block_Precond();
	virtual MV_Vector_double solve(const MV_Vector_double &) const;
	virtual MV_Vector_double trans_solve(const MV_Vector_double &) const;
};
#endif // !THERMAL

class NullPreconditioner : public Preconditioner {
    public:
	NullPreconditioner() {};
	virtual MV_Vector_double solve (const MV_Vector_double &x) const {
	    return x;
	}
	virtual MV_Vector_double trans_solve (const MV_Vector_double &x) const {
	    return x;
	}
};

// interface to SparseLib++ preconditioners

class SL_Diagonal_Precond : public Preconditioner, 
			      public DiagPreconditioner_double {
    public:
	SL_Diagonal_Precond(CompRow_Mat_double &A)
	    : DiagPreconditioner_double(A) {}
	virtual MV_Vector_double solve(const MV_Vector_double &x) const {
	    return DiagPreconditioner_double::solve(x);
	}
	virtual MV_Vector_double trans_solve(const MV_Vector_double &x) const {
	    return DiagPreconditioner_double::trans_solve(x);
	}
};

class SL_ILUPreconditioner : public Preconditioner, 
			      public CompRow_ILUPreconditioner_double {
    public:
	SL_ILUPreconditioner(CompRow_Mat_double &A)
	    : CompRow_ILUPreconditioner_double(A) {}
	virtual MV_Vector_double solve(const MV_Vector_double &x) const {
	    return CompRow_ILUPreconditioner_double::solve(x);
	}
	virtual MV_Vector_double trans_solve(const MV_Vector_double &x) const {
	    return CompRow_ILUPreconditioner_double::trans_solve(x);
	}
};

class SL_ICPreconditioner : public Preconditioner, 
			      public ICPreconditioner_double {
    public:
	SL_ICPreconditioner(CompRow_Mat_double &A)
	    : ICPreconditioner_double(A) {}
	virtual MV_Vector_double solve(const MV_Vector_double &x) const {
	    return ICPreconditioner_double::solve(x);
	}
	virtual MV_Vector_double trans_solve(const MV_Vector_double &x) const {
	    return ICPreconditioner_double::trans_solve(x);
	}
};

// ---------------------------------------------------------- //

Preconditioner* preconditioner(Grid*);
Preconditioner* preconditioner(CompRow_Mat_double &);

Menu *precondmenu();

#include "endheader.h"

#endif
