// -*- C++ -*-
// $RCSfile: amtriangle.h,v $
// $Revision: 1.25 $
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

#ifndef AMTRIANGLE_H
#define AMTRIANGLE_H

class AMNode;
class AMTriangle;
class AMTriangleIterator;
class Elector;

#include "adaptmesh.h"
#include "color.h"
#include "meshcoord.h"
#include "meshgroups.h"
#include "timestamp.h"
#include "truefalse.h"
#include "vec.h"
#include <stdio.h>
#include <iostream.h>

class Cell_coordinate;
class Material;
class RubberBand;

enum TriangleDivisionMethod {TDM_NEWESTNODE, TDM_LONGESTEDGE, TDM_E};

class AMTriangle {
private:
  // node 0 is the newest node
  // node i is opposite neighbor i
  // nodes and edges are numbered going counterclockwise around the triangle
  AMNode *node[3];
  AMTriangle *neighbor[3];
  AMTriangle *parent;
  AMTriangle *child[2];
  AdaptiveMesh *mesh;
  int generation;
  int initial_generation;	// used to prevent too much subdivision
  int index;			// set and used during output
  bool swapped;
  Color color;			// set before drawing mesh

  unsigned char status_;
  static const unsigned char SELECTED;

  void mark_for_division();
  void start_division() { initial_generation = generation; }
  int division_depth() const { return generation - initial_generation; }

  void divide();
  AMTriangle *find_nbr_child(AMTriangle*, const AMNode*, const AMNode*);
  int neighbor_no(const AMNode*, const AMNode*) const;

  void listnbrs(ostream&) const; // debugging

  double E_tentative_divide(int) const;	// what E would be if divided thru edge

  bool replace_neighbor(AMTriangle *oldnbr, AMTriangle *newnbr);
  void add_neighbor(int, int, AMTriangle*); // used in AdaptiveMesh::copy()

  int divider() const;		// which node should be divided?
  int divided_node;		// which node was divided

  Vec<int> longestedges() const;
  Vec<int> smallest_E_nodes() const;	// edge(s) that minimizes E when divided

  const Cell_coordinate &closest_pixel(const Vec<Cell_coordinate>&) const;

  void add_group(MeshGroup*);
  void remove_group(MeshGroup*);

  AMTriangle *child_containing(const MeshCoord &);
  AMTriangle *child_containing(const Cell_coordinate &);

  Material *representative_material;
  //  Cell_coordinate material_cell; // cell containing representative material
  TimeStamp representative_material_time;
  void choose_center_material(); // get material from center pixel
  void elect_material(Elector&); // get material from most popular pixel
  void inherit_material(bool forced=0);	// get mat'l according to current method
  bool inhibit_inheritance;

  Cell_coordinate group_cell;	// cell containing most popular groups
  TimeStamp group_cell_time;

  Vec<MeshGroup*> meshgroups;	// list of groups to which this belongs
  TimeStamp meshgroups_time;

  TimeStamp lastmod_time;

  double lastE;
  double currentE;
  TimeStamp Ecalculated;
  TimeStamp lastEcalculated;
  
  void elect_groups(Elector&);
  void inherit_elected_groups(Elector&); // get groups from representative pixel
  void inherit_all_groups();	// get groups from all pixels
  void inherit_center_groups();	// get groups from center pixel
  void copy_groups(const Cell_coordinate&);
	// used by inherit_elected_groups & inherit_center_groups
  void inherit_groups(bool forced=0);// get groups according to current method
public:
  bool inhibit_groupinheritance;
private:

  void writegoof(FILE *file);
  void writeoof2(ostream &) const;

  void save(ostream&);// not const, because it calls inherit_material indirectly
  static AMTriangle *read(istream&, AdaptiveMesh*, AMTriangle*);

  AMTriangle(AMTriangle *parent,
	     AMNode *n0, AMNode *n1, AMNode *n2);
  AMTriangle() {}
  ~AMTriangle();
  AMTriangle *copy(AdaptiveMesh *) const;
public:
  void select() { status_ |= SELECTED; }
  void unselect() { status_ &= ~SELECTED; }
  void toggle_select() { status_ ^= SELECTED; }
  bool selected() const { return status_ & SELECTED; }
  int nnbrs_selected() const;	// how many neighbors are selected?

  bool active() const;		// are all the nodes in active pixels?

  MeshCoord center() const;
  int depth() const { return generation; }
  int npixels() const;
  Vec<Cell_coordinate> get_pixels() const;
  //  const Vec<Cell_coordinate> &get_pixels(); const { return pixels; }
  Cell_coordinate closest_pixel() const;
  int contains(const MeshCoord&) const;
  bool outside(const Rectangle&) const;
  AMTriangle *neighbor_towards(const MeshCoord&) const;
  Material *material();
  Material *resolve_material(); // returns defaultmaterial if necessary
  void set_material(Material*, bool);
  bool is_interface();
  bool is_double_interface();
  //  Cell_coordinate representative_material_cell();
  const Vec<MeshGroup*> &mesh_groups();
  void inhibit_group_inheritance() { inhibit_groupinheritance = true; }
  static int inhibittest(AMTriangle*&); // used in AdaptiveMesh::remove_group

  bool divided() const { return child[0] != 0; }

  // add other nodes other than the given node to the given list
  void list_other_nodes(const AMNode*, Vec<AMNode*> &others) const;

  // area of cell/triangle intersection
  double intersection(const Cell_coordinate &) const;
  double area() const;
  double E();			// function to be minimized by fiddling nodes
  double Ehomogeneity();
  double Eangle() const;
  void revertE();

  // for looping over pixels under a triangle
  Cell_coordinate operator[](AMTriangleIterator&) const;

  static Enum<TriangleDivisionMethod> divisionmethod;
  static void init();		// call once to set up Enum

  void draw(ImageCanvas &) const;
  void draw_selected(ImageCanvas&) const;
  //  void draw_material(ImageCanvas&, const Image&, const Color&);
  void fill(ImageCanvas&) const;// fill with triangle's color
  void outline(ImageCanvas&, const Color&) const;
  void outline_ps(FILE*, const Color&, const Rectangle&) const;

  void fill_ps(FILE*, const Rectangle&) const;
  void draw_ps(FILE*, const Rectangle&) const;

  friend int operator==(const AMTriangle&, const AMTriangle&);
  friend int operator<(const AMTriangle&, const AMTriangle&);
  friend ostream &operator<<(ostream &, const AMTriangle&);

#ifdef DEBUG
  void dump_intersections() const;
#endif // DEBUG

  friend class AdaptiveMesh;
  friend class AMIterator;
  friend class AMTriangleIterator;
  friend class AMNode;
  friend class Group<AMTriangle*>;
};


class AMNode {
private:
  int index;
  unsigned char edgeflags;
  MeshCoord coord_;
  TimeStamp nodemoved;		// when node was moved 
  MeshCoord lastcoord;
  TimeStamp lastmoved;

  AMNode(AdaptiveMesh*, const AMNode*, const AMNode*);
  AMNode(AdaptiveMesh*, double, double, unsigned char);
  AMNode() {};
  AMNode *copy() const;
  // bits set in edgeflags
  static const unsigned char top_;
  static const unsigned char btm_;
  static const unsigned char lft_;
  static const unsigned char rgt_;
  int top() const { return edgeflags & top_; }
  int btm() const { return edgeflags & btm_; }
  int lft() const { return edgeflags & lft_; }
  int rgt() const { return edgeflags & rgt_; }

  // Maintain list of triangles that have this node as a corner
  Vec<AMTriangle*> triangle;
  void add_triangle(AMTriangle *t) { triangle.grow(1, t); }
  void remove_triangle(AMTriangle *t) { triangle.remove(t); }
  int areas_ok(double minarea=0.0) const; // are all areas positive?

  int MCmove(double, double, double*);
  int Laplacemove(double*);

  void save(ostream&) const;
  static void read(AdaptiveMesh*, istream&);

public:
  double E() const;		// sum of E for each triangle
  double Eangle() const;
  double Ehomogeneity() const;
  void revert();		// revert node position and all triangles' E
  void move_to(const MeshCoord&);
  void move_by(const MeshCoord&);
  void move_back();
  const MeshCoord &coord() const { return coord_; }

  bool active() const;		// is the node in an active pixel?

  friend class AMTriangle;
  friend class AMTriangleIterator;
  friend class AdaptiveMesh;
  friend class NodeRubberBand;
  friend int operator!=(const AMNode&, const AMNode&);
  friend int operator<(const AMNode&, const AMNode&);
  friend int operator>(const AMNode&, const AMNode&);
  friend ostream &operator<<(ostream &, const AMNode&);
};



#endif
