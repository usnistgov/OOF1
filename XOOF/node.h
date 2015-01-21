// -*- C++ -*-
// $RCSfile: node.h,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-12-15 21:57:53 $


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

#ifndef THERMAL
class BP_block;
#endif // !THERMAL

#include "meshcoord.h"
#include "mvvd.h"
#include "mvmd.h"
#include "mvblasd.h"
#include "twotwo.h"
#include "typeregistry.h"
#include "vec.h"

class DegreeOfFreedom;
class ThermalDistortion;

// base class
class Node {
private:
  Node(const Node&);		// prohibited
  Node &operator=(const Node&);	// prohibited
  Vec<Element*> element;
protected:
  int selected;

public:
  DegreeOfFreedom *x;
  DegreeOfFreedom *y;
#ifdef THERMAL
  DegreeOfFreedom *phi;//one degree of freedom per variable to solve
#endif
  int index;
	
  Node();
  Node(int n, DegreeOfFreedom *xx, DegreeOfFreedom *yy
#ifdef THERMAL
       , DegreeOfFreedom *phiphi
#endif // THERMAL
       );
  virtual ~Node() {}

  virtual const CharString &tag() const = 0;
	
  // get actual position
  virtual MeshCoord position(double enhancement=1.0)  const {
    return original() + enhancement*displacement();
  }
  // get displacement from original position
  virtual MeshCoord displacement() const = 0;
  // get original position
  virtual MeshCoord original() const = 0;
  // increment displacement
  virtual void move(const MeshCoord&) = 0;
  // clear displacement
  void reset_displacement(const int do_x, const int do_y);
  // apply linear distortion
  virtual void distort(const TwoTwo&, const MeshCoord&, 
		       const int, const int) = 0;
  // get force on all degrees of freedom
  virtual MeshCoord force() const;
  // apply force to unfixed degree of freedom
  virtual void applyforce(const MeshCoord &) = 0;
  void reset_force();
#ifdef THERMAL
  double voltage() const;	// analogous to displacement()
  virtual void v_move(const double&); // analogous to move()
  void reset_voltage();		// analogous ot reset_displacement()
  virtual void v_distort(const ThermalDistortion&); // analogous to distort()
  virtual double surf_charge() const; // analogous to force()
  virtual void apply_surfcharge(const double &); // anal. to applyforce()
  void reset_surfcharge();	// analogous ot reset_force()
#endif

  /* transformation matrix converts displacement
   * from internal coordinates to Cartesian coordinates
   */
  virtual TwoTwo *transformation() = 0;
  virtual int trivialtransform() const = 0;
	
  int fixed() const;

#ifndef THERMAL	
  BP_block *bpblock(); // block preconditioner setup
#endif // !THERMAL

  // set boundary condition flags
  void fix_x();
  void fix_y();
  void free_x();
  void free_y();
#ifdef THERMAL
  void fix_phi();
  void free_phi();
#endif // THERMAL
  // additional parameters needed to define node
  virtual CharString parameters() const { return CharString(); }
  CharString flags() const;
	
  int isselected() const { return selected; }
  void select() { selected = 1; }
  void unselect() { selected = 0; }
  void toggleselect() { if(selected) unselect(); else select(); }
  int hashfunc(const MeshCoord&, double&);

  bool disconnected() const;
  Element *neighboring(const Node*) const;

  virtual void binarywrite(FILE*) = 0;

#ifdef DEBUG
  void dump(ostream&) const;
#endif

  friend class IsoElement;
  friend class Element;
  friend ostream& operator<<(ostream&, const Node&);
  friend class Grid;
};


class XYNode : public Node {
private:
  static CharString nameplate;
  virtual const CharString &tag() const { return nameplate; }
  static int binaryread(FILE*);
  static NodeTypeRegistration reg;
public:
  XYNode() {}
#ifndef THERMAL
  XYNode(int n, DegreeOfFreedom *xx, DegreeOfFreedom *yy)
    : Node(n, xx, yy) {}
#else // THERMAL
  XYNode(int n, DegreeOfFreedom *xx, DegreeOfFreedom *yy,
	 DegreeOfFreedom *phiphi)
    : Node(n, xx, yy, phiphi) {}
#endif // THERMAL

  static XYNode *create(const int index, double x, double y,
			double dx, double dy
#ifdef THERMAL
			, double dphi
#endif // THERMAL
			);

  // used in binaryread and XYNodeCmd

  virtual MeshCoord original() const;
  virtual MeshCoord displacement() const;
  virtual void distort(const TwoTwo&, const MeshCoord&,
		       const int, const int);
  virtual void applyforce(const MeshCoord &);
  virtual TwoTwo *transformation() { return &I2x2;}
  virtual int trivialtransform() const { return 1; }

  virtual void move(const MeshCoord &d);
  virtual void binarywrite(FILE*);
};

/* LinearNodes have two degrees of freedom at arbitrary angles.
 * Only the displacements are measured at an angle -- the original position
 * is cartesian.
 */

class LinearNode : public Node {
private:
  // T transforms from internal coordinates to Cartesian coordinates.
  TwoTwo T;
  static CharString nameplate;
  virtual const CharString &tag() const { return nameplate; }
  static int binaryread(FILE*);
  static NodeTypeRegistration reg;
public:
  LinearNode() {}
  LinearNode(const int n, DegreeOfFreedom *x, DegreeOfFreedom *y,
#ifdef THERMAL
	     DegreeOfFreedom *phiphi,
#endif
	     const TwoTwo *tr = 0);
   static LinearNode *create(const int index,
		     double x, double y,
		     double dx, double dy,
#ifdef THERMAL
			     double dphi,
#endif
			     const TwoTwo &T);
	
  virtual MeshCoord original() const;
  virtual MeshCoord displacement() const;
  virtual TwoTwo *transformation() { return &T;}
  virtual int trivialtransform() const { return 0; }
	
  virtual void move(const MeshCoord &);
  virtual MeshCoord force() const;
  virtual void distort(const TwoTwo&, const MeshCoord&,
		       const int, const int);
  virtual void applyforce(const MeshCoord &);
  virtual CharString parameters() const;
  virtual void binarywrite(FILE*);
};

#endif

