// -*- C++ -*-
// $RCSfile: trielement.h,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2004-10-14 21:32:04 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Base class for triangular linear elements

#ifndef TRIELEMENT_H
#define TRIELEMENT_H

#include "element.h"
#include "symmmatrix.h"
#include "strainstress.h"
#include "meshcoord.h"

class TriElement : public Element {
public:
  TriElement() {}
  TriElement(Grid*);
  virtual ~TriElement() {}
	
  virtual const CharString &tag() const = 0;
	
  static void AddArgs(CommandM *cmd) {
    Element::AddArgs(cmd);
  }

  virtual void binarywrite(FILE*, char) const;
  virtual CharString parameters() const { return Element::parameters(); }
  virtual void printstiffness(ostream&) const;

  virtual const StrainStress &elastic_straintensor();
  virtual StrainStress total_straintensor() = 0;
  virtual const StrainStress &stresstensor() = 0;
  virtual MeshCoord force(const Node*) const = 0;
  virtual double original_area() const;
  virtual double current_area() const;
#ifdef THERMAL
  virtual const MV_Vector_double &gradient_temperature_field();
  virtual const MV_Vector_double &heat_flux_field() = 0;
  virtual double heat(const Node*) const;
#endif // THERMAL
  virtual int contains(const MeshCoord&) const; // is a pt inside?
  virtual bool outside(const Rectangle&) const;
  virtual MeshCoord center(double enhancement) const;

  virtual bool isbdy() const;  // is this a boundary element?
  virtual void newcoords();   // recompute stiffness

  // for computing matrix products w/out using global stiffness
  virtual void Ku_mult(const MV_Vector_double&, MV_Vector_double&);
  virtual void Ku_mult(const MV_Vector_double&, MV_Vector_double&,
		       const int, const int);
#ifndef THERMAL
  virtual void interiordiagonals(MV_Vector_double&) const;
  virtual void interiorblocks(Vec<BP_block*>) const;
#endif // !THERMAL

  // for constructing global stiffness matrices
  virtual void loadinterior(SparseLinkMat&) const;
  virtual void loadboundary(SparseLinkMat&,
			    SparseLinkMat&, SparseLinkMat&) const;
#ifdef THERMAL
  virtual void loadinteriorT(SparseLinkMat&) const;
  virtual void loadboundaryT(SparseLinkMat&,
			    SparseLinkMat&, SparseLinkMat&) const;
#endif // THERMAL
  // for constructing internal force vector
  virtual void loadinterior_rhs(MV_Vector_double&) const = 0;
  virtual void loadboundary_rhs(MV_Vector_double&) const = 0;

  virtual bool isempty() const { return false; }

protected:
  SymmMatrix K;
  static SymmMatrix K_dflt;
  MV_Vector_double thermal_forces;
  static MV_Vector_double thermal_dflt;
  virtual void setstiffness() = 0;
#ifdef THERMAL
  SymmMatrix K_t; //3x3
  static SymmMatrix K_t_dflt;
  virtual void setstiffnessT() = 0;
#endif // THERMAL
  MV_ColMat_double transformation();
  static Element *binaryread(FILE*, TrueFalse&);
  virtual std::vector<CharString> *print_properties(ostream &os) const {
    return Element::print_properties(os);
  }

  friend class Debug;
  friend class Bmatrix;
  friend class SideList;
#ifdef THERMAL
  friend class Gvector;
#endif // THERMAL
};

class SideList {
private:
  MeshCoord side[3];
public:
  SideList(const TriElement*);
  double area;
  const MeshCoord &operator[](const int i) const {
    return side[i];
  }
};
	
class Bmatrix {
private:
  MV_ColMat_double B[3];
  void setB(const SideList&);
public:
  Bmatrix(const TriElement*);
  Bmatrix(const SideList&);
  const MV_ColMat_double &operator[](const int i) const {
    return B[i];
  }
};

#ifdef THERMAL
// G converts a vector of temperatures at nodes to the temperature
// gradient.  The out of plane component of the gradient is zero.
// grad_i T = sum_nu G[nu] T_nu, where G[nu] is a three vector and nu
// goes over the nodes of the element.
class Gvector {
private:
  MV_Vector_double G[3];
  void setG(const SideList&);
public:
  Gvector(const TriElement*);
  Gvector(const SideList&);
  const MV_Vector_double &operator[](const int i) const {
    return G[i];
  }
};
#endif // THERMAL

#endif // TRIELEMENT_H
