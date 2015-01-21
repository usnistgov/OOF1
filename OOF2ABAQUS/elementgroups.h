// -*- C++ -*-
// $RCSfile: elementgroups.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-22 15:15:24 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef ELEMENTGROUPS_H
#define ELEMENTGROUPS_H

class ElementGroup;

#include "charstring.h"
#include "element.h"
#include "groups.h"
#include "menuDef.h"
#include <iostream.h>

class ElementGroup : public Group<Element> {
public:
  ElementGroup(const CharString &name) : Group<Element>(name) {}
  virtual ~ElementGroup();
};

ostream &operator<<(ostream&, const ElementGroup&);

#endif
