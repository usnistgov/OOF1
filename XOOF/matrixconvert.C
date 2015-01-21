// -*- C++ -*-
// $RCSfile: matrixconvert.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:12 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// convert sparselinkmat to  sparselib compressed row format

#include "sparselink.h"
#include "comprow_double.h"
#include "vec.h"

CompRow_Mat_double CRMat(const SparseLinkMat &m) {
    Vec<int> row_ptr(m.nrows+1);
    Vec<double> val(m.nz);
    Vec<int> col_ind(m.nz);
    int nzcounter = 0;
    // loop over rows
    for(int i=0; i<m.nrows; i++) {
	row_ptr[i] = nzcounter;
	for(SLMentry<double> *p = m.row[i]; p != 0; p = p->next) {
	    val[nzcounter] = p->val;
	    col_ind[nzcounter] = p->col;
	    nzcounter++;
	}
    }
    row_ptr[m.nrows] = nzcounter;
    return CompRow_Mat_double(m.nrows, m.ncols, nzcounter,
			    &val[0], &row_ptr[0], &col_ind[0]);
}
