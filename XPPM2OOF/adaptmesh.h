// -*- C++ -*-
// $RCSfile: adaptmesh.h,v $
// $Revision: 1.19 $
// $Author: langer $
// $Date: 2004-11-02 22:36:05 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Adaptive mesh using newest node bisection

#ifndef ADAPTMESH_H
#define ADAPTMESH_H

class AMTriangle;
class AMNode;
class AMIterator;
class RefinementCondition;

#include "enum.h"
#include "meshgroups.h"
#include "timestamp.h"
#include "truefalse.h"
#include "vec.h"
#include <stdio.h>

class Cell_coordinate;
class CharString;
class Color;
class FileName;
class Elector;
class Goof;
class Image;
class ImageCanvas;
class MeshCoord;
class PixelGroup;
class RefineCmd;

class AdaptiveMesh {
private:
  int nroots;
  Vec<AMTriangle*> root;
  Goof *goof;
  Vec<AMNode*> nodes;
  AdaptiveMesh(const AdaptiveMesh&) {}; // disable implicit copying
  int pixels_listed;
  int depth;
  bool periodic_x;
  bool periodic_y;
  void set_triangle_indices() const;
  void set_neighbor_pointers();
  Vec<AMTriangle*> dividelist;	// triangles scheduled for division
  void refine_dividelist();
  // Stuff for oof2 output
  void find_material_groups(Vec<MeshGroup*>&, const Vec<PixelGroup*>&) const;
  void print_nodes(ostream&, const CharString&) const;
  void print_elements(ostream &, const CharString&) const;
  void print_nodegroups(ostream&, const CharString&) const;
  void print_elementgroups(ostream&, const CharString&, const Vec<MeshGroup*>&)
    const;
public:
  AdaptiveMesh(Goof*, int, int);
  AdaptiveMesh();
  ~AdaptiveMesh();
  AdaptiveMesh *copy() const;	// allow expclit copying only

  AMTriangle *smallest_triangle_containing(const Cell_coordinate&,
					   AMTriangle* guess=0) const;
  AMTriangle *smallest_triangle_containing(const MeshCoord&,
					   AMTriangle* guess=0) const;
  AMNode *closest_node(const MeshCoord&) const;

  AMTriangle *operator[](const AMIterator&) const;

  void refine(int iterations, const RefinementCondition&);

  void refine_group(int, MeshGroup*);
  static double min_area;	// don't refine triangles smaller than this
  static int max_divisions;	// don't do more divisions than this at one go

  double E();
  void Erange(double&, double&);
  void fiddleMC(double T, double delta, const Vec<AMNode*>&);
                            // do one MC step with temperature T
  int fiddleDownhill(double delta); // move nodes downhill in energy
  void fiddleLaplace(const Vec<AMNode*>&); // move nodes to avg of nbr positions
  Vec<AMNode*> activenodes() const;
  int areas_ok(double minarea=0.0) const; // are all triangle areas positive?
  static double alpha;

  int swap_edges(AMTriangle*, AMTriangle*, AMTriangle*&, AMTriangle *&);
  void unswap_edges(AMTriangle*, AMTriangle*, AMTriangle*, AMTriangle *);
  void swap_worst();
  void test_swap();

  int move_node(const MeshCoord &from, const MeshCoord &to);

  int maxdepth() const { return depth; }
  int ntriangles() const;
  int nnodes() const { return nodes.capacity(); }

  void draw(ImageCanvas&, const Color&, int width) const;
  void draw_selected(ImageCanvas&, const Color&, const Color&) const;
  void draw_material(ImageCanvas&) const;

  void draw_ps(FILE*, const Color&, int width, const Rectangle&) const;
  void draw_material_ps(FILE*, const Rectangle&) const;
  void draw_selected_ps(FILE*, const Color&, const Color&, const Rectangle&)
    const;

  // set the color of each triangle
  void setcolors(const Color &dflt_color);
  void setselectedcolors(const Color&);
  void getpixels(int, Colormap) const; // get X pixel values for triangle colors

  void select_all();
  void unselect_all();
  void select(const MeshGroup&);
  void select_too(const MeshGroup&);
  int unselect_sorethumbs();
  int select_dimples();
  int selectE(double, double);
  int select_interface();
  int select_neighbors();

  // When the rules for transferring materials from pixels to mesh are
  // changed, the material_rules_changed timestamp is updated. If the
  // transfer method is "continuous", then the
  // material_recompute_requested timestamp is updated. If the update
  // method is "explicit", then the recompute_requested timestamp is
  // updated later (but only if material_rules_changed is later than
  // materials_recompute_performed.
  TimeStamp material_rules_changed;
  TimeStamp material_recompute_requested;
  TimeStamp material_recompute_performed;
  void materials_need_recomputing();
  void material_timechange();	// called when transfertime is changed
  void assign_material_selected(Material*);

  TimeStamp groups_rules_changed;
  TimeStamp groups_recompute_requested;
  TimeStamp groups_recompute_performed;
  void groups_need_recomputing();
  void groups_timechange();

  bool prepare_output();
  void writegoof(const FileName &);
  void write_oof2(ostream&, const CharString&);
  void printgoofheader(FILE *);

  MeshCoord cellcenter(const Cell_coordinate&) const;
  //  Cell_coordinate mesh2cell(const MeshCoord&) const;

  Vec<MeshGroup*> grouplist;
  MeshGroup *add_group(const CharString &name);
  MeshGroup *get_group(const CharString &name) const;
  MeshGroup *find_group(const CharString &name);
  void remove_group(MeshGroup*, bool);
  void remove_all_groups(bool);

  void inherit_pixel_groups(bool forced=0);
  void inherit_pixel_materials(bool forced=0);

  void remove_periodic_x();
  void remove_periodic_y();

  void save(ostream&) const; // for writing config files
  void load(istream&);

  static TrueFalse continuous_redraw;	// redraw continuously?

  void sanity() const;		// perform sanity check

  friend class AMNode;
  friend class AMTriangle;
  friend class AMTriangleIterator;
  friend class AMIterator;
};

extern TrueFalse meshvisible;
extern TrueFalse meshselectvisible;

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Mesh Refinement classes

class RefinementCondition {
public:
  virtual int setup() { return 1; } // initiialization, return 1 if ok.
  virtual int operator()(AMTriangle*) const = 0; // divide this triangle?
  virtual ~RefinementCondition() {}
  virtual void AddArgs(CommandM*) {}; // provide arguments to command
  virtual const CharString name() const = 0; // name of this refinement method
  virtual RefinementCondition *clone() const = 0; // duplicate this object
};

class RefineAll : public RefinementCondition {
public:
  virtual int operator()(AMTriangle*) const { return 1; }
  virtual const CharString name() const { return CharString("all"); }
  virtual RefinementCondition *clone() const { return new RefineAll; }
};

class RefineE : public RefinementCondition {
private:
  double threshold;
public:
  RefineE() : threshold(.1) {}
  virtual int operator()(AMTriangle*) const;
  virtual void AddArgs(CommandM *cmd) {
    AddArgument(cmd, "threshold", threshold);
  };
  virtual const CharString name() const { return CharString("E"); }
  virtual RefinementCondition *clone() const;
};

class RefineSelected : public RefinementCondition {
  virtual int operator()(AMTriangle*) const;
  virtual const CharString name() const { return CharString("selection"); }
  virtual RefinementCondition *clone() const { return new RefineSelected; }
};

class RefineDepth : public RefinementCondition {
private:
  int maxdepth;
  int mindepth;
public:
  RefineDepth() : mindepth(0), maxdepth(0) {}
  virtual int operator()(AMTriangle*) const;
  virtual void AddArgs(CommandM *cmd) {
    AddArgument(cmd, "min", mindepth);
    AddArgument(cmd, "max", maxdepth);
  }
  virtual const CharString name() const { return CharString("depth"); }
  virtual RefinementCondition *clone() const;
};

class RefineInterface : public RefinementCondition {
public:
  virtual int operator()(AMTriangle*) const;
  virtual const CharString name() const { return CharString("interface"); }
  virtual RefinementCondition *clone() const { return new RefineInterface; }
};

class RefineDoubleInterface : public RefinementCondition {
public:
  virtual int operator()(AMTriangle*) const;
  virtual const CharString name() const { return CharString("bumps"); }
  virtual RefinementCondition *clone() const { return new RefineDoubleInterface; }
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Iterator for adaptive mesh class
// Usage:
//  AdaptiveMesh m;
//  for(AMIterator i(m, AMI_ALL); !i.end(); ++i)
//     AMTriangle *t = m[i];
// AMI_ALL means iterator returns all childless triangles.
// AMI_SELECTED means iterator returns only selected triangles.
// AMI_UNSELECTED means what you think it does.
// Triangles with children are never returned.

enum AMIteratorType {AMI_ALL, AMI_SELECTED, AMI_UNSELECTED};

class AMIterator {
private:
  const AdaptiveMesh *mesh;
  AMIteratorType type;
  int rootnumber;
  int finished;
  AMTriangle *triangle;
  void next();
public:
  AMIterator(const AdaptiveMesh *, AMIteratorType);
  void operator++();
  int end() const { return finished; }
  friend class AdaptiveMesh;
};

#endif
