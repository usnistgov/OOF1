// -*- C++ -*-
// $RCSfile: grid.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-12-26 14:36:48 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// grid --

class Grid;

#ifndef GRID_H
#define GRID_H

#include "truefalse.h"
#include "sparselink.h"
#include "vec.h"
#include <stdio.h>

class CharString;
class Element;
class ElementGroup;
class MeshCoord;
class Node;
class NodeGroup;

#include "beginheader.h"

class Grid {
private:
  Grid(const Grid&);		// prohibited
  Grid &operator=(const Grid&); // prohibited
  SparseLinkMatrix<bool> connectivity; // are nodes physically connected?
  SparseLinkMatrix<Node*> edgenodes;
public:
  Vec<Element*> interiorE;    // interior elements (w/out boundary nodes)
  Vec<Node*> node;	    // nodes 
  Vec<NodeGroup*> nodegroup;
  Vec<ElementGroup*> elementgroup;
  Vec<ElementGroup*> materialgroup;

  Grid();
  Grid(FILE *);
  ~Grid();

  void make_connectivity();
  bool connected(int);		// is a node connected?
	
  // routines for constructing grid
  void setNnodes(int);		// just allocates space
  void setNelements(int);	// just allocates space
  int AddElement(Element *elem);
  void AddNode(int index, Node *newnode); // add to node list
  NodeGroup *getgroup(const CharString &, const TrueFalse&);
  ElementGroup *getEgroup(const CharString &, const TrueFalse&);

  ElementGroup *getMgroup(const Element *el) const;
  ElementGroup *makeMgroup(const CharString&);

  int Nnodes() const { return node.capacity(); }
  Node *getnode(int i) { return node[i]; }

  Node *get_edgenode(const Node*, const Node*);
  void make_edgenodes();

  void find_material_groups();
  void print_abaqus(ostream&);
  void print_nodes(ostream&);
  void print_elements(ostream&) const;
  void print_nodegroups(ostream&) const;
  void print_elementgroups(ostream&) const;
  void print_materials(ostream&) const;
  void list_materials(ostream&) const;
  void get_dimensions(MeshCoord&, MeshCoord&) const;
};

EXTERN Grid *grid DFLT(0);

#include "endheader.h"
#endif


