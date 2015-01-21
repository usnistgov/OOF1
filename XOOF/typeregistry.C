// -*- C++ -*-
// $RCSfile: typeregistry.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:19 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "typeregistry.h"

Vec<ElementTypeRegistration*> &element_registry() {
    static Vec<ElementTypeRegistration*> *list = 0;
    if(!list) list = new Vec<ElementTypeRegistration*>;
    return *list;
}

Vec<NodeTypeRegistration*> &node_registry() {
    static Vec<NodeTypeRegistration*> *list = 0;
    if(!list) list = new Vec<NodeTypeRegistration*>;
    return *list;
}
