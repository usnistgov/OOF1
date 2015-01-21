// -*- C++ -*-
// $RCSfile: parameters.h,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2000-10-16 12:51:18 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// global stuff

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "charstring.h"
#include "truefalse.h"

#include "beginheader.h"

#ifdef THERMAL
EXTERN CharString programname DFLT("thermal_oof");
#else  // !THERMAL
EXTERN CharString programname DFLT("oof");
EXTERN double delta_T DFLT(0);	// temperature
#endif // THERMAL
EXTERN double fictiveT DFLT(0);	// fictive temperature

EXTERN int maxiter DFLT(1000);	// max iterations in iml++ routines
EXTERN double tol DFLT(1.e-5);	// tolerance for iml++ iterative methods
EXTERN TrueFalse use_globalK DFLT(1); // use global stiffness matrix?
  /* can't use TF_TRUE to initialize, since it might not be constructed yet */

EXTERN TrueFalse verbose DFLT(1);	// chatter annoyingly?

class Grid;
EXTERN Grid *grid DFLT(0);	// the world as we know it
int gridinit();			// is the grid initialized?

#include "endheader.h"

#endif
