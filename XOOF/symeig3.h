// -*- C++ -*-
// $RCSfile: symeig3.h,v $
// $Revision: 1.4 $
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

// Eigenvalues and eigenvectors of a symmetric 3x3 matrix

#ifndef SYMEIG_H
#define SYMEIG_H

#include <math.h>
#include <iostream.h>

class EigenValues;

// g++ 2.91.60 won't link this properly unless all EIGENVECTOR classes
// have different names!
template <class MATRIX, class EIGENVECTOR1, class EIGENVECTOR2, class EIGENVECTOR3>
void eigensystem(const MATRIX &mat,
		 EIGENVECTOR1 &eig1, EIGENVECTOR2 &eig2, EIGENVECTOR3 &eig3);

template <class MATRIX>
void eigenvalues(const MATRIX &mat, EigenValues &eig);

#include "symeig3.C"

#endif // SYMEIG_H
