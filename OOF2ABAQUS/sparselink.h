// -*- C++ -*-
// $RCSfile: sparselink.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 16:49:37 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Sparse row matrix stored with linked lists, for easy construction
// Meant to be used as temporary storage while constructing comprow matrix

template <class TYPE> class SLMentry;
template <class TYPE> class SparseLinkMatrix;

#ifndef SPARSELINK_H
#define SPARSELINK_H

#include "vec.h"

template <class TYPE> class SparseLinkIterator;
template <class TYPE>
ostream &operator<<(ostream&, const SparseLinkMatrix<TYPE>&);

template <class TYPE>
class SLMentry {
    private:
	SLMentry(int c) : col(c), val(0), next(0) {}
	SLMentry(int c, TYPE v) : col(c), val(v), next(0) {}
	SLMentry *next;
	TYPE val;
	int col;
  //    friend CompRow_Mat_double CRMat(const SparseLinkMatrix<double>&);
#ifdef __GNUG__
    friend ostream &operator<< <>(ostream&, const SparseLinkMatrix<TYPE>&);
#else
    friend ostream &operator<<(ostream&, const SparseLinkMatrix<TYPE>&);
#endif
    friend class SparseLinkMatrix<TYPE>;
    friend class SparseLinkIterator<TYPE>;
};

template <class TYPE>
ostream &operator<<(ostream&, const SparseLinkMatrix<TYPE>&);

template <class TYPE>
class SparseLinkMatrix {
private:
  int nrows, ncols, nz;
  int stretchy;	// resized on the fly? no bounds check
  Vec<SLMentry<TYPE>*> row;
public:
  SparseLinkMatrix();
  SparseLinkMatrix(const int, const int);
  ~SparseLinkMatrix();
  void clear(void);	// remove all entries
  void setsize(int, int);	// set size and make nonstretchy
  // return an entry, creating space if necessary
  TYPE &operator()(const int, const int);
  int rows() const { return nrows; }
  int cols() const { return ncols; }
  //  friend CompRow_Mat_double CRMat(const SparseLinkMatrix<double>&);
#ifdef __GNUG__
  friend ostream &operator<< <>(ostream&, const SparseLinkMatrix<TYPE>&);
#else
  friend ostream &operator<<(ostream&, const SparseLinkMatrix<TYPE>&);
#endif
  friend class SparseLinkIterator<TYPE>;
};

typedef SparseLinkMatrix<double> SparseLinkMat;

// -------------- //

/* iterate over the whole matrix:
 * SparseLinkIterator<TYPE> iterator(SparseLinkMatrix<TYPE> m);
 * TYPE x;
 * int i, j;
 * while(iterator(i, j, x)) {    // sets i and j
 *	...;
 * }
 *
 * iterate over one row:
 * SparseLinkIterator<TYPE> iterator(SparseLinkMatrix<TYPE> m, const int rowno);
 * TYPE x;
 * int j;
 * while(iterator(i, j, x)) {
 *   ...;
 * }
 */

template <class TYPE>
class SparseLinkIterator {
    private:
	const SparseLinkMatrix<TYPE> *slm;
	int rownumber;
	SLMentry<TYPE> *element;    // points to next entry to be returned
	void nextrow();		    // find next non-empty row
	int onerow;
	int whichrow;
    public:
	SparseLinkIterator(const SparseLinkMatrix<TYPE> &);
	SparseLinkIterator(const SparseLinkMatrix<TYPE> &, const int);
	int operator()(int &i, int &j, TYPE &x);
};

#include "sparselink.C"

#endif // SPARSELINK_H
