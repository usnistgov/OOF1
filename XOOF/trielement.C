// -*- C++ -*-
// $RCSfile: trielement.C,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2001-02-18 02:34:49 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Triangular linear elements

#include "freedom.h"
#include "mvmult.h"
#include "node.h"
#include "parameters.h"
#include "preconditioner.h"
#include "readbinary.h"
#include "symmmatrix.h"
#include "trielement.h"

SymmMatrix TriElement::K_dflt(6);
MV_Vector_double TriElement::thermal_dflt(6, 0.0);
#ifdef THERMAL
SymmMatrix TriElement::K_t_dflt(3);
#endif // THERMAL

TriElement::TriElement(Grid *g)
  : Element(g),
    K(6),
#ifdef THERMAL
    K_t(3),
#endif // THERMAL
    thermal_forces(6, 0.0)
{}

Element *TriElement::binaryread(FILE *file, TrueFalse &ok) {
  Element::binaryread(file, ok);
  if(ok) {
    Element::nodenumber_dflt.resize(3);
    for(int j=0; j<3; j++) {
      if(!readbinary(file, Element::nodenumber_dflt[j])) {
	ok = TF_FALSE;
	break;
      }
    }
  }
  return 0;
}

void TriElement::binarywrite(FILE *file, char formatflag) const {
    Element::binarywrite(file, formatflag);
    for(int j=0; j<3; j++)  
	writebinary(file, corner[j]->index);
}

// ------------------------------------------------------------- //

// Geometry

double TriElement::original_area() const {  // original area of element
  return trianglearea(corner[0]->original(),
		      corner[1]->original(),
		      corner[2]->original());
}

double TriElement::current_area() const {   // actual area of element
  return trianglearea(corner[0]->position(),
		      corner[1]->position(),
		      corner[2]->position());
}

int TriElement::contains(const MeshCoord &point) const {
    MeshCoord a0(corner[0]->position(grid->enhancement()) - point);
    MeshCoord a1(corner[1]->position(grid->enhancement()) - point);
    MeshCoord a2(corner[2]->position(grid->enhancement()) - point);
    if(cross(a0, a1) >= 0.0 && cross(a1, a2) >= 0.0 && cross(a2, a0) >= 0.0)
	return 1;
    return 0;
}

bool TriElement::outside(const Rectangle &rect) const {
  double x[3], y[3];
  for(int i=0; i<3; i++) {
    x[i] = corner[i]->position(grid->enhancement()).x;
    y[i] = corner[i]->position(grid->enhancement()).y;
  }
  bool left = true;
  bool right = true;
  bool above = true;
  bool below = true;
  for(int i=0; i<3; i++) {
    left = left && x[i] < rect.xmin;
    right = right && x[i] > rect.xmax;
    below = below && y[i] < rect.ymin;
    above = above && y[i] > rect.ymax;
  }
  return left || right || above || below;
}

static const double onethird = 1.0/3.0;

MeshCoord TriElement::center(double enhancement) const {
    return onethird*(corner[0]->position(enhancement) +
		     corner[1]->position(enhancement) +
		     corner[2]->position(enhancement));
}

// is this a boundary element?

bool TriElement::isbdy() const {
    return corner[0]->fixed() || corner[1]->fixed() || corner[2]->fixed();
}

/* make a coordinate transformation matrix for all nodes
 * from the individial node transformation matrices.
 */
 
MV_ColMat_double TriElement::transformation() {
    MV_ColMat_double A(6, 6, 0.0);
    for(int nu=0; nu<3; nu++) {
	// indexing scheme for A is same as 4-index SymmMatrix
	A(2*nu  , 2*nu  ) = corner[nu]->transformation()->m00;
	A(2*nu  , 2*nu+1) = corner[nu]->transformation()->m01;
	A(2*nu+1, 2*nu  ) = corner[nu]->transformation()->m10;
	A(2*nu+1, 2*nu+1) = corner[nu]->transformation()->m11;
    }
    return A;
}

// ------------------------------------------------------------- //

SideList::SideList(const TriElement *triel) {
    for(int i=0; i<3; i++)
	side[i] = triel->corner[(i+2)%3]->original() -
			triel->corner[(i+1)%3]->original();
    area = triel->original_area();
}

Bmatrix::Bmatrix(const TriElement *triel) {
    setB(SideList(triel));
}

Bmatrix::Bmatrix(const SideList &side) {
    setB(side);
}

void Bmatrix::setB(const SideList &side) {
    double invarea = 1/side.area;
    for(int mu=0; mu<3; mu++) {
	B[mu].newsize(3, 2);
	B[mu](0, 0) = -0.5*invarea*side[mu].y;
	B[mu](1, 1) =  0.5*invarea*side[mu].x;
	B[mu](0, 1) =  0.0;
	B[mu](1, 0) =  0.0;
	B[mu](2, 0) =  0.25*invarea*side[mu].x;
	B[mu](2, 1) = -0.25*invarea*side[mu].y;
    }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef THERMAL
Gvector::Gvector(const TriElement *triel) {
    setG(SideList(triel));
}

Gvector::Gvector(const SideList &side) {
    setG(side);
}

void Gvector::setG(const SideList &side) {
    double invarea = 1/side.area;
    for(int mu=0; mu<3; mu++) {
	G[mu].newsize(3);
	G[mu](0) =  0.5*invarea*side[mu].y;
	G[mu](1) = -0.5*invarea*side[mu].x;	
	G[mu](2) =  0.0; 
	/*
	fprintf(stderr,"G[%d](0) = %g\n", mu, G[mu](0));
	fprintf(stderr,"G[%d](1) = %g\n", mu, G[mu](1));
	fprintf(stderr,"G[%d](2) = %g\n", mu, G[mu](2));
	fprintf(stderr,"\n");
	*/
    }
    
}
#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Geometrical two dimensional strain, will be corrected for thermal
// strains and plane stress/strain in the derived classes

const StrainStress &TriElement::elastic_straintensor() {
  MV_Vector_double the_strain(3, 0.0);
  const Bmatrix B(this);
  for(int mu=0; mu<3; mu++)
    the_strain += B[mu] * corner[mu]->displacement();
  elastic_strain(0,0) = the_strain(0);
  elastic_strain(1,1) = the_strain(1);
  elastic_strain(0,1) = the_strain(2);
  elastic_strain(0,2) = elastic_strain(1,2) = elastic_strain(2,2) = 0.0;
  return elastic_strain;
}

#ifdef THERMAL
const MV_Vector_double &TriElement::gradient_temperature_field() {
  MV_Vector_double the_GT_field(3, 0.0);
  const Gvector G(this);
  for(int mu=0; mu<3; mu++)
    the_GT_field += G[mu] * corner[mu]->voltage();
  Grad_T_field = -1*the_GT_field;
  
  return Grad_T_field;
}
#endif // THERMAL

// ------------------------------------------------------------- //

// I/O

void TriElement::printstiffness(ostream &os) const {
    for(int i=0; i<3; i++)
	os << "   Node " << corner[i]->index << " " << *corner[i] << endl;
    os << "elastic" << endl;
    os << K << endl;
#ifdef THERMAL
    os << "thermal" << endl << K_t << endl;
#endif // THERMAL
}

// ------------------------------------------------------------ //

// Equilibration without a global matrix

// without creating a global matrix, multiply the displacements
// u by the local stiffness and update the result.

// this version assumes that the element has no boundary nodes
void TriElement::Ku_mult(const MV_Vector_double &u, MV_Vector_double &result)
{
    for(int i=0; i<3; i++) {
	const Node *n = corner[i];
	const int ix = n->x->dofindex();
	const int iy = n->y->dofindex();
	
	// diagonal terms
	result[ix] += K(0, 0, i, i)*u[ix];
	result[ix] += K(0, 1, i, i)*u[iy];
	result[iy] += K(1, 0, i, i)*u[ix];
	result[iy] += K(1, 1, i, i)*u[iy];

	// off diagonal terms
	const int j = (i+1)%3;
	const Node *m = corner[j];
	const int jx = m->x->dofindex();
	const int jy = m->y->dofindex();
	result[ix] += K(0, 0, i, j)*u[jx];
	result[ix] += K(0, 1, i, j)*u[jy];
	result[iy] += K(1, 0, i, j)*u[jx];
	result[iy] += K(1, 1, i, j)*u[jy];
	result[jx] += K(0, 0, j, i)*u[ix];
	result[jx] += K(0, 1, j, i)*u[iy];
	result[jy] += K(1, 0, j, i)*u[ix];
	result[jy] += K(1, 1, j, i)*u[iy];
   }

}

/* this version tests for the row and column fixations
 * the arguments fixedrows and fixedcols indicate which
 * matrix is being multiplied:
 *    fixedrows == 0,  fixedcols == 0 ==> Kii
 *    fixedrows == 1,  fixedcols == 0 ==> Kbi
 *    fixedrows == 0,  fixedcols == 1 ==> Kib
 *    fixedrows == 1,  fixedcols == 1 ==> Kbb
 */

void TriElement::Ku_mult(const MV_Vector_double &u, MV_Vector_double &result,
			const int fixedrows, const int fixedcols)
{
    for(int i=0; i<3; i++) {
	// do diagonal terms (ie, involving one node)
	const Node *n = corner[i];
	const int ix = n->x->dofindex();
	const int iy = n->y->dofindex();
	const int nfixedx = n->x->fixed();   // is this dof fixed?
	const int nfixedy = n->y->fixed();
	
	if(nfixedx == fixedrows) {
	    if(nfixedx == fixedcols) result[ix] += K(0, 0, i, i)*u[ix];
	    if(nfixedy == fixedcols) result[ix] += K(0, 1, i, i)*u[iy];
	}
	if(nfixedy == fixedrows) {
	    if(nfixedx == fixedcols) result[iy] += K(1, 0, i, i)*u[ix];
	    if(nfixedy == fixedcols) result[iy] += K(1, 1, i, i)*u[iy];
	}

	// off diagonal terms (ie, involving a pair of nodes)
	const int j = (i+1)%3;
	const Node *m = corner[j];
	const int jx = m->x->dofindex();
	const int jy = m->y->dofindex();
	const int mfixedx = m->x->fixed();
	const int mfixedy = m->y->fixed();
	
	if(nfixedx == fixedrows) {
	    if(mfixedx == fixedcols) result[ix] += K(0, 0, i, j)*u[jx];
	    if(mfixedy == fixedcols) result[ix] += K(0, 1, i, j)*u[jy];
	}
	if(nfixedy == fixedrows) {
	    if(mfixedx == fixedcols) result[iy] += K(1, 0, i, j)*u[jx];
	    if(mfixedy == fixedcols) result[iy] += K(1, 1, i, j)*u[jy];
	}
	if(mfixedx == fixedrows) {
	    if(nfixedx == fixedcols) result[jx] += K(0, 0, j, i)*u[ix];
	    if(nfixedy == fixedcols) result[jx] += K(0, 1, j, i)*u[iy];
	}
	if(mfixedy == fixedrows) {
	    if(nfixedx == fixedcols) result[jy] += K(1, 0, j, i)*u[ix];
	    if(nfixedy == fixedcols) result[jy] += K(1, 1, j, i)*u[iy];
	}
    }

}

#ifndef THERMAL
// set up diagonal preconditioner w/out global matrix

void TriElement::interiordiagonals(MV_Vector_double &d) const {
    for(int i=0; i<3; i++) {
	if(!corner[i]->x->fixed())
	    d[corner[i]->x->dofindex()] += K(0, 0, i, i);
	if(!corner[i]->y->fixed())
	    d[corner[i]->y->dofindex()] += K(1, 1, i, i);
    }
}

// set up block diagonal preconditioner w/out global matrix

void TriElement::interiorblocks(Vec<BP_block*> block) const {
    for(int i=0; i<3; i++) {
	Node *crnr = corner[i];
	int indx = crnr->index;
	int fixedx = crnr->x->fixed();
	int fixedy = crnr->y->fixed();
	if(fixedx) {
	    if(!fixedy)	    // fixed x, free y
		block[indx]->add(K(1, 1, i, i));
	}
	else {
	    if(fixedy)	    // free x, fixed y
		block[indx]->add(K(0, 0, i, i));
	    else	    // free x, free y
		block[indx]->add(K(0, 0, i, i), K(0, 1, i, i),  K(1, 1, i, i));
	}
    }
}
#endif // !THERMAL

// ---------------------------------------------------------- //

// for constructing global stiffness matrices

void TriElement::loadinterior(SparseLinkMat &kii) const {
    for(int i=0; i<3; i++) {
	int x0 = corner[i]->x->dofindex();
	int y0 = corner[i]->y->dofindex();
	int j = (i+1)%3;
	int x1 = corner[j]->x->dofindex(); 
	int y1 = corner[j]->y->dofindex(); 

	kii(x0, x0) += K(0, 0, i, i);
	kii(x0, y0) += K(0, 1, i, i);
	kii(y0, x0) += K(1, 0, i, i);
	kii(y0, y0) += K(1, 1, i, i);
	
	kii(x0, x1) += K(0, 0, i, j);
	kii(x0, y1) += K(0, 1, i, j);
	kii(y0, x1) += K(1, 0, i, j);
	kii(y0, y1) += K(1, 1, i, j);
	kii(x1, x0) += K(0, 0, j, i);
	kii(x1, y0) += K(0, 1, j, i);
	kii(y1, x0) += K(1, 0, j, i);
	kii(y1, y0) += K(1, 1, j, i);
    }
}



#ifdef THERMAL
void TriElement::loadinteriorT(SparseLinkMat &kii) const {
  for(int i=0; i<3; i++) {
    //3 nodes per element
    
    int phi0 = corner[i]->phi->dofindex();
    int j = (i+1)%3;//i & j go over the nodes
    
    int phi1 = corner[j]->phi->dofindex();
    kii(phi0,phi0) += K_t(i, i);
    kii(phi0,phi1) += K_t(i, j);
    kii(phi1, phi0) += K_t(j, i);
  }
}
#endif // THERMAL


void TriElement::loadboundary(SparseLinkMat &kii,
			SparseLinkMat &kib, SparseLinkMat &kbb) const
{
    for(int i=0; i<3; i++) {
	int x0 = corner[i]->x->dofindex();		// dof index
	int y0 = corner[i]->y->dofindex();		// dof index
	int fixedx0 = corner[i]->x->fixed();	// dof type
	int fixedy0 = corner[i]->y->fixed();	// dof type
	int j = (i+1)%3;
	int x1 = corner[j]->x->dofindex();		// dof index
	int y1 = corner[j]->y->dofindex();		// dof index
	int fixedx1 = corner[j]->x->fixed();	// dof type
	int fixedy1 = corner[j]->y->fixed();	// dof type
	
	// x0-x0
	double v = K(0, 0, i, i);
	if(fixedx0)
	    kbb(x0, x0) += v;
	else
	    kii(x0, x0) += v;
	    
	// y0-y0
	v = K(1, 1, i, i);
	if(fixedy0)
	    kbb(y0, y0) += v;
	else
	    kii(y0, y0) += v;
	    
	// x0-y0
	v = K(0, 1, i, i);
	if(fixedx0) {
	    if(fixedy0) {
		kbb(x0, y0) += v;
		kbb(y0, x0) += v;
	    }
	    else
		kib(y0, x0) += v;
	}
	else {
	    if(fixedy0)
		kib(x0, y0) += v;
	    else {
		kii(x0, y0) += v;
		kii(y0, x0) += v;
	    }
	}
	
	// x0-x1
	v = K(0, 0, i, j);
	if(fixedx0) {
	    if(fixedx1) {
		kbb(x0, x1) += v;
		kbb(x1, x0) += v;
	    }
	    else
		kib(x1, x0) += v;   	
	}
	else {
	    if(fixedx1)
		kib(x0, x1) += v;
	    else {
		kii(x0, x1) += v;
		kii(x1, x0) += v;
	    }
	}
	
	// x0-y1
	v = K(0, 1, i, j);
	if(fixedx0) {
	    if(fixedy1) {
		kbb(x0, y1) += v;
		kbb(y1, x0) += v;
	    }
	    else
		kib(y1, x0) += v;   	
	}
	else {
	    if(fixedy1)
		kib(x0, y1) += v;
	    else {
		kii(x0, y1) += v;
		kii(y1, x0) += v;
	    }
	}
	
	// y0-x1
	v = K(1, 0, i, j);
	if(fixedy0) {
	    if(fixedx1) {
		kbb(y0, x1) += v;
		kbb(x1, y0) += v;
	    }
	    else
		kib(x1, y0) += v;   	
	}
	else {
	    if(fixedx1)
		kib(y0, x1) += v;
	    else {
		kii(y0, x1) += v;
		kii(x1, y0) += v;
	    }
	}
	
	// y0-y1
	v = K(1, 1, i, j);
	if(fixedy0) {
	    if(fixedy1) {
		kbb(y0, y1) += v;
		kbb(y1, y0) += v;
	    }
	    else
		kib(y1, y0) += v;   	
	}
	else {
	    if(fixedy1)
		kib(y0, y1) += v;
	    else {
		kii(y0, y1) += v;
		kii(y1, y0) += v;
	    }
	}
    }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef THERMAL
void TriElement::loadboundaryT(SparseLinkMat &kii,
			SparseLinkMat &kib, SparseLinkMat &kbb) const
{
  for(int i=0; i<3; i++) {
    int phi0 = corner[i]->phi->dofindex(); // dof index
    int fixedphi0 = corner[i]->phi->fixed(); // dof type
    int j = (i+1)%3;
    int phi1 = corner[j]->phi->dofindex(); // dof index
    int fixedphi1 = corner[j]->phi->fixed(); // dof type
    	
    // phi0-phi0
    double v = K_t(i, i);
    if(fixedphi0)
      kbb(phi0, phi0) += v;
    else
      kii(phi0, phi0) += v;
    
    //phi0-phi1
    v = K_t(i, j);
    if(fixedphi0) {
      if(fixedphi1) {
	kbb(phi0, phi1) += v;
	kbb(phi1, phi0) += v;
      }
      else
	kib(phi1, phi0) += v;   	
    }
    else {
      if(fixedphi1)
	kib(phi0, phi1) += v;
      else {
	kii(phi0, phi1) += v;
	kii(phi1, phi0) += v;
      }
    }
  }//end for
}
#endif // THERMAL

// ---------------------------------------------------------- //

#ifdef THERMAL

double TriElement::heat(const Node *n) const {
  double f = 0.0;
  int whichnode;
  for(whichnode=0; whichnode<3; whichnode++)
    if(n == corner[whichnode])
      break;
  if(whichnode == 3) {
    cerr << "Error in TriElement::heat!" << endl;
    return f;
  }
  for(int i=0; i<3; i++) {
    double dphi = corner[i]->phi->disp();
    //the stiffness matrix is essentially the hooke's constant
    //hooke's constant is analogous to the capacitance
    //the third term couples the voltages with the forces
    
    f +=  K_t(whichnode, i)*dphi; 
  }
  return f;
}
#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void TriElement::newcoords() {
    if(!(corner[0]->trivialtransform() && corner[1]->trivialtransform()
	&& corner[2]->trivialtransform()))
	    setstiffness();
}
