// -*- C++ -*-
// $RCSfile: random.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:12 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// random number generator.

// currently just a shell for the IRIX random().

#include "charstring.h"
#include "random.h"
#include <math.h>
#include "stdlib.h"

static const long BIGGEST = 2147483647; // 2^31-1

void rndmseed(int seed) {
    srandom(seed);
}

double rndm() {
    return random()/(BIGGEST+1.0);
}

void setseed(const CharString&) { // called from menus
    rndmseed(seed);
}

// gaussian deviates, copied from numerical recipes

double gasdev() {
    static int saved = 0;
    static double save_me;
    double v1, v2, rsq;
    if(!saved) {
	do {
	    v1 = 2.0*rndm() - 1.0;
	    v2 = 2.0*rndm() - 1.0;
	    rsq = v1*v1 + v2*v2;
	} while(rsq >= 1.0 || rsq == 0);
	
	double fac = sqrt(-2.0*log(rsq)/rsq);
	save_me = v1*fac;
	saved = 1;
	return v2*fac;
    }
    else {
	saved = 0;
	return save_me;
    }
}

