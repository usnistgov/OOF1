// -*- C++ -*-
// $RCSfile: amtriangle.C,v $
// $Revision: 1.45 $
// $Author: langer $
// $Date: 2006-01-12 16:59:53 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

//#include "config.h"

#include "adaptmesh.h"
#include "amtriangle.h"
#include "amtriangleiterator.h"
#include "colorutils.h"
#include "elector.h"
#include "goof.h"
#include "imagecanvas.h"
#include "material.h"
#include "meshcmds.h"
#include "pixelgroups.h"
#include "saveconfig.h"
#include "word.h"
#include <iostream.h>
#include <math.h>
#include "stdlib.h"
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif // HAVE_MALLOC_H

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

const unsigned char AMNode::top_ = 1;
const unsigned char AMNode::btm_ = 2;
const unsigned char AMNode::lft_ = 4;
const unsigned char AMNode::rgt_ = 8;


// put a node between two nodes
AMNode::AMNode(AdaptiveMesh *mesh, const AMNode *n1, const AMNode *n2)
  : coord_(0.5*(n1->coord() + n2->coord())),
    edgeflags(n1->edgeflags & n2->edgeflags)
{
  index = mesh->nodes.capacity();
  mesh->nodes.grow(1, this);
}

AMNode::AMNode(AdaptiveMesh *mesh, double x, double y,
	       unsigned char e)
  : coord_(x, y),
    edgeflags(e)
{
  index = mesh->nodes.capacity();
  mesh->nodes.grow(1, this);
}

void AMNode::move_to(const MeshCoord &p) {
  lastmoved = nodemoved;
  lastcoord = coord_;
  ++nodemoved;
  coord_ = p;
  for(int i=0; i<triangle.capacity(); i++)
    ++triangle[i]->lastmod_time;
}

void AMNode::move_by(const MeshCoord &dp) {
  lastmoved = nodemoved;
  lastcoord = coord_;
  ++nodemoved;
  coord_ += dp;
  for(int i=0; i<triangle.capacity(); i++)
    ++triangle[i]->lastmod_time;
}

void AMNode::move_back() {
  coord_ = lastcoord;
  for(int i=0; i<triangle.capacity(); i++)
    ++triangle[i]->lastmod_time;
  nodemoved = lastmoved;
}

AMNode *AMNode::copy() const {
  AMNode *newnode = new AMNode;
  newnode->index = index;
  newnode->edgeflags = edgeflags;
  newnode->coord_ = coord_;
  newnode->lastcoord = lastcoord;
  newnode->lastmoved = lastmoved;
  // don't copy triangle list -- will be done when triangles are copied
  return newnode;
}

bool AMNode::active() const {
  return current_goof->active(coord_);
}

ostream &operator<<(ostream &os, const AMNode &node) {
  return os << node.coord_;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Routines for saving and reading nodes in configuration files

// Don't save the index, because nodes will be stored in order, and
// don't save the timestamps, so that the quantities they govern will
// be recomputed automatically.

void AMNode::save(ostream &file) const {
  int ef = edgeflags;		// writing unsigned chars is dangerous
  file << ef << " " << coord_ << " " << lastcoord << endl;
}

void AMNode::read(AdaptiveMesh *mesh, istream &file) {	// static
  int edgeflags;
  MeshCoord coord, lastcoord;
  file >> edgeflags >> coord >> lastcoord;
  // constructor puts node in list in mesh
  if(file) {
    AMNode *knowed = new AMNode(mesh, coord.x, coord.y, edgeflags);
    knowed->lastcoord = lastcoord;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//


// Comparison operators on Nodes are used in
// AMTriangle::list_other_nodes and when comparing AMTriangles. We'll
// never be comparing Nodes that are in different Meshes, so it's
// sufficient to compare their indices.

int operator!=(const AMNode &n1, const AMNode &n2) {
  return (n1.index != n2.index);
}

int operator<(const AMNode &n1, const AMNode &n2) {
  return (n1.index < n2.index);
}

int operator>(const AMNode &n1, const AMNode &n2) {
  return (n1.index > n2.index);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Enum<TriangleDivisionMethod> AMTriangle::divisionmethod(TDM_LONGESTEDGE);

void AMTriangle::init() {
  // call this just once
  static int once = 0;
  if(once) return;
  Enum<TriangleDivisionMethod>::name(TDM_NEWESTNODE, "newest_node");
  Enum<TriangleDivisionMethod>::name(TDM_LONGESTEDGE, "longest_edge");
  Enum<TriangleDivisionMethod>::name(TDM_E, "smallest_E");
  once = 1;
  return;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

const unsigned char AMTriangle::SELECTED = 0x1;
// There used to be more flags defined here.

static int trianglecount = 0;

AMTriangle::AMTriangle(AMTriangle *mom,
		       AMNode *n0, AMNode *n1, AMNode *n2)
  : parent(mom),
    status_(0),
    representative_material(0),
    swapped(0),
    inhibit_inheritance(0),
    inhibit_groupinheritance(0),
    index(trianglecount++)	// used for debugging and reused for output
{
  node[0] = n0;
  node[1] = n1;
  node[2] = n2;
  n0->add_triangle(this);
  n1->add_triangle(this);
  n2->add_triangle(this);
  child[0] = child[1] = 0;
  if(parent) {
    mesh = parent->mesh;
    if(parent->selected()) select();
    generation = parent->generation+1;
    initial_generation = parent->initial_generation;
    if(mesh->depth < generation) mesh->depth = generation;
  }
  else {
    mesh = 0; // hopefully will be set later
    generation = 0;
    initial_generation = 0;
  }
  neighbor[0] = neighbor[1] = neighbor[2] = 0;
  Ecalculated.backdate();	// force initial calculation of E
  representative_material_time.backdate();
  ++lastmod_time;
}

AMTriangle::~AMTriangle() {
  // Don't call AMNode::remove_triangle(this), because when the mesh
  // is destructed, the nodes are destructed before the triangles!
  mesh->goof->triangle_destroyed(this);
  if(child[0]) delete child[0];
  if(child[1]) delete child[1];
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AMTriangle::save(ostream &file) {
  file << node[0]->index << " "
       << node[1]->index << " "
       << node[2]->index << " "
       << selected() << " "
       << (child[0] != 0) << endl;

  // added for config_version 3 -- all triangles store their own materials
  file << inhibit_inheritance
       << " " << resolve_material()->index() << endl;

  // added for config_version 3 -- all triangles store their own groups
  file << inhibit_groupinheritance
       << " " << meshgroups.capacity();
  for(int i=0; i<meshgroups.capacity(); i++) {
    file << " " << meshgroups[i]->query_name();
  }
  file << endl;

  if(child[0]) {
    child[0]->save(file);
    child[1]->save(file);
  }
}

AMTriangle *AMTriangle::read(istream &file, AdaptiveMesh *mesh,
			     AMTriangle *parent)
{
  int nodeindex[3];
  file >> nodeindex[0] >> nodeindex[1] >> nodeindex[2];
  if(!file) return 0;

  bool slctd;
  file >> slctd;
  if(!file) return 0;

  bool is_parent;
  file >> is_parent;
  if(!file) return 0;

  bool inhibited = 0;
  int mat = 0;
  if(configversion() == 2) {
    file >> inhibited;
    if(!file) return 0;
    if(inhibited) {
      file >> mat;
      if(!file) return 0;
    }
  }
  
  bool group_inhibited = 0;
  Vec<Word*> glist;
  if(configversion() >= 3) {
    file >> inhibited;
    if(!file) return 0;
    file >> mat;
    if(!file) return 0;
    file >> group_inhibited;
    if(!file) return 0;
    int ngrps;
    file >> ngrps;
    if(!file) return 0;
    for(int i=0; i<ngrps; i++) {
      Word *grp = new Word;
      file >> *grp;
      if(!file) return 0;
      glist.grow(1, grp);
    }
  }

  AMTriangle *triangle = new AMTriangle(parent,
					mesh->nodes[nodeindex[0]],
					mesh->nodes[nodeindex[1]],
					mesh->nodes[nodeindex[2]]);
  if(slctd)
    triangle->select();

  triangle->mesh = mesh;

  triangle->set_material(Material::allmaterials[mat], inhibited);

  if(group_inhibited)
    triangle->inhibit_group_inheritance();
  for(int i=0; i<glist.capacity(); i++) {
    mesh->find_group(*glist[i])->append(triangle);
    delete glist[i];
  }

  if(is_parent) {
    // Oops.  AMTriangle constructor called Node::add_triangle(), but
    // we're reading a refined triangle.  When triangles are refined,
    // AMTriangle::divide() calls Node::remove_triangle().  We need to
    // call that by hand here.  This is ugly, and should be done
    // better in some later version.
    mesh->nodes[nodeindex[0]]->remove_triangle(triangle);
    mesh->nodes[nodeindex[1]]->remove_triangle(triangle);
    mesh->nodes[nodeindex[2]]->remove_triangle(triangle);
    // read the children
    triangle->child[0] = read(file, mesh, triangle);
    triangle->child[1] = read(file, mesh, triangle);
  }
  return triangle;
}

ostream &operator<<(ostream &os, const AMTriangle &triangle) {
  os << "triangle "
     << *triangle.node[0] << " "
     << *triangle.node[1] << " "
     << *triangle.node[2];
  return os;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AMTriangle::mark_for_division() {
  // See the comment in AMTriangle::divide() wrt the order in which
  // triangles are marked for division.
  mesh->dividelist.grow(1, this);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AMTriangle::divide() {
  if(divided())
    return;

  //                           .   <-- node div of parent
  //                          /|\ 
  //                         /   \ 
  //         *this ---->    /  |  \ 
  //                       / 0   1 \  <--- child numbers
  //                      /    |    \ 
  //  node1 of parent -> /_____._____\   <--- node2 of parent
  //                     \     ^newnode
  //    		  \         / 
  //                       \  nbr  /
  //                        \     /
  // 
  //                           .
  //                          /|\ 
  //     Node numbering      /1|2\      For all triangles, neighbor[i]
  //     of children        /  |  \         is opposite node[i]
  //                       /   |   \   
  //                      /    |    \ 
  //                     /2___0|0___1\ 

  bool divide_child[2];
  bool divide_nbr = false;
  divide_child[0] = divide_child[1] = false;

  int div;			// node through which to divide triangle

  if((mesh->max_divisions > 0 && division_depth() >= mesh->max_divisions)
     || (area() <= mesh->min_area)) {
    // Triangle has been divided too much already.  Divide again only if
    // division is forced by a neighbor, and divide compatibly with
    // the neighbor, so that no further divisions will be needed.  If
    // two or more neighbors force division, then this should use the
    // current divisionmethod to chose between them, but it doesn't.
    bool forced = false;
    for(int i=0; i<3 && !forced; i++)
      if(neighbor[i] && neighbor[i]->divided()) {
	// If the neighbor has been divided, then it has to have been
	// divided through the edge it shares with this triangle.  If
	// it had been divided in some other direction, then one of
	// its children would be this triangle's neighbor instead!
	div = i;
	forced = true;
      }
    if(!forced)			// division wasn't forced
      return;			// don't divide
  }
  else {			// triangle hasn't been divided too much
     div = divider();		// choose optimal direction
  }

  divided_node = div;

  AMTriangle *nbr =  neighbor[div]; // neighbor across divided edge
  AMNode *newnode = 0;		// new node
  if(nbr && nbr->divided())
    newnode = nbr->child[0]->node[0];
  else
    newnode = new AMNode(mesh, node[(div+1)%3], node[(div+2)%3]);

  // create children

  int node1 = (div + 1) % 3;
  int node2 = (div + 2) % 3;
  child[0] = new AMTriangle(this, newnode, node[div], node[node1]);
  child[1] = new AMTriangle(this, newnode, node[node2], node[div]);

  // update each node's list of triangles
  node[0]->remove_triangle(this);
  node[1]->remove_triangle(this);
  node[2]->remove_triangle(this);

  // update the neighbor pointers
  child[0]->neighbor[2] = child[1];// children are neighbors of each other
  child[1]->neighbor[1] = child[0];
  child[0]->neighbor[0] = neighbor[node2]; // neighbors of parent are
  child[1]->neighbor[0] = neighbor[node1]; //   neighbors of the children
  if(neighbor[node1])
    neighbor[node1]->replace_neighbor(this, child[1]);
  if(neighbor[node2])
    neighbor[node2]->replace_neighbor(this, child[0]);

  // Mark children for division if they have neighbors that have been
  // divided.
  if(neighbor[node2] && neighbor[node2]->divided())
    divide_child[0] = true;
  if(neighbor[node1] && neighbor[node1]->divided())
    divide_child[1] = true;

  if(nbr) {
    if(nbr->divided()) {
      //      cerr << "Neighbor " << nbr->index << " has been divided" << endl;

      // Neighbor has been divided compatibly, so its descendents are
      // the neighbors of this triangle's children.  If it's been
      // divided incompatibly, then it shouldn't be a neighbor-- one
      // of its descendents should be the neighbor!

      // The neighbors children could have already been divided, so
      // they may not be the neighbors of this triangle's
      // children. Find which of the descendents of nbr are adjacent
      // to this triangle's children.
      AMTriangle *nbr0 =
	nbr->child[1]->find_nbr_child(this, newnode, node[node1]);
      child[0]->neighbor[1] = nbr0;
      if(nbr0) {
	nbr0->neighbor[nbr0->neighbor_no(newnode, node[node1])] = child[0];
	if(nbr0->divided())
	  // nbr0 is divided through the edge it shares with child[0]
	  child[0]->mark_for_division(); // should be divide_child[0] = true; ?
      }

      AMTriangle *nbr1 =
	nbr->child[0]->find_nbr_child(this, newnode, node[node2]);
      child[1]->neighbor[2] = nbr1;
      if(nbr1) {
	nbr1->neighbor[nbr1->neighbor_no(newnode, node[node2])] = child[1];
	if(nbr1->divided())
	  child[1]->mark_for_division();
      }
    }
    else {
      divide_nbr = true;
      child[0]->neighbor[1] = 0; // will be assigned when nbr is divided
      child[1]->neighbor[2] = 0;
    }
  }

  for(int i=0; i<2; i++)
    if(divide_child[i])
      child[i]->mark_for_division();
  if(divide_nbr)
    nbr->mark_for_division();

  // transfer groups to children
  child[0]->meshgroups.resize(meshgroups.capacity());
  child[1]->meshgroups.resize(meshgroups.capacity());
  child[0]->inhibit_groupinheritance = inhibit_groupinheritance;
  child[1]->inhibit_groupinheritance = inhibit_groupinheritance;
  for(int i=0; i<meshgroups.capacity(); i++) {
    child[0]->meshgroups[i] = meshgroups[i];
    child[1]->meshgroups[i] = meshgroups[i];
    meshgroups[i]->replace(this, child[0]);
    meshgroups[i]->append(child[1]);
  }
  meshgroups.smash();

  // transfer material to children
  child[0]->representative_material = representative_material;
  child[1]->representative_material = representative_material;
  child[0]->inhibit_inheritance = inhibit_inheritance;
  child[1]->inhibit_inheritance = inhibit_inheritance;
  // make sure material is updated as soon as possible:
  child[0]->representative_material_time.backdate();
  child[1]->representative_material_time.backdate();

  // parent is no longer selected
  unselect();

  mesh->goof->triangle_destroyed(this);

#ifdef DEBUG
  if(AdaptiveMesh::continuous_redraw)
    mesh->goof->redraw();
#endif
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AMTriangle::listnbrs(ostream &os) const { // used for debugging
  for(int i=0; i<3; i++)
    if(neighbor[i])
      os << neighbor[i]->index << " ";
    else 
      os << "none ";
}

bool AMTriangle::replace_neighbor(AMTriangle *oldnbr, AMTriangle *newnbr) {
  for(int i=0; i<3; i++)
    if(neighbor[i] == oldnbr) {
      neighbor[i] = newnbr;
      return true;		// replacement was successful
    }
  return false;			// replacement wasn't made
}

// Find which neighbor contains nodes n1 and n2, assuming that this
// triangle actually has nodes n1 and n2.

int AMTriangle::neighbor_no(const AMNode *n1, const AMNode *n2) const {
  for(int i=0; i<3; i++) {
    if(node[i] != n1 && node[i] != n2)
      return i;
  }
  cerr << "Error in AMTriangle::neighbor_no()!" << endl;
  return -1;			// should throw an exception?
}

// Find the smallest child of this triangle that contains nodes n1 and
// n2.  It is assumed that this and nbr both share nodes n1 and n2.

AMTriangle *AMTriangle::find_nbr_child(AMTriangle *nbr,
				   const AMNode *n1, const AMNode *n2)
{
  // check that this contains n1 and n2
  bool n1found = false;
  bool n2found = false;
  for(int i=0; i<3; i++) {
    if(node[i] == n1) n1found = true;
    if(node[i] == n2) n2found = true;
  }
  if(!n1found || !n2found)	// got the wrong triangle
    return 0;

  if(!divided()) {		// this is the neighbor
    return this;
  }
  else {
    AMTriangle *c0 = child[0]->find_nbr_child(nbr, n1, n2);
    if(c0) return c0;		// child[0] is the smallest neighbor
    AMTriangle *c1 = child[1]->find_nbr_child(nbr, n1, n2);
    if(c1) return c1;		// child[1] is the smallest neighbor
    // neither child contains nodes n1 and n2; this is the neighbor
    return this;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// return the node which, when the triangle is divided through it,
// produces the smallest E

Vec<int> AMTriangle::smallest_E_nodes() const {
  Vec<int> smallest;
  smallest.setphysicalsize(3);
  double e[3];
  double min = e[0] = E_tentative_divide(0);
  int which = 0;
  for(int i=1; i<3; i++) {
    e[i] = E_tentative_divide(i);
    if(e[i] < min) {
      min = e[i];
      which = i;
    }
  }
  smallest.grow(1, which);
  for(int i=1; i<3; i++) {
    int j = (which+i)%3;
    if(e[j] == min)
      smallest.grow(1, j);
  }
  return smallest;
}

// find E of the children if the triangle were divided through node nodeno

double AMTriangle::E_tentative_divide(int nodeno) const {
  AMTriangle *child0 = new AMTriangle;
  AMTriangle *child1 = new AMTriangle;
  AMNode *newnode = new AMNode;
  newnode->move_to(0.5*(node[(nodeno+1)%3]->coord() +
			node[(nodeno+2)%3]->coord()));
  child0->node[0] = node[nodeno];
  child0->node[1] = node[(nodeno+1)%3];
  child0->node[2] = newnode;
  child1->node[0] = node[nodeno];
  child1->node[1] = newnode;
  child1->node[2] = node[(nodeno+2)%3];
  child0->mesh = mesh;
  child1->mesh = mesh;
  child0->child[0] = 0;
  child0->child[1] = 0;
  child1->child[0] = 0;
  child1->child[1] = 0;

  double e = child0->E() + child1->E();

  delete child0;
  delete child1;
  delete newnode;
  return e;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AMTriangle::divider() const { // which edge should be divided?

  if(divisionmethod == TDM_NEWESTNODE)
    return 0;			// newest node is always node 0

  // Some methods can be ambiguous, and return a vector of possible directions.
  Vec<int> choices;
  if(divisionmethod == TDM_LONGESTEDGE)
    choices = longestedges();
  else if(divisionmethod == TDM_E)
    choices = smallest_E_nodes();
  if(choices.capacity() == 1)
    return choices[0];
  // Have to decide between more than one option.
  // Chose to divide compatibly with a neighbor, if possible.
  for(int i=0; i<choices.capacity(); i++) {
    if(neighbor[i] && neighbor[i]->divided() &&
       neighbor[i]->neighbor[neighbor[i]->divided_node] == this)
      return i;
  }
  // No neighbors have been divided.  At this point we could try to
  // divide through an edge with no neighbors, but that's hard to
  // detect, since neighbor[i]==0 if there's no neighbor, OR if this
  // triangle has been more refined than its neighbor.  So give up,
  // and return the first choice.
  return choices[0];
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

const Vec<MeshGroup*> &AMTriangle::mesh_groups() {
  mesh->inherit_pixel_groups();
  return meshgroups;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// A triangle is active if any of its nodes is active.

bool AMTriangle::active() const {
  for(int i=0; i<3; i++)
    if(node[i]->active()) return 1;
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AMTriangle::nnbrs_selected() const {
  int n = 0;
  for(int i=0; i<3; i++)
    if(neighbor[i] && neighbor[i]->selected())
      n++;
  return n;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// find closest pixel from a list of pixels
const Cell_coordinate
&AMTriangle::closest_pixel(const Vec<Cell_coordinate>& plist) const
{
  MeshCoord middle = center();
  int closest = 0;
  double d = sq_distance(middle, mesh->cellcenter(plist[0]));
  for(int i=1; i<plist.capacity(); i++) {
    double dd = sq_distance(middle, mesh->cellcenter(plist[i]));
    if(dd < d) {
      dd = d;
      closest = i;
    }
  }
  return plist[closest];
}

// find closest pixel in whole image (ie pixel containing center of triangle)
Cell_coordinate AMTriangle::closest_pixel() const {
  MeshCoord middle = center();
  return Cell_coordinate(middle.x, middle.y);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AMTriangle::contains(const MeshCoord &pt) const {
  // This can fail due to round-off error!
  MeshCoord s0 = node[0]->coord() - pt;
  MeshCoord s1 = node[1]->coord() - pt;
  MeshCoord s2 = node[2]->coord() - pt;
  return ((s0%s1 >= 0) && (s1%s2 >= 0) && (s2%s0 >= 0));
}

// int AMTriangle::contains(const Cell_coordinate &pt) const {
//   return contains(mesh->cellcenter(pt));
// }

AMTriangle *AMTriangle::child_containing(const MeshCoord &pt)
{
  if(!child[0]) return this;	// fn can't be const, if it returns "this".
  const MeshCoord &nexus = child[0]->node[0]->coord();
  const MeshCoord septum = child[0]->node[1]->coord() - nexus;
  // Using child[0]->node[1] instead of simply node[0] allows this to
  // work even if this triangle was divided through a node other than
  // node[0].

  if(septum % (pt - nexus) > 0)
    return child[0]->child_containing(pt);
  else
    return child[1]->child_containing(pt);
}

AMTriangle *AMTriangle::child_containing(const Cell_coordinate &pt) {
  return child_containing(mesh->cellcenter(pt));
}

// Quick check to see if a triangle is outside a rectangle.  A
// negative response does NOT mean that the triangle is inside, just
// that the quick check failed to prove that it is outside.

bool AMTriangle::outside(const Rectangle &rect) const {
  double x[3], y[3];
  for(int i=0; i<3; i++) {
    x[i] = node[i]->coord().x;
    y[i] = node[i]->coord().y;
  }
  bool left = true;
  bool right = true;
  bool above = true;
  bool below = true;
  for(int i=0; i<3; i++) {
    left = left && x[i] < rect.ll.x;
    right = right && x[i] > rect.ur.x;
    below = below && y[i] < rect.ll.y;
    above = above && y[i] > rect.ur.y;
  }
  return left || right || above || below;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Find the neighbor in the direction of a given pt, by finding which
// edge of the triangle intersects the line between the center of the
// triangle and the pt.

AMTriangle *AMTriangle::neighbor_towards(const MeshCoord &pt) const {
  int i;
  MeshCoord cntr(center());
  MeshCoord dest(pt - cntr);	// vector from center to pt (destination)

  MeshCoord r[3];		// vectors from center to node
  // cross products of dest with vectors from center to nodes of triangle
  double cp[3];
  double dp[3];			// dot products
  for(i=0; i<3; i++) {
    r[i] = node[i]->coord() - cntr;
    cp[i] = cross(dest, r[i]);
    dp[i] = dot(dest, r[i]);
  }

  // Two of the cross products have the same sign, so their nodes are
  // on the same side of the line between pt and center. The third
  // cross product corresponds to a node that is at one end of the
  // edge we're seeking.  If one cross product is zero, then the other
  // two must have opposite signs, and it doesn't matter which we
  // pick.  If two or more cross products are zero, something is
  // wrong!

  // Once one end is found, the other end is the node with the larger
  // *normalized* dot product with dest.  The neighbor we want to return
  // is the neighbor whose number is the same as the node that ISN'T
  // on the intersecting edge, because the neighbor number is the
  // number of the node opposite it in the triangle.

  for(i=0; i<3; i++) {
    int j = (i == 2 ? 0 : i+1);
    int k = (i == 0 ? 2 : i-1);
    if(cp[j]*cp[k] > 0) {	// cross products j and k have the same sign
      dp[j] /= r[j].norm();
      dp[k] /= r[k].norm();
      if(dp[j] > dp[k])
	return neighbor[k];
      else if(dp[k] > dp[j])
	return neighbor[j];
      else {
	// dp[j] == dp[k].  Corners j and k are on a line
	// perpendicular to dest, and they're on the same side of the
	// line. Which corner we want depends on whether they're
	// upstream or downstream from i.  Since the vectors all
	// originate at the center of the triangle, if dp[i]==0 then
	// dp[j]!=dp[k], and so we don't have to worry about dp[i]==0
	// here.
	if(dp[i] < 0) {
	  // dp[j]==dp[k] > 0, and the point with the smaller cross
	  // product is the other end of the intersecting edge.
	  if(cp[j] < cp[k])
	    return neighbor[k];
	  else
	    return neighbor[j];
	}
	else {			// dp[i] > 0
	  if(cp[j] > cp[k])
	    return neighbor[k];
	  else
	    return neighbor[j];
	}
      }
    }
  }

  // If we got this far, then no pair of cross products have the same
  // sign, so one of the corners must lie on the line between center
  // and pt.
  for(i=0; i<3; i++) {
    if(cp[i] == 0.0) {
      if(dp[i] > 0)
	return neighbor[i==2? 0 : i+1];	// node i is upstream, choose other edge
      else
	return neighbor[i];	// node i is downstream
    }
  }

  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static const double third = 1./3.;
MeshCoord AMTriangle::center() const {
  return third*(node[0]->coord() + node[1]->coord() + node[2]->coord());
}

double AMTriangle::area() const {
  return trianglearea(node[0]->coord(), node[1]->coord(), node[2]->coord());
}

Vec<int> AMTriangle::longestedges() const {
  Vec<int> longest;
  longest.setphysicalsize(3);
  double length[3];
  double max = -1;
  int which;
  for(int i=0; i<3; i++) {
    MeshCoord edge = node[(i+1)%3]->coord() - node[(i+2)%3]->coord();
    length[i] = dot(edge, edge);
    if(length[i] > max) {
      max = length[i];
      which = i;
    }
  }
  longest.grow(1, which);
  for(int i=1; i<3; i++) {
    int j = (which+1)%3;
    if(length[j] == max)
      longest.grow(1, j);
  }
//    if(longest.capacity() > 1) {
//      cerr <<"lengths = ";
//      for(int i=0; i<3; i++)
//        cerr << sqrt(length[i]) << " ";
//      cerr << "longestedges found " << longest.capacity()
//  	 << "	 edges with length " << sqrt(max) << endl;
//    }
  return longest;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AMTriangle::list_other_nodes(const AMNode *n, Vec<AMNode*> &other) const {
  for(int i=0; i<3; i++)
    if(node[i] != n) {
      int found = 0;
      for(int k=0; k<other.capacity() && !found; k++)
	if(other[k] == node[i])
	  found = 1;
      if(!found)
	other.grow(1, node[i]);
    }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AMTriangle::draw(ImageCanvas &canvas) const {
  XPoint xp[4];
  xp[0] = canvas.mesh2xpoint(node[0]->coord());
  xp[1] = canvas.mesh2xpoint(node[1]->coord());
  xp[2] = canvas.mesh2xpoint(node[2]->coord());
  xp[3] = xp[0];
  XDrawLines(gfxinfo.display(), canvas.pixmap(), canvas.gc(), xp, 4,
	     CoordModeOrigin);
}

void AMTriangle::draw_selected(ImageCanvas &canvas) const {
  if(selected()) {
    XPoint xp[3];
    xp[0] = canvas.mesh2xpoint(node[0]->coord());
    xp[1] = canvas.mesh2xpoint(node[1]->coord());
    xp[2] = canvas.mesh2xpoint(node[2]->coord());
    XFillPolygon(gfxinfo.display(), canvas.pixmap(), canvas.gc(), xp, 3,
		 Convex, CoordModeOrigin);
  }
}

void AMTriangle::fill(ImageCanvas &canvas) const {
  XSetForeground(gfxinfo.display(), canvas.gc(), color.pixel);
  XPoint xp[3];
  xp[0] = canvas.mesh2xpoint(node[0]->coord());
  xp[1] = canvas.mesh2xpoint(node[1]->coord());
  xp[2] = canvas.mesh2xpoint(node[2]->coord());
  XFillPolygon(gfxinfo.display(), canvas.pixmap(), canvas.gc(), xp, 3,
	       Convex, CoordModeOrigin);
}

void AMTriangle::outline(ImageCanvas &canvas, const Color &outlinecolor) const {
  XSetForeground(gfxinfo.display(), canvas.gc(), outlinecolor.pixel);
  XSetLineAttributes(gfxinfo.display(), canvas.gc(),
		     4, LineSolid, CapButt, JoinBevel);	// 4 is line width
  XPoint xp[4];
  xp[0] = canvas.mesh2xpoint(node[0]->coord());
  xp[1] = canvas.mesh2xpoint(node[1]->coord());
  xp[2] = canvas.mesh2xpoint(node[2]->coord());
  xp[3] = xp[0];
  XDrawLines(gfxinfo.display(), canvas.pixmap(), canvas.gc(), xp, 4,
	     CoordModeOrigin);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AMTriangle::writegoof(FILE *file) {
  Material *matl = material();
  unsigned char flag = 0;
  if(matl)
    matl->output(file, flag, node[0]->index, node[1]->index, node[2]->index);
  else
    defaultmaterial->output(file, flag,
			    node[0]->index, node[1]->index, node[2]->index);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// elect the most popular material

void AMTriangle::elect_material(Elector &elector) {
  if(representative_material_time > mesh->material_recompute_requested &&
     representative_material_time > lastmod_time) return;
  for(AMTriangleIterator it(*this); !it.end(); ++it) {
    const Cell_coordinate pixel = (*this)[it];
    elector.vote((*this)[it], intersection(pixel));
  }
  if(elector.turnout() == 0.0) {
    cerr << "There's something wrong!" << endl;
    for(AMTriangleIterator it2(*this); !it2.end(); ++it2) {
      Cell_coordinate pixel = (*this)[it2];
      cerr << pixel << " " << intersection(pixel) << endl;
    }
  }
  elector.tally();
  //  material_cell = elector.winner();
  representative_material = mesh->goof->material[elector.winner()];
  ++representative_material_time;
}

void AMTriangle::choose_center_material() {
  if(representative_material_time > mesh->material_recompute_requested &&
     representative_material_time > lastmod_time) return;
  //  material_cell = closest_pixel();
  representative_material = mesh->goof->material[closest_pixel()];
  ++representative_material_time;
}

Material *AMTriangle::material() {
  inherit_material();
  return representative_material;
}

Material *AMTriangle::resolve_material() {
  Material *mat = material();
  if(!mat)
    return defaultmaterial;
  return mat;
}

// Cell_coordinate AMTriangle::representative_material_cell() {
//   inherit_material();
//   return material_cell;
// }

void AMTriangle::inherit_material(bool forced) {
  if(forced || !inhibit_inheritance || !representative_material) {
    if(representative_material_time > mesh->material_recompute_requested && 
       representative_material_time > lastmod_time && !forced) return;
    switch(int(materialtransfermethod)) {
    case MTM_ELECTION:
      elect_material(material_elector);
      break;
    case MTM_CENTERPIXEL:
      choose_center_material();
      break;
    }
  }
}

void AMTriangle::set_material(Material *mat, bool inhibit) {
  ++representative_material_time;
  inhibit_inheritance = inhibit;
  representative_material = mat;
}

// is this triangle's material different from any of its neighbors?
bool AMTriangle::is_interface() {
  Material *my_material = material();
  if(!my_material) {		// this triangle has no material ...
    for(int i=0; i<3; i++)
      if(neighbor[i] && neighbor[i]->material())
	return 1;		// ... but neighbor has a material
    return 0;			// this tri and all nbrs have no material
  }
  // this triangle has a material assigned to it
  for(int i=0; i<3; i++)
    if(neighbor[i])
      if(!neighbor[i]->material() || // nbr has no material ...
	 (neighbor[i]->material() && // ... or nbr has a different material
	  !(*my_material == *neighbor[i]->material())))
	return 1;
  return 0;
}

// is this triangle's material different from at least two of its neighbors?
bool AMTriangle::is_double_interface() {
  Material *my_material = material();
  int nnbrs = 0;		// number of neighboring triangles
  int ndiff = 0;		// number which have different materials
  for(int i=0; i<3; i++) {
    if(neighbor[i]) {
      nnbrs++;
      Material *nbr_material = neighbor[i]->material();
      if(my_material) {
	if(!nbr_material || !(*my_material == *nbr_material))
	  ndiff++;
      }
      else {			// no material assigned to this triangle
	if(nbr_material)
	  ndiff++;
      }
    }
  }
  if(nnbrs == 1 && ndiff == 1) return 1;
  if(nnbrs == 2 && ndiff >= 1) return 1;
  if(nnbrs == 3 && ndiff >= 2) return 1;
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Inherit groups from pixels

void AMTriangle::elect_groups(Elector &elector) {
  if(group_cell_time > mesh->groups_recompute_requested) return;
  for(AMTriangleIterator it(*this); !it.end(); ++it) {
    const Cell_coordinate pixel = (*this)[it];
    elector.vote((*this)[it], intersection(pixel));
  }
  elector.tally();
  ++group_cell_time;
  group_cell = elector.winner();
}

void AMTriangle::copy_groups(const Cell_coordinate &pxl) {
  const LinkList<PixelGroup*> &grplist = mesh->goof->pixelgrouplist[pxl];
  for(LinkListIterator<PixelGroup*> j = grplist.begin(); !j.end(); ++j) {
    // find or create corresponding group in mesh
    MeshGroup *g = mesh->find_group(grplist[j]->query_name());
    // add this triangle to the group
    g->append(this);
  }
  ++meshgroups_time;
}

void AMTriangle::inherit_elected_groups(Elector &elector) {
  elect_groups(elector);	// computes group_cell
  copy_groups(group_cell);
}

void AMTriangle::inherit_center_groups() {
  copy_groups(closest_pixel());
}

void AMTriangle::inherit_all_groups() {
  // list of all groups contained in any pixel
  Vec<MeshGroup*> allgroups(0, BlockSize(100));

  for(AMTriangleIterator it(*this); !it.end(); ++it) {
    const LinkList<PixelGroup*> &grplist
      = mesh->goof->pixelgrouplist[(*this)[it]];
    // loop over all groups in pixel
    for(LinkListIterator<PixelGroup*> j = grplist.begin(); !j.end(); ++j) {
      // see if there are any new groups
      int found = 0;
      const CharString &grpname = grplist[j]->query_name();
      for(int k=0; k<allgroups.capacity() && !found; k++) {
	if(grpname == allgroups[k]->query_name())
	  found = 1;
      }
      // add new group to allgroups, and also to Mesh::grouplist
      if(!found)
	allgroups.grow(1, mesh->find_group(grpname));
    }
  }
  
  // put this triangle in allgroups
  for(int n=0; n<allgroups.capacity(); n++)
    allgroups[n]->append(this);

  ++meshgroups_time;
}

void AMTriangle::inherit_groups(bool forced) {
  if(forced || !inhibit_groupinheritance) {
    if(meshgroups_time > mesh->groups_recompute_requested && !forced)
      return;
    switch(int(grouptransfermethod)) {
    case GTM_ELECTION:
      inherit_elected_groups(group_elector);
      break;
    case GTM_ALLPIXELS:
      inherit_all_groups();
      break;
    case GTM_CENTERPIXEL:
      inherit_center_groups();
      break;
    }
  }
}

// static function used by AdaptiveMesh::remove_group() to see if a
// triangle can be removed from a group.

int AMTriangle::inhibittest( AMTriangle *&tri) {
  return !tri->inhibit_groupinheritance;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Add a group to the triangle, but not if it's already there.  This
// could be done more efficiently by keeping the list sorted, but is
// it worth the overhead? We don't expect triangles to be in very many
// groups.

void AMTriangle::add_group(MeshGroup *grp) {
  const CharString &name = grp->query_name();
  for(int i=0; i<meshgroups.capacity(); i++) {
    if(meshgroups[i]->query_name() == name)
      return;
  }
  meshgroups.grow(1, grp);
}

void AMTriangle::remove_group(MeshGroup *grp) {
  meshgroups.remove(grp);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// stuff used by MeshGroup

int operator==(const AMTriangle &t1, const AMTriangle &t2) {
  if(*t1.node[0] != *t2.node[0]) return 0;
  if(*t1.node[1] != *t2.node[1]) return 0;
  if(*t1.node[2] != *t2.node[2]) return 0;
  return 1;
}

int operator<(const AMTriangle &t1, const AMTriangle &t2) {
  if(*t1.node[0] < *t2.node[0]) return 1;
  if(*t1.node[0] > *t2.node[0]) return 0;
  if(*t1.node[1] < *t2.node[1]) return 1;
  if(*t1.node[1] > *t2.node[1]) return 0;
  if(*t1.node[2] < *t2.node[2]) return 1;
  return 0;
}

template <>
void MeshGroup::append(AMTriangle* const &tri) {
  append_base(tri);
  tri->add_group(this);
}

template <>
void MeshGroup::append(const Vec<AMTriangle*> &list) {
  append_base(list);
  for(int i=0; i<list.capacity(); i++)
    list[i]->add_group(this);
}

template <>
void MeshGroup::clear() {
  for(int i=0; i<members.capacity(); i++)
    members[i]->meshgroups.remove(this);
  clear_base();
}

template <>
void MeshGroup::remove(AMTriangle * const &tri) {
  tri->remove_group(this);
  remove_base(tri);
}


//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Vec<Cell_coordinate> AMTriangle::get_pixels() const {
  Vec<Cell_coordinate> list(0, BlockSize(10));
  for(AMTriangleIterator it(*this); !it.end(); ++it)
    list.grow(1, (*this)[it]);
  return list;
}

 int AMTriangle::npixels() const {
  int n = 0;
  for(AMTriangleIterator it(*this); !it.end(); ++it)
    n++;
  return n;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

AMTriangle *AMTriangle::copy(AdaptiveMesh *newmesh) const {
  AMTriangle *newtri = new AMTriangle;
  int i;

  for(i=0; i<2; i++) {
    if(child[i]) {
      newtri->child[i] = child[i]->copy(newmesh);
      newtri->child[i]->parent = newtri;
    }
    else {
      newtri->child[0] = 0;
      newtri->child[1] = 0;
    }
  }

  newtri->mesh = newmesh;
  newtri->generation = generation;
  newtri->index = index;
  newtri->swapped = swapped;
  newtri->status_ = 0;
  if(selected())
    newtri->select();
  newtri->parent = 0;		// will be set by parent
  newtri->representative_material = representative_material;
  //  newtri->material_cell = material_cell;
  newtri->inhibit_inheritance = inhibit_inheritance;
  newtri->representative_material_time = representative_material_time;
  newtri->group_cell = group_cell;
  newtri->group_cell_time = group_cell_time;
  newtri->meshgroups_time = meshgroups_time;
  newtri->inhibit_groupinheritance = inhibit_groupinheritance;
  newtri->lastE = lastE;
  newtri->currentE = currentE;
  newtri->Ecalculated = Ecalculated;
  newtri->lastEcalculated = lastEcalculated;
  
  for(i=0; i<3; i++)
    newtri->node[i] = newmesh->nodes[node[i]->index];

  if(!child[0])
    // Add triangle to list in nodes iff it has no children.
    for(i=0; i<3; i++)
      newtri->node[i]->add_triangle(newtri);

  // Can't set neighbor pointers here, because neighbor may not have
  // been copied yet.
  for(i=0; i<3; i++)
    newtri->neighbor[i] = 0;

  for(i=0; i<meshgroups.capacity(); i++) {
    MeshGroup *grp = newmesh->get_group(meshgroups[i]->query_name());
    //    newtri->meshgroups.grow(1, grp);
    if(grp)			// don't worry about unnamed groups
      grp->append(newtri);	// adds grp to newtri->meshgroups too
  }

  return newtri;
}

void AMTriangle::add_neighbor(int n0, int n1, AMTriangle *nbr) {
  // n0 and n1 are node indices. nbr is the neighbor opposite the OTHER node.
  for(int i=0; i<3; i++) {
    if(node[i]->index == n0) {
      if(node[(i+1)%3]->index == n1) {
#ifdef DEBUG
	if(neighbor[(i+2)%3] != 0)
	  cerr << "Error 1 in AMTriangle::add_neighbor()! "
		    << neighbor[(i+2)%3]
		    << endl;
#endif
	neighbor[(i+2)%3] = nbr;
      }
#ifndef DEBUG
      else {
#else
	else if(node[(i+2)%3]->index == n1) {
	  if(neighbor[(i+1)%3] != 0)
	    cerr << "Error 2 in AMTriangle::add_neighbor()! "
		 << neighbor[(i+1)%3]
		 << endl;
#endif
	  neighbor[(i+1)%3] = nbr;
	}
#ifdef DEBUG
      else
	cerr << "Error 3 in AMTriangle::add_neighbor()! " << endl;
#endif      
      return;
    }
  }
  cerr << "Error in AMTriangle::add_neighbor()!" << endl;
  exit(1);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// The Sun compiler seems to want these declared explicitly...
#if defined(sun) && !defined(__GNUG__)
template class LinkList<Cell_coordinate>::LinkListNode;
template class LinkList<AMTriangle*>::LinkListNode;
#endif
