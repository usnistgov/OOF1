// -*- C++ -*-
// $RCSfile: sparselink.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:48 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef SPARSELINK_C
#define SPARSELINK_C

#include "sparselink.h"
#include "vec.h"
#include <iostream.h>

// fixed size
template <class TYPE>
SparseLinkMatrix<TYPE>::SparseLinkMatrix(const int n, const int m)
	    : nrows(n), ncols(m), nz(0), row(n, 0), stretchy(0)
{}

// variable size
template <class TYPE>
SparseLinkMatrix<TYPE>::SparseLinkMatrix()
    : nrows(0), ncols(0), nz(0), row(0), stretchy(1)
{}

template <class TYPE>
SparseLinkMatrix<TYPE>::~SparseLinkMatrix() {
    clear();
}

template <class TYPE>
void SparseLinkMatrix<TYPE>::setsize(int n, int m) {
  clear();
  nrows = n;
  ncols = m;
  row.resize(n, 0);
  stretchy = 0;
}

template <class TYPE>
void SparseLinkMatrix<TYPE>::clear() {
    for(int i=0; i<nrows; i++) {
	SLMentry<TYPE> *tmp = row[i], *next;
	while(tmp) {
	    next = tmp->next;
	    delete tmp;
	    tmp = next;
	}
	row[i] = 0;
    }
    if(stretchy) {
	row.resize(0);
	nrows = ncols = 0;
    }
    nz = 0;
}

// access function (i,j) creates space for element (i,j) if necessary

template <class TYPE>
TYPE &SparseLinkMatrix<TYPE>::operator()(const int i, const int j) {
    // look through list row(i)
    if(stretchy) {
	if(i >= nrows) {    // make space for new row
	    nrows = i+1;
	    row.resize(nrows, (SLMentry<TYPE>*) 0);
	}
	if(j >= ncols)
	    ncols = j+1;
    }
    
    SLMentry<TYPE> *next = row[i];
    SLMentry<TYPE> *last = 0;
    SLMentry<TYPE> *newentry;
    while(next && next->col < j) {
	last = next;
	next = next->next;
    }
    
    if(!next) { // got through the whole list without finding anything
	if(last) {
	    newentry = last->next = new SLMentry<TYPE>(j);
	    nz++;
	    return newentry->val;
	}
	// nothing in list yet
	newentry = row[i] = new SLMentry<TYPE>(j);
	nz++;
	return newentry->val;
    }
    else {	// insert before end of list
	if(next->col == j)    // entry already there
	    return next->val;
	
	if(last) {  // insert between last and next
	    newentry = last->next = new SLMentry<TYPE>(j);
	    nz++;
	    newentry->next = next;
	    return newentry->val;
	}
	// insert at beginning of list
	newentry = row[i] = new SLMentry<TYPE>(j);
	nz++;
	newentry->next = next;
	return newentry->val;
    }
}

template <class TYPE>
ostream &operator<<(ostream &os, const SparseLinkMatrix<TYPE> &slm) {
    for(int i=0; i<slm.nrows; i++) {
	for(SLMentry<TYPE> *e = slm.row[i]; e != 0; e = e->next)
	    os << i << " " << e->col << " " << e->val << endl;
    }
    return os;
}

// ------------------------------------------------- //

template <class TYPE>
SparseLinkIterator<TYPE>::SparseLinkIterator(const SparseLinkMatrix<TYPE> &m)
    : slm(&m), rownumber(0), element(0), onerow(0)
{
    nextrow();
}

template <class TYPE>
SparseLinkIterator<TYPE>::SparseLinkIterator(const SparseLinkMatrix<TYPE> &m, 
							    const int rowno)
    : slm(&m), rownumber(rowno), element(0), onerow(1)
{
    if(rowno >=0 && rowno < slm->nrows)
	element = slm->row[rowno];
}

template <class TYPE>
int SparseLinkIterator<TYPE>::operator()(int &i, int &j, TYPE &x) {
    if(!element)
	return 0;   // done
    else {
	i = rownumber;
	j = element->col;
	x = element->val;
	element = element->next;
	if(!element) {
	    rownumber++;
	    nextrow();
	}
	return 1;
    }
}

template <class TYPE>
void SparseLinkIterator<TYPE>::nextrow() {	// find next non-empty row
    if(onerow) return;
    while(rownumber < slm->nrows && !(element=slm->row[rownumber]))
	rownumber++;
}

#endif
