// -*- C++ -*-
// $RCSfile: emptyelement.C,v $
// $Revision: 1.7 $
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

// Empty Element
 

#include "charstring.h"
#include "createelement.h"
#include "element.h"
#include "emptyelement.h"
#include "grid.h"
#include "node.h"
#include "parameters.h"
#include "readbinary.h"
#include "mvvd.h"
//#include "stressstrain.h"
#include "strainstress.h"
#include <iostream.h>

// static and default data
int EmptyElement::Nnodes(3);
CharString EmptyElement::nameplate("empty");
ElementTypeRegistration EmptyElement::reg(nameplate, EmptyElement::binaryread);
StrainStress EmptyElement::nostress;
#ifdef THERMAL
MV_Vector_double EmptyElement::noflux(3, 0.0);
#endif // THERMAL

EmptyElement::EmptyElement(Grid *g) : TriElement(g) {
  K.resize(0);
}

Element *EmptyElement::binaryread(FILE *file, TrueFalse &ok) {
  TriElement::binaryread(file, ok);
  if(!ok) return 0;
  return create_element((EmptyElement*)0);
}

void EmptyElement::binarywrite(FILE *file, char formatflag) const {
  TriElement::binarywrite(file, formatflag);
}

void EmptyElement::setstiffness() {}

#ifdef THERMAL
void EmptyElement::setstiffnessT() {}
#endif // THERMAL

void EmptyElement::newcoords() {}

// ---------------------------------------------------------- //

// for constructing global stiffness matrices

void EmptyElement::loadinterior(SparseLinkMat&) const {}

void EmptyElement::loadboundary(SparseLinkMat&,
			SparseLinkMat&, SparseLinkMat&) const {}

void EmptyElement::loadinterior_rhs(MV_Vector_double&) const {}
void EmptyElement::loadboundary_rhs(MV_Vector_double&) const {}

#ifdef THERMAL
void EmptyElement::loadinteriorT(SparseLinkMat&) const {}
void EmptyElement::loadboundaryT(SparseLinkMat&,
				 SparseLinkMat&, SparseLinkMat&) const {}
#endif // THERMAL
 

// ---------------------------------------------------------- //

// without creating a global matrix, multiply the displacements
// u by the local stiffness and update the result.

void EmptyElement::Ku_mult(const MV_Vector_double&, MV_Vector_double&)
{}

void EmptyElement::Ku_mult(const MV_Vector_double&, MV_Vector_double&,
			const int, const int)
{}

#ifndef THERMAL
// set up diagonal preconditioner w/out global matrix

void EmptyElement::interiordiagonals(MV_Vector_double&) const {}

// set up block diagonal preconditioner w/out global matrix

void EmptyElement::interiorblocks(Vec<BP_block*>) const {}
#endif // !THERMAL

// ---------------------------------------------------------- //

MeshCoord EmptyElement::force(const Node*) const {
  return MeshCoord(0.0, 0.0);
}

// ---------------------------------------------------------- //

void EmptyElement::printstiffness(ostream &os) const {
    for(int i=0; i<3; i++)
	os << "   Node " << corner[i]->index << " " << *corner[i] << endl;
    os << "00000000" << endl;
}
