// -*- C++ -*-
// $RCSfile: node.h,v $
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


class Node;

#ifndef NODE_H
#define NODE_H

#include "meshcoord.h"
#include "typeregistry.h"
#include "vec.h"

class NodeGroup;

// base class
class Node {
private:
  Node(const Node&);		// prohibited
  Node &operator=(const Node&);	// prohibited
  Vec<Element*> element;
public:
  MeshCoord position;
  int index;
	
  Node();
  Node(int n, double xx, double yy);
  virtual ~Node() {}

  Vec<NodeGroup*> groups;
  virtual const CharString &tag() const = 0;
	
  // additional parameters needed to define node
  virtual CharString parameters() const { return CharString(); }
  CharString flags() const;
	
  Element *neighboring(const Node*) const;

  friend ostream& operator<<(ostream&, const Node&);
};


class XYNode : public Node {
private:
  static CharString nameplate;
  virtual const CharString &tag() const { return nameplate; }
  static int binaryread(FILE*);
  static NodeTypeRegistration reg;
public:
  XYNode() {}
  XYNode(int n, double xx, double yy) : Node(n, xx, yy) {}
  static XYNode *create(const int index, double x, double y);
};

/* LinearNodes have two degrees of freedom at arbitrary angles.
 * Only the displacements are measured at an angle -- the original position
 * is cartesian.
 */

class LinearNode : public Node {
private:
  static CharString nameplate;
  virtual const CharString &tag() const { return nameplate; }
  static int binaryread(FILE*);
  static NodeTypeRegistration reg;
public:
  LinearNode() {}
  LinearNode(const int n, double x, double y) : Node(n, x, y) {}
  static LinearNode *create(const int index, double x, double y);
};

#endif

