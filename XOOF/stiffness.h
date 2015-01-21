// -*- C++ -*-
// $RCSfile: stiffness.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:15 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef STIFFNESS_H
#define STIFFNESS_H

enum Fixation {F_FREE=0, F_FIXED=1};

#include "grid.h"

class Stiffness {
    private:
	Grid *g;
	int fixedrows, fixedcols;
    public:
	Stiffness(Grid *gg, Fixation frow, Fixation fcol) :
	    g(gg), fixedrows(frow), fixedcols(fcol) {}
    friend MV_Vector_double operator*(const Stiffness&,
					const MV_Vector_double &);
};


#endif
 
