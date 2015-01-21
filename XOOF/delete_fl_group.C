// -*- C++ -*-
// $RCSfile: delete_fl_group.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:46 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "fatalerror.h"
#include "forms.h"
#include "delete_fl_group.h"
#include <iostream.h>
#include "stdlib.h"

extern "C" {
  void fl_delete_group(FL_OBJECT*);
}

void delete_fl_group(FL_OBJECT *grp) {
  FL_OBJECT *obj = grp->next;
  fl_hide_object(grp);
  if(grp->objclass != FL_BEGIN_GROUP) {
    cerr << "Attempt to call delete_fl_group on a non-group!" << endl;
    fatalerror();
  }
  while(obj->objclass != FL_END_GROUP) {
    FL_OBJECT *next = obj->next;
    // if obj->form == 0, then object has already been deleted. It's
    // probably part of a composite object.
    if(obj->form) {
      fl_hide_object(obj);
      fl_delete_object(obj);
      fl_free_object(obj);
    }
    obj = next;
  }
  fl_delete_group(grp);
  fl_free_object(grp);
}
