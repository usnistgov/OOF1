// -*- C++ -*-
// $RCSfile: symmmatrix.h,v $
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


// Symmetric matrix storage class
// not for linear algebra!

class SymmMatrix;

#ifndef SYMMMATRIX_H
#define SYMMMATRIX_H

#include <iostream.h>
#include <stdio.h>

class SymmMatrix {
protected:
  double **m;
  int nrows;
  void allocate();
  void free();
  void copy(double**);
public:
  SymmMatrix() : m(0), nrows(0) {}
  SymmMatrix(int);		    // specifies size
  SymmMatrix(const SymmMatrix&);	    // copy constructor
  virtual ~SymmMatrix();
  const SymmMatrix &operator=(const SymmMatrix&);
  double &operator()(int i, int j);
  double operator()(int i, int j) const;
  double &operator()(int i, int j, int mu, int nu) {
    return operator()(i+2*mu, j+2*nu);
    /* Don't change indexing here without changing setstiffness() !*/
  }
  double operator()(int i, int j, int mu, int nu) const {
    return operator()(i+2*mu, j+2*nu);
    /* Don't change indexing here without changing setstiffness() !*/
  }
  void resize(int);
  int size() const { return nrows; }
  void clear(double x=0);
  int badindex(int i) const { return i < 0 || i >= nrows; }
	
  int binaryread(FILE*);	// matrix is assumed to be allocated already
	
  friend class Debug;
  friend ostream& operator<<(ostream&, const SymmMatrix&);
};


//ostream& operator<<(ostream&, const SymmMatrix&);

#endif
