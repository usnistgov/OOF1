// -*- C++ -*-
// $RCSfile: createelement.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:45 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef CREATEELEMENT_C
#define CREATEELEMENT_C

#include "element.h"
#include "grid.h"
#include "parameters.h"
#include "fatalerror.h"
#include "menuDef.h"
#include "stdlib.h"

template <class ELEMENT>
ELEMENT *create_element(const ELEMENT*) {
    // check nodenumbers
    for(int i=0; i<Element::nodenumber_dflt.capacity(); i++) {
	if(Element::nodenumber_dflt[i] >= grid->Nnodes()) {
	  garcon()->msout << ms_error << "nodenumber "
			  << Element::nodenumber_dflt[i]
			  << " is too big!" << endl << ms_normal;
	    fatalerror();
	}
    }
    ELEMENT *elem = new ELEMENT(grid);
    if(!grid->AddElement(elem)) return 0;
    return elem;
}

#endif
