// -*- C++ -*-
// $RCSfile: emptyelement.h,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2001-11-30 16:57:19 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// empty element

class EmptyElement;

#ifndef EMPTYELEMENT_H
#define EMPTYELEMENT_H

#include "debug.h"
#include "trielement.h"
#include "typeregistry.h"

class EmptyElement : public TriElement {
private:
  static CharString nameplate;
  static ElementTypeRegistration reg;
  static StrainStress nostress;
  static Element *binaryread(FILE*, TrueFalse&);
#ifdef THERMAL
  static MV_Vector_double noflux;
#endif // THERMAL
public:
  // constructors
  EmptyElement(Grid*);
  EmptyElement() {}
  virtual ~EmptyElement() {}
  static void AddArgs(CommandM *cmd) {
    TriElement::AddArgs(cmd);
  }
  
  static int Nnodes;
  
  virtual const CharString &tag() const { return nameplate; }
  
  virtual void binarywrite(FILE*, char) const;
  
  virtual void printstiffness(ostream &) const;
  virtual const StrainStress &stresstensor() { return nostress; }
  virtual StrainStress total_straintensor() { return nostress; }
  virtual MeshCoord force(const Node*) const;

#ifdef THERMAL  
  virtual const MV_Vector_double &heat_flux_field() { return noflux; }
  virtual double heat(const Node*) const { return 0; }
#endif // THERMAL

  virtual void newcoords();
  
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
  virtual void loadinterior_rhs(MV_Vector_double&) const;
  virtual void loadboundary_rhs(MV_Vector_double&) const;
  
  virtual void setstiffness();
#ifdef THERMAL
  virtual void setstiffnessT();
  virtual void loadinteriorT(SparseLinkMat&) const;
  virtual void loadboundaryT(SparseLinkMat&,
			    SparseLinkMat&, SparseLinkMat&) const;
#endif // THERMAL
  
  virtual bool isempty() const { return true; }
  virtual bool isempty_phi() const { return true; }
  friend class Debug;
};

#endif
