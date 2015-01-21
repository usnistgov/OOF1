// -*- C++ -*-
// $RCSfile: stressdrawer.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:16 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Template for creating a Drawer for each ElementFunction.

#ifndef STRESSDRAWER_H
#define STRESSDRAWER_H

#include "colordrawer.h"
#include "element.h"
#include "elementfunctionbase.h"
#include "symmmatrix.h"

template <class ELEMENTFUNCTION>
class Color_Drawer : public ColorDrawer {
public:
  Color_Drawer(Grid *g, FormDrawer *fd) : ColorDrawer(g, fd) {
    setvalues();
    fullscale();
  }
  virtual const ElementFunctionBase &func() const {
    return ELEMENTFUNCTION::instance();
  }
  static Drawer *create(Grid*, FormDrawer*); // defined in elementfunction.C
};


#endif
