// -*- C++ -*-
// $RCSfile: symmmatrix.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-31 19:15:14 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "fatalerror.h"
#include "mvmult.h"
#include "readbinary.h"
#include "mvmd.h"
#include "symmmatrix.h"
#include <iomanip.h>
#include <iostream.h>
#include "stdlib.h"
#include <string.h>

// construct an n by n matrix

SymmMatrix::SymmMatrix(int n) : nrows(n), m(0) {
    allocate();
}

// copy constructor

SymmMatrix::SymmMatrix(const SymmMatrix& sm) : nrows(sm.nrows), m(0) {
    allocate();
    copy(sm.m);
}
    
SymmMatrix::~SymmMatrix() {
    free();    
}

// assignment

const SymmMatrix& SymmMatrix::operator=(const SymmMatrix& sm) {
    if(&sm != this) {
	if(nrows != sm.nrows) {
	    free();
	    nrows = sm.nrows;
	    allocate();
	}
	copy(sm.m);
    }
    return *this;
}

// access

double SymmMatrix::operator()(int i, int j) const {
#ifdef DEBUG
    if(i >= nrows || j >= nrows) {
	cerr << "SymmMatrix error: i="<<i<<" j="<<j<<" nrows="<<nrows<<endl;
	abort();
    }
#endif
    if(i > j) return m[i][j];	// as stored
    return m[j][i];		// transpose
}

double &SymmMatrix::operator()(int i, int j) {
#ifdef DEBUG
    if(i >= nrows || j >= nrows) {
	cerr << "SymmMatrix error: i="<<i<<" j="<<j<<" nrows="<<nrows<<endl;
	abort();
    }
#endif
    if(i > j) return m[i][j];	// as stored
    return m[j][i];		// transpose
}

MV_ColMat_double operator*(const SymmMatrix &a, const SymmMatrix &b) {
  if(a.nrows != b.nrows) abort();
  int nrows = a.nrows;
  MV_ColMat_double result(nrows, nrows, 0.0);
  for(int i=0; i<nrows; i++) {
    for(int j=0; j<nrows; j++) {
      double &r = result(i, j);
      for(int k=0; k<nrows; k++)
	r += a(i, k) * b(k, j);
    }
  }
  return result;
}

const SymmMatrix &SymmMatrix::operator+=(const SymmMatrix &a) {
  int sz = nrows*(nrows+1)/2;
  double *here = *m;
  double *there = *a.m;
  for(int i=0; i<sz; i++)
    here[i] += there[i];
  return *this;
}

const SymmMatrix &SymmMatrix::operator-=(const SymmMatrix &a) {
  int sz = nrows*(nrows+1)/2;
  double *here = *m;
  double *there = *a.m;
  for(int i=0; i<sz; i++)
    here[i] -= there[i];
  return *this;
}

MV_Vector_double operator*(const SymmMatrix &a, const MV_Vector_double &x) {
  int nrows = a.nrows;
  if(x.size() != nrows) abort();
  MV_Vector_double result(nrows, 0);
  for(int i=0; i<nrows; i++) {
    double &r = result(i);
    for(int j=0; j<nrows; j++)
      r += a(i,j)*x(j);
  }
  return result;
}

// Compute A^ T (*this) A

/*
SymmMatrix SymmMatrix::transform(const MV_ColMat_double &A) const {
  if(A.size(0) != nrows) abort();
  SymmMatrix result(nrows);
  MV_ColMat_double temp(nrows, nrows, 0.0);
  int i, j, k, l;
  for(j=0; j<nrows; j++)
    for(k=0; k<nrows; k++) {
      double &t = temp(j, k);
      for(l=0; l<nrows; l++)
	t += A(j, l)*m[k][l];
    }
  for(i=0; i<nrows; i++)
    for(j=0; j<=i; j++) {
      double &r = result(i, j);
      for(k=0; k<nrows; k++)
	r += temp(j, k)*A(i, k);
    }
  return result;
}
*/

SymmMatrix SymmMatrix::transform(const MV_ColMat_double &A) const {
  if(A.size(0) != nrows) abort();
  SymmMatrix result(nrows);
  for(int i=0; i<nrows; i++) {
    for(int j=0; j<=i; j++) {
      double &r = result(i,j);
      for(int k=0; k<nrows; k++)
 	for(int l=0; l<nrows; l++)
 	  r += A(i, k) * (*this)(k, l) * A(j, l);
    }
  }
  return result;
}

SymmMatrix operator*(double x, const SymmMatrix &A) {
  SymmMatrix result(A);
  result *= x;
  return result;
}

SymmMatrix operator*(const SymmMatrix &A, double x) {
  SymmMatrix result(A);
  result *= x;
  return result;
}

extern "C" {
  void dscal_(int*, double*, double*, int*);
}

const SymmMatrix &SymmMatrix::operator*=(double x) {
  int n = (nrows*(nrows+1))/2;
  int one = 1;
  dscal_(&n, &x, m[0], &one);
  return *this;
}

const SymmMatrix &SymmMatrix::operator/=(double x) {
  return operator*=(1./x);
}

SymmMatrix operator/(const SymmMatrix &A, double x) {
  SymmMatrix result(A);
  result *= 1./x;
  return result;
}

// allocate and free storage

void SymmMatrix::allocate() {   // assumes nrows has already been set
    if(m) {
	cerr << "Error in SymmMatrix::allocate()!" << endl;
	fatalerror();
    }
    m = new double*[nrows];	    // Allocate the pointers to the rows
    int sz = nrows*(nrows+1)/2;
    // Allocate all space contiguously
    double *u = m[0] = new double[sz];
    int i;
    for(i = 1; i<nrows; i++)
	m[i] = m[i-1] + i; // Sets the row pointers   	
    for(i = 0; i<sz; i++)
	u[i] = 0;
}

void SymmMatrix::free() {
    if(nrows != 0) {
	delete [] m[0];	// delete data storage
	delete [] m;	// delete pointers to rows
    }
    m = 0;
}

void SymmMatrix::copy(double **smd) {	// copy data from another data array
    int sz = nrows*(nrows+1)/2;
    (void) memcpy(&m[0][0], &smd[0][0], sz*sizeof(double));
//     double *mb = &m[0][0];		// base of this data space
//     double *other = &smd[0][0];		// base of copied data
//     for(int i=0; i<sz; i++) mb[i] = other[i];
}

void SymmMatrix::resize(int n) {
    if(n == nrows) return;
    free();
    nrows = n;
    allocate();
}

ostream& operator<<(ostream& os, const SymmMatrix &sm) {
  os.setf(ios::scientific, ios::floatfield);
  os << "[ ";
  for(int i=0; i<sm.nrows; i++) {
    for(int j=i; j<sm.nrows; j++)
      os << setw(13) << sm(i, j) << " ";
    if(i != sm.nrows-1)
      os << "; ";
  }
  os << "]";
  return os;
}

void SymmMatrix::clear(double x) {
  for(int i=0; i<nrows; i++)
    for(int j=0; j<=i; j++)
      m[i][j] = x;
}

void SymmMatrix::binarywrite(FILE *file) const {
    writebinary(file, m[0][0], nrows*(nrows+1)/2);
}

int SymmMatrix::binaryread(FILE *file) {
    // matrix is assumed to be allocated already
    return readbinary(file, m[0][0], nrows*(nrows+1)/2);
}
