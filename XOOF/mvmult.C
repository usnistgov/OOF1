// -*- C++ -*-
// $RCSfile: mvmult.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:53 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// provide a matrix*vector multiplication operator for mv++

#include "fatalerror.h"
#include "mvmult.h"
#include "mvvd.h"
#include "mvmd.h"
#include "meshcoord.h"

// blas declarations
extern "C" {
    void dgemm_(char*, char*, int*, int*, int*, double*,
	    const double*, int*, const double*, int*, double*, double*, int*);
    void dgemv_(char*, int*, int*, double*, const double*, int*, const double*,
	int*, double*, double*, int*);
    void daxpy_(int*, double*, const double*, int*, double*, int*);
}

// A * x

MV_Vector_double operator*(const MV_ColMat_double &m,
				const MV_Vector_double &v) {
    int vsize = v.size();
    int mrows = m.size(0);
    int mcols = m.size(1);
    MV_Vector_double result(mrows, 0.0);
    char enn = 'n';
    double One = 1.0;
    int one = 1;
    double zero = 0.0;
    if(mcols != vsize) fatalerror(); // should be more sophisticated here!
    
    dgemv_(&enn, &mrows, &mcols, &One, &m(0, 0), &mrows, &v[0], &one, &zero, 
	&result[0], &one);
    return result;
}

MV_Vector_double operator*(const MV_ColMat_double &m, const MeshCoord &v) {
  if(m.size(1) != 2) fatalerror();	// should be more sophisticated here!
  int mrows = m.size(0);
  MV_Vector_double result(mrows);
  for(int i=0; i<mrows; i++)
    result[i] = m(i, 0)*v.x + m(i, 1)*v.y;
  return result;
}

// A^T * x

MV_Vector_double trans_mult(const MV_ColMat_double &m,
			    const MV_Vector_double &v) {
    int vsize = v.size();
    int mrows = m.size(0);
    int mcols = m.size(1);
    MV_Vector_double result(mcols, 0.0);
    char tee = 't';
    double One = 1.0;
    int one = 1;
    double zero = 0.0;
    if(mrows != vsize) fatalerror(); // should be more sophisticated here!
    
    dgemv_(&tee, &mrows, &mcols, &One, &m(0, 0), &mrows, &v[0], &one, &zero, 
	&result[0], &one);
    return result;
}

// A * B

MV_ColMat_double operator*(const MV_ColMat_double &a,
                           const MV_ColMat_double &b)
{
    int arows = a.size(0);
    int acols = a.size(1);
    int brows = b.size(0);
    int bcols = b.size(1);
    MV_ColMat_double product(arows, bcols);
    // call blas
    char enn = 'n';
    double one = 1.0;
    double zero = 0.0;
    dgemm_(&enn, &enn, &arows, &bcols, &acols, &one, &a(0, 0), &arows,
	    &b(0, 0), &brows, &zero, &product(0, 0), &arows);
    return product;
}


// A^T * B

MV_ColMat_double trans_mult(const MV_ColMat_double &a,
			    const MV_ColMat_double &b)
{
    int arows = a.size(0);
    int acols = a.size(1);
    int brows = b.size(0);
    int bcols = b.size(1);
    MV_ColMat_double product(acols, bcols);
    char enn = 'n';
    char tee = 't';
    double one = 1.0;
    double zero = 0.0;
    dgemm_(&tee, &enn, &acols, &bcols, &arows, &one, &a(0, 0), &arows, 
	    &b(0, 0), &brows, &zero, &product(0, 0), &acols);
    return product;
}

// A + B

MV_ColMat_double operator+(const MV_ColMat_double &a,
                           const MV_ColMat_double &b) {
    int size = a.size(0) * a.size(1);
    MV_ColMat_double sum(a);	// copy a
    // call blas...
    int one = 1;
    double One = 1.0;
    daxpy_(&size, &One, &b(0, 0), &one, &sum(0, 0), &one);
    return sum;
}
