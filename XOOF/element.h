// -*- C++ -*-
// $RCSfile: element.h,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2004-10-19 02:24:35 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// element types
 
#ifndef ELEMENT_H
#define ELEMENT_H

class Element;

#include "debug.h"
#include "menuDef.h"
#include "mvmd.h"
#include "mvvd.h"
#include "sparselink.h"
#include "strainstress.h"
#include "truefalse.h"
#include "vec.h"
#include <iostream.h>
#include <stdio.h>
#include <vector>

class BP_block;
class ElementGroup;
class EulerAngle;
class Grid;
class MeshCoord;
class Node;
class Rectangle;
class SymmMatrix;

// base class for elements

class Element {
private:
  Element(const Element&); // prohibited!
  Element &operator=(const Element&); // prohibited
    	
  int selected;
protected:
  TrueFalse current_stress;
  StrainStress stress;
  TrueFalse current_elastic_strain;
  StrainStress elastic_strain;
#ifdef THERMAL
  TrueFalse current_grad_thermal;
  MV_Vector_double Grad_T_field;
  TrueFalse current_J;
  MV_Vector_double J_field;
#endif
public:
  /* these would be private or protected if
   * friend templates were allowed.
   */
  int index;			// unique among all elements
  Vec<Node*> corner;		// pointers to corners
  Vec<ElementGroup*> groups;

public:	// would be protected if friend templates were allowed
  static float gray_dflt;
  static int index_dflt;
  static Vec<int> nodenumber_dflt;
  static Element *binaryread(FILE*, TrueFalse&);
  static unsigned char inputformatflag;
  static int cloneindex;	// element to copy stiffness from
public:
  // constructors
  Element() {}
  Element(Grid*);
  virtual ~Element();
	
  // for identification purposes only
  virtual const CharString &tag() const = 0;
	
  // for construction of initialization menus in ElementCmd
  static void AddArgs(CommandM *cmd) {
    AddArgument(cmd, "gray", gray_dflt);
  }
  // called before create_element during execution of ElementCmd
  virtual void OptionalArgHandler() {}

  int isselected() const { return selected; }
  void select() { selected = 1; }
  void unselect() { selected = 0; }
  void toggleselect() { if(selected) unselect(); else select(); }
	
  virtual int contains(const MeshCoord&) const = 0; // is a point inside?
  virtual bool outside(const Rectangle&) const = 0; // is this outside rect?
  virtual MeshCoord center(double enhancement=1.0) const = 0;
  int hashfunc(const MeshCoord&, double&);
	
  virtual void newcoords() = 0; // when coord transformations change

  virtual void setstiffness() {}
#ifdef THERMAL
  virtual void setstiffnessT() {}
#endif // THERMAL
  virtual CharString parameters() const { return CharString();}
  virtual void printstiffness(ostream&) const {}
  virtual void binarywrite(FILE*, char) const;
  static void reset_defaults();	// resets default values for binaryread

  virtual const StrainStress &elastic_straintensor() = 0; // 3D
  virtual StrainStress total_straintensor() = 0; // 3D
  virtual const StrainStress &stresstensor() = 0;
  virtual double original_area() const = 0;
  virtual double current_area() const = 0;
  virtual EulerAngle query_orientation() const;
  double elastic_energy();
  double new_elastic_energy();
  virtual MeshCoord force(const Node*) const = 0;
#ifdef THERMAL
  virtual const MV_Vector_double &gradient_temperature_field() = 0;
  virtual const MV_Vector_double &heat_flux_field() = 0;
  double thermal_energy();
  virtual double heat(const Node*) const = 0;
#endif // THERMAL
  virtual double mutation_status() { return 0.0; } // for graphical output

#ifdef THERMAL
  // These need to be defined so that the classes in stressdrawer.C
  // can use them.  It's ugly having to define them here, instead of
  // only in TriElementT where they belong.
  virtual double average_T() const { return 0.0; }
  virtual double deltaT() const { return 0.0; }
#endif // THERMAL
	
  // for matrix operations w/out constructing global stiffness
  virtual void Ku_mult(const MV_Vector_double&, MV_Vector_double&) = 0;
  virtual void Ku_mult(const MV_Vector_double&, MV_Vector_double&,
		       const int, const int) = 0;
#ifndef THERMAL
  virtual void interiordiagonals(MV_Vector_double&) const = 0;
  virtual void interiorblocks(Vec<BP_block*>) const = 0;
#endif // !THERMAL
	
  // for constructing global stiffness matrices
  virtual void loadinterior(SparseLinkMat&) const = 0;
  virtual void loadboundary(SparseLinkMat&,
			    SparseLinkMat&, SparseLinkMat&) const = 0;
#ifdef THERMAL
  virtual void loadinteriorT(SparseLinkMat&) const = 0;
  virtual void loadboundaryT(SparseLinkMat&,
			    SparseLinkMat&, SparseLinkMat&) const = 0;
#endif // THERMAL
  // for constructing internal force vector
  virtual void loadinterior_rhs(MV_Vector_double&) const {};
  virtual void loadboundary_rhs(MV_Vector_double&) const {};

  virtual int mutate() { return 0; }   // change properties
  virtual void revert() {};

  virtual bool isbdy() const = 0;	// does this have any bdy nodes?
  virtual bool isempty() const { return false; }
#ifdef THERMAL
  virtual bool isempty_phi() const { return false; }
#endif // THERMAL
	
  double datum;	// stores value being plotted. kludgy.
  float intrinsic_gray;
  void not_current();

  // for sorting lists of elements by index
  static int compare(const void *, const void *);

  // Stuff added for oof2oof2.
  virtual bool defines_planestrain() const { return false; }
  virtual bool get_planestrain() const { return false; }
#ifdef THERMAL
  virtual bool defines_Tplanarity() const { return false; }
  virtual bool get_Tplanarity() const { return false; }
#endif // THERMAL

  // Stuff for oof2oof2
  void print_material(ostream &, const CharString&) const;
  virtual std::vector<CharString> *print_properties(ostream&) const;
  void oof2write(ostream &) const;
  virtual bool same_type(const Element*) const;	// used by operator==
  friend bool operator==(const Element&, const Element&);
  
  friend class Debug;
  friend ostream &operator<<(ostream&, const Element&);
};

CharString newPropertyName();
#endif


