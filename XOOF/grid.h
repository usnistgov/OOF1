// -*- C++ -*-
// $RCSfile: grid.h,v $
// $Revision: 1.18 $
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


// grid --

class Grid;

#ifndef GRID_H
#define GRID_H

#include "debug.h"
#include "element.h"
#include "freedom.h"
#include "hashmesh.h"
#include "iterator.h"
#include "menuDef.h"
#include "comprow_double.h"
#include "mvmd.h"
#include "mvvd.h"
#include "sparselink.h"
#include "timestamp.h"
#include "truefalse.h"
#include "vec.h"

#include <stdio.h>

//class CmdList;
class DegreeOfFreedom;
class ElementFunctionBase;
class ElementGroup;
class FileName;
class LinePlot;
class MeshCoord;
class Node;
class NodeGroup;
class Rectangle;
class Shape;
class Stiffness;
class StressStrainStat;
class TwoTwo;
class ThermalDistortion;

class DoFset {			// set of degrees of freedom
private:
  DoFset();
  ~DoFset();
  Vec<DegreeOfFreedom*> free_dof; // free degrees of freedom
  Vec<DegreeOfFreedom*> fixed_dof; // fixed degrees of freedom
  Vec<DegreeOfFreedom*> inactive_dof; // enslaved or marooned dofs.
  DegreeOfFreedom *AddDoF(DegreeOfFreedom *dof); // add to free_dof list
  void shufflelists();
  void shuffle_dof(const Vec<DegreeOfFreedom*> &doflist);
  void clear_fixed_flags(const Vec<DegreeOfFreedom*> &doflist);
  void set_fixed_flags(const Vec<DegreeOfFreedom*> &doflist);
  void fix_masters();
#ifdef DEBUG
  void dump(ostream&) const;
#endif
  friend class Grid;
  friend class NodeGroup;
  friend class Debug;
public:
  void initialize(int);
  int nfixed() const { return fixed_dof.capacity(); }
  int nfree() const { return free_dof.capacity(); }
};

class Grid {
private:
  SparseLinkMatrix<bool> connectivity; // are nodes physically connected?
  SparseLinkMatrix<bool> connectivity_phi; // are they thermally connected?
  SparseLinkMatrix<bool> fullconnectivity; // includes empty elements...
  CompRow_Mat_double Kii;
  CompRow_Mat_double Kib;
  CompRow_Mat_double Kbb;
  // methods for mucking with the lists, used by shufflelists()
  void move_elem(const Vec<Element*> &);
  void fix_disconnected(); 
  // coeffs of stuff linear in u_ij in E (internal forces)
  MV_Vector_double Li();	// interior nodes
  MV_Vector_double Lb();	// boundary nodes
  HashMesh<Node> nodehash;    // hash tables for quick lookup
  HashMesh<Element> elemhash;
  double enhancement_;		// multiplier for displacements in display
  void rehash();
  int hash_needed;
  Grid(const Grid&);		// prohibited
  Grid &operator=(const Grid&); // prohibited
  bool fix_masters_elastic_flag;
#ifdef THERMAL
  bool fix_masters_thermal_flag;
#endif // THERMAL

  // support functions for oof2 output
  void find_material_groups(Vec<ElementGroup*>&) const;
  bool check_plane_strain(bool&) const;
  void print_nodes(ostream&, const CharString&) const;
  void print_elements(ostream&, const CharString&) const;
  void print_nodegroups(ostream&, const CharString&) const;
  void print_elementgroups(ostream&, const CharString&,
			   const Vec<ElementGroup*>&) const;
  void print_bcs(ostream&, const CharString&) const;
  void print_fields(ostream&, const CharString&) const;
#ifdef THERMAL
  bool check_Tplanarity(bool&) const;
#endif // THERMAL

public:
  Vec<Element*> interiorE;    // interior elements (w/out boundary nodes)
  Vec<Element*> boundaryE;    // elements with at least one fixed d.of.f
  Vec<Node*> node;	    // nodes 
  Vec<NodeGroup*> nodegroup;
  Vec<ElementGroup*> elementgroup;
  DoFset elastic_dof;
  //  CmdList *bccmds;
  //  CmdList *distortcmds;

  Grid();
  Grid(FILE *);
  ~Grid();
	
  void redraw();
	
  // routines for constructing grid
  void setNnodes(int);		// just allocates space
  void setNelements(int);	// just allocates space
  int AddElement(Element *elem);
  void AddNode(int index, Node *newnode); // add to node list
  DegreeOfFreedom *AddDoFelastic(DegreeOfFreedom *dof) {
    return elastic_dof.AddDoF(dof);
  }
  NodeGroup *getgroup(const CharString &, const TrueFalse&);
  ElementGroup *getEgroup(const CharString &, const TrueFalse&);
  void showgroups() const;
  void showEgroups() const;
  void make_connectivity();
  bool connected(int);		// is a node connected?
  void reset_connectivity();
#ifdef THERMAL
  bool connected_phi(int);
  void make_connectivity_phi();
#endif // THERMAL

  // real work
  void make_stiffness();
  // tell elements that there's been a coord transformation
  void newcoords();
  // find equilibrium displacements for free dof's.
  int equilibrate(int);		// returns 0 if doesn't converge
  // change mutable elements
  int mutate();
  void complete_equilibrate(int use_global);
  // put elements and dof's in correct lists
  void shufflelists_elastic();
  // put elements back into the interiorE list, in index order
  void unshufflelists();
  // set the "fixed" flags in master degrees of freedom
  void fix_masters();
  void fix_masters_needed();
  void fix_masters_elastic();
  void fix_masters_elastic_needed()  { fix_masters_elastic_flag = true; }
  // emancipate all degrees of freedom, but don't emancipate groups
  void emancipate_all_elastic_dofs() const;
  // enslave all dofs in node groups marked ENSLAVED
  void enslave_marked_elastic_dofs() const;
  // don't mark any groups ENSLAVED
  void emancipate_all_groups() const;

  // make all the elements recalculate their stresses next time 'round
  void needs_recomputed();
  
  // remove FIXED flag from all node groups
  void freeall_nodes_x() const;
  void freeall_nodes_y() const;

  void apply_absolute_elastic_distortion();
  void apply_incremental_elastic_distortion();
  void clear_elastic_distortion();

  void reset();			// clear displacements and forces	

  double energy();
  MeshCoord force(const NodeGroup *); // total force on a group
  MeshCoord avgdisp(const NodeGroup *); // avg displacement of group
  MeshCoord avgposition(const NodeGroup*); // avg position of group
  TimeStamp elements_changed;	// when elements were added or replaced
  TimeStamp node_elists_updated; // when lists of elements in nodes were made
  void update_node_elists();	// make lists of elements in nodes

  double flux(const NodeGroup&, double (*)(Element*, const MeshCoord&));

#ifdef THERMAL
  DoFset thermal_dof;
  DegreeOfFreedom *AddDoFthermal(DegreeOfFreedom *dof) { // add to free_dof list
    return thermal_dof.AddDoF(dof);
  }
  void make_stiffnessT();
  void fix_masters_thermal();
  void fix_masters_thermal_needed() { fix_masters_thermal_flag = true; }
  void shufflelists_thermal();
  void emancipate_all_thermal_dofs() const;
  void enslave_marked_thermal_dofs() const;
  int thermal_equilibrate();
  int equilibrate_all(int);
  void freeall_nodes_phi() const;
  void apply_incremental_thermal_distortion();
  void apply_absolute_thermal_distortion();
  void clear_thermal_distortion();
  double thermal_energy();
  double avgtemp(const NodeGroup *) const;
  double heatrate(const NodeGroup*) const;
#endif // THERMAL

  void asciioutput(ostream &);
  void oof2oof2(ostream&, const CharString&, int, int) const;
  void postscript(FILE *psfile);
	
  // access to node list for printing, debugging, etc.
  int Nnodes() const { return node.capacity(); }
  inline Node *getnode(int i) { return node[i]; }

  Rectangle query_extreme_pos(void) const;

  // for mouse selection...
  // find the node nearest the given point
  Node *findNode(const MeshCoord &where);
  void unselectallnodes() const;
  void selectallnodes() const;
  // find the element containing the given point
  Element *findElement(const MeshCoord &where);
  void unselectallelements() const;
  void selectallelements() const;
  void selectelement(int) const;
  void toggleselect() const;
  void selectrandomelements(double) const;
  void selectshape(const Shape&) const;
  void selectshape_nodes(const Shape&) const;
  StressStrainStat stress_statistics(Iterator<Element> &itel) const;
  StressStrainStat strain_statistics(Iterator<Element> &itel) const;
	
  void require_rehash() { hash_needed = 1; }

  double enhancement() const { return enhancement_; }
  void set_enhancement(double);
  // this is mixing the Grid and the interface classes a bit too much....
  static double enhancement_dflt;
  static void set_enhancement_cmd(const CharString&);

  void lineplot(const FileName&, const LinePlot&, const ElementFunctionBase&);

  friend class Stiffness;
  friend class Diagonal_Precond;
  friend class Block_Precond;
  friend MV_Vector_double operator*(const Stiffness&, const MV_Vector_double &);
  friend class Debug;

#ifdef DEBUG
  void printKii(ostream &os);
  void printKib(ostream &os);
  void printKbb(ostream &os);
  void printKlocal(ostream &os);
  void nodegroupdump(ostream&) const;
  void nodedump(ostream&) const;
  void dofdump(ostream&) const;
#endif
};


#endif


