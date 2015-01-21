// -*- C++ -*-
// $RCSfile: symmmatrix.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 14:11:10 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "fatalerror.h"
#include "readbinary.h"
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

int SymmMatrix::binaryread(FILE *file) {
  // matrix is assumed to be allocated already
  return readbinary(file, m[0][0], nrows*(nrows+1)/2);
}
