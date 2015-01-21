// -*- C++ -*-
// $RCSfile: nodegroups.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-12-26 14:36:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



#include "grid.h"
#include "main.h"
#include "nodegroups.h"

NodeGroup::~NodeGroup() {
  grid->nodegroup.remove(this);
}

void NodeGroup::add(Node *node) {
  Add(node);
  node->groups.grow(1, this);
}

ostream &operator<<(ostream &os, const NodeGroup &ng) {
  os << "*Nset, nset=" << ng.name() << endl;
  int linecount=0;		// no. of numbers on one line
  for(int i=0; i<ng.size(); i++) {
    os << ng[i]->index+1;
    if(i != ng.size()-1)
      os << ", ";
    if(++linecount == numbers_per_line) {
      os << endl;
      linecount = 0;
    }
  }
  if(linecount != 0)
    os << endl;
  return os;
}

