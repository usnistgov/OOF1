// -*- C++ -*-
// $RCSfile: generate.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:11 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef GENERATE_H
#define GENERATE_H

#include "grid.h"

// how to put diagonals in rectangles
enum DiagStyle {LEANLEFT, LEANRIGHT, ALTERNATE, RANDOM};

template <class ELEMENT, class INTERIORNODE, class BOUNDARYNODE>
Grid *generate_uniform(
	const ELEMENT*, const INTERIORNODE*, const BOUNDARYNODE*, 
	int, int, double, double, DiagStyle);

#include "generate.C"

#endif // GENERATE_H
