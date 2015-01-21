// -*- C++ -*-
// $RCSfile: nodegroups.h,v $
// $Revision: 1.2 $
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


#ifndef NODEGROUPS_H
#define NODEGROUPS_H

class NodeGroup;

#include "charstring.h"
#include "groups.h"
#include "menuDef.h"
#include "node.h"
#include <iostream.h>

class NodeGroup : public Group<Node> {
public:
  NodeGroup(const CharString &name) : Group<Node>(name) {}
  virtual ~NodeGroup();
  void add(Node*);
};

ostream &operator<<(ostream&, const NodeGroup&);

#endif
