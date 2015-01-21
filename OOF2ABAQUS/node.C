// -*- C++ -*-
// $RCSfile: node.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-12-22 16:49:37 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "grid.h"
#include "meshcoord.h"
#include "node.h"
#include "readbinary.h"
#include <iostream.h>

Node::Node()
  : element(0, BlockSize(3))
{}

Node:: Node(int n, double xx, double yy)
  : index(n),
    position(xx, yy),
    element(0, BlockSize(3))
{}

Element *Node::neighboring(const Node *nbr) const {
  // does this node have the given node as a neighbor?
  for(int i=0; i<element.capacity(); i++) {
    Element *elem = element[i];
    for(int j=0; j<elem->corner.capacity(); j++) {
      if(elem->corner[j] == nbr) {
	return elem;
      }
    }
  }
  return false;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

ostream &operator<<(ostream &os, const Node &n) {
  os << n.index+1 << ", " << n.position;
  return os;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

CharString XYNode::nameplate("xy");
NodeTypeRegistration XYNode::reg(nameplate, binaryread);

int XYNode::binaryread(FILE *file) {
  int index;
  char flag;
  float x, y, dx, dy;
  float fx, fy;
  if(!readbinary(file, flag)) return 0;
  if(!readbinary(file, index)) return 0;
  if(!readbinary(file, x)) return 0;
  if(!readbinary(file, y)) return 0;
  if(flag & 0x1) {
    if(!readbinary(file, dx)) return 0;
    if(!readbinary(file, dy)) return 0;
  }
  else {
    dx = dy = 0.0;
  }
  if(flag & 0x2) {
    if(!readbinary(file, fx)) return 0;
    if(!readbinary(file, fy)) return 0;
  }
  (void) create(index, x, y);
  return 1;
}


XYNode *XYNode::create(const int index, double x, double y) {
  // create a node	
  XYNode *newnode = new XYNode(index, x, y);
  grid->AddNode(index, newnode);
  return newnode;
}

// ------------------------------------------------------------//

CharString LinearNode::nameplate("linear");
NodeTypeRegistration LinearNode::reg(nameplate, binaryread);

int LinearNode::binaryread(FILE *file) {
    int index;
    char flag;
    float x, y, dx, dy;
    float t00, t01, t10, t11;
    float fx, fy;
    if(!readbinary(file, flag)) return 0;
    if(!readbinary(file, index)) return 0;
    if(!readbinary(file, x)) return 0;
    if(!readbinary(file, y)) return 0;
    if(flag & 0x1) {
	if(!readbinary(file, dx)) return 0;
	if(!readbinary(file, dy)) return 0;
    }
    else
	dx = dy = 0.0;
    if(flag & 0x2) {
	if(!readbinary(file, t00)) return 0;
	if(!readbinary(file, t01)) return 0;
	if(!readbinary(file, t10)) return 0;
	if(!readbinary(file, t11)) return 0;
    }
    else {
	t00 = t11 = 1.0;
	t01 = t10 = 0.0;
    }
    if(flag & 0x4) {
      if(!readbinary(file, fx)) return 0;
      if(!readbinary(file, fy)) return 0;
    }
    (void) create(index, x, y);
    return 1;
}


LinearNode *LinearNode::create(const int index, double x, double y) {
  // create a node	
  LinearNode *newnode = new LinearNode(index, x, y);
  grid->AddNode(index, newnode);
  return newnode;
}
