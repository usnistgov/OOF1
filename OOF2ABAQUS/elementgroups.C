// -*- C++ -*-
// $RCSfile: elementgroups.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-12-22 16:49:36 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "elementgroups.h"
#include "grid.h"
#include "griditerator.h"
#include "main.h"
#include <iostream.h>

ElementGroup::~ElementGroup() {
  grid->elementgroup.remove(this);	// remove this group from list in grid
}

ostream &operator<<(ostream &os, const ElementGroup &eg) {
  os << "*Elset, elset=" << eg.name() << endl;
  int linecount = 0;
  for(int i=0; i<eg.size(); i++) {
    if(empties || !eg[i]->isempty()) {
      os << eg[i]->index+1;
      if(i != eg.size() - 1)
	os << ", ";
      if(++linecount == numbers_per_line) {
	os << endl;
	linecount = 0;
      }
    }
  }
  if(linecount != 0)
    os << endl;
  return os;
}

