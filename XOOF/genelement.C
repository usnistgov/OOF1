// -*- C++ -*-
// $RCSfile: genelement.C,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2005-02-15 22:19:50 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// General Triangular element

#include "genelement.h"
#include "grid.h"
#include "inputflags.h"
#include "mvmult.h"
#include "parameters.h"
#include "readbinary.h"
#include <iomanip.h>

// default values of arguments
EulerAngle GTElement::orientation_dflt;
TrueFalse GTElement::planestrain_dflt(0);
TrueFalse GTElement::Kset(0);
#ifdef THERMAL
Enum<ThermalPlanarity> GTElement::thermal_planarity_dflt(TP_IN_PLANE_FLUX);
#endif // THERMAL

// ----------------------------------------------------------//

GTElement::GTElement(Grid *g)
  : TriElementT(g),
    planestrain(planestrain_dflt),
    orientation(orientation_dflt)
#ifdef THERMAL
  ,
    thermal_planarity(thermal_planarity_dflt)
#endif // THERMAL
{

  // can't call setstiffness here, since the parameters haven't been set
  // in the derived class
  Kset = TF_FALSE;
 
  if(inputformatflag & STIFFNESS) {
    K = K_dflt;
#ifdef THERMAL
    K_t = K_t_dflt;
#endif // THERMAL
    thermal_forces = thermal_dflt;
    Kset = TF_TRUE;
  }
  else if(inputformatflag & DUPLICATE) {
    K = ((GTElement*) g->interiorE[cloneindex])->K;
#ifdef THERMAL
    K_t = ((GTElement*) g->interiorE[cloneindex])->K_t;
#endif // THERMAL
    thermal_forces = ((GTElement*) g->interiorE[cloneindex])->thermal_forces;
    Kset = TF_TRUE;
  }
}

// ----------------------------------------------------------//

// binaryread is called by the derived class's binaryread. That
// function should *first* call this one.

Element *GTElement::binaryread(FILE *file, TrueFalse &ok) {
  TriElementT::binaryread(file, ok);
  if(!ok) return 0;

  float a, b, c;
  if(!readbinary(file, a)) {			// euler a  (float)
    ok = TF_FALSE;
    return 0;
  }
  if(!readbinary(file, b)) {			// euler b  (float)
    ok = TF_FALSE;
    return 0;
  }
  if(!readbinary(file, c)) {			// euler c  (float)
    ok = TF_FALSE;
    return 0;
  }
  orientation_dflt = EulerAngle(a, b, c);


  if(!readbinary(file, inputformatflag)) {         // flag (char)
    ok = TF_FALSE;
    return 0;
  }
  if((inputformatflag & STIFFNESS) && (inputformatflag&DUPLICATE))
    garcon()->msout << ms_error << "stiffness and duplicate?"
		    << endl << ms_normal;
  planestrain_dflt = inputformatflag & PLANESTRAIN;
#ifdef THERMAL 
  if(inputformatflag & PLANE_TEMP_GRAD)
    thermal_planarity_dflt = TP_IN_PLANE_GRADIENTS;
  else
    thermal_planarity_dflt = TP_IN_PLANE_FLUX;
#endif // THERMAL

  if(inputformatflag & STIFFNESS) {
    if(!K_dflt.binaryread(file)) {   // stiffness matrix
      ok = TF_FALSE;
      return 0;
    }
#ifdef THERMAL
    if(!K_t_dflt.binaryread(file)) {
      ok = TF_FALSE;
      return 0;
    }
#endif // THERMAL 
   for(int i=0; i<6; i++)        // thermal forces
      if(!readbinary(file, thermal_dflt(i))) {
	ok = TF_FALSE;
	return 0;
      }
  }
  else if(inputformatflag & DUPLICATE) {
    if(!readbinary(file, cloneindex)) {
      ok = TF_FALSE;
      return 0;
    }

  }
  return 0;
}


void GTElement::binarywrite(FILE *file, char formatflag) const {
  if(planestrain) formatflag |= PLANESTRAIN;
#ifdef THERMAL
  if(thermal_planarity == TP_IN_PLANE_GRADIENTS) formatflag |= PLANE_TEMP_GRAD;
#endif // THERMAL
  TriElementT::binarywrite(file, formatflag);
  float a = orientation.alpha();
  float b = orientation.beta();
  float c = orientation.gamma();
  writebinary(file, a);
  writebinary(file, b);
  writebinary(file, c);
  writebinary(file, formatflag);
  if(formatflag & STIFFNESS) {
    K.binarywrite(file);      // stiffness matrix
#ifdef THERMAL
    K_t.binarywrite(file);
#endif // THERMAL
    for(int j=0; j<6; j++)
      writebinary(file, thermal_forces[j]);  // thermal forces
  }
}

// ----------------------------------------------------------//

void GTElement::setstiffness() {
  /* the transformation matrix is computed first, so that it doesn't
   * have to be recomputed if more than one setup is done.
   */
  if(corner[0]->trivialtransform() && corner[1]->trivialtransform()
     && corner[2]->trivialtransform())
    setstiffness(0);
  else {
    MV_ColMat_double transf(transformation());
    setstiffness(&transf);
  }
}

void GTElement::setstiffness(MV_ColMat_double *transf) {
  int i,j;

  // Sides of triangle
  const SideList side(this);

  const double area = side.area;
  if(area == 0.0)
    garcon()->msout << ms_error << "Element " << index
		    << " has zero area!" << endl << ms_normal;
  if(area < 0.0)
    garcon()->msout << ms_error << "Element " << index
		    << " has negative area! " << area << endl << ms_normal;

  const Bmatrix B(side);  // 3 3x2 matrices
  
  Cijkl cijkl(setCijkl().transform(orientation));
  
  SymmMatrix alpha(setAlpha().transform(orientation));

  if(!planestrain) {		// plane stress
    MV_ColMat_double D(cijkl.planestress()); // 3x3
    D(0, 2) *= 2;
    D(1, 2) *= 2;
    D(2, 2) *= 4;
    D(2, 1) *= 2;
    D(2, 0) *= 2;
    
    
    
    MV_Vector_double alphavec(3);
    alphavec[0] = alpha(0, 0);
    alphavec[1] = alpha(1, 1);
    alphavec[2] = alpha(0, 1);

    for(int nu=0; nu<3; nu++) {
      MV_ColMat_double BD(trans_mult(B[nu], D)); // 2x3
      
      for(int mu=0; mu<=nu; mu++) {
	MV_ColMat_double knodal(BD*B[mu]); 
	
	// 2x2
	K(0, 0, nu, mu) = area*knodal(0, 0);
	K(1, 1, nu, mu) = area*knodal(1, 1);
	K(0, 1, nu, mu) = area*knodal(0, 1);
	K(1, 0, nu, mu) = area*knodal(1, 0);
	
      }
      
      // thermal forces
      MV_Vector_double tf(area*(BD*alphavec));
      thermal_forces[2*nu] = tf[0]; 
      thermal_forces[2*nu+1] = tf[1];
    }
  }
  else {                                             // plane strain
    MV_ColMat_double D(cijkl.planestrain());     // 3x6 [(00 11 01) by voigt]
    D(0, 3) *= 2;		// 0012
    D(0, 4) *= 2;		// 0002
    D(0, 5) *= 2;		// 0001
    D(1, 3) *= 2;               // 1112
    D(1, 4) *= 2;               // 1102
    D(1, 5) *= 2;               // 2212
    D(2, 0) *= 2;		// 0100
    D(2, 1) *= 2;		// 0111
    D(2, 2) *= 2;		// 0122
    D(2, 3) *= 4;		// 0112
    D(2, 4) *= 4;		// 0102
    D(2, 5) *= 4;		// 0101

    MV_Vector_double alphavec(6);
    for(i=0; i<3; i++)
      for(j=0; j<=i; j++)
	alphavec[voigt(i,j)] = alpha(i, j);


    for(int nu=0; nu<3; nu++) {

      MV_ColMat_double BD(trans_mult(B[nu], D));  // 2x6 [xy by voigt]
      MV_ColMat_double BDsmall(2, 3);             // 2x3 [xy by (00 11 01)]
      for(i=0; i<2; i++) {
	BDsmall(i, 0) = BD(i, 0);
	BDsmall(i, 1) = BD(i, 1);
	BDsmall(i, 2) = BD(i, 5);
      }
 
      for(int mu=0; mu<=nu; mu++) {
	MV_ColMat_double knodal(BDsmall*B[mu]);   // 2x2
	K(0, 0, nu, mu) = area*knodal(0, 0);
	K(1, 1, nu, mu) = area*knodal(1, 1);
	K(0, 1, nu, mu) = area*knodal(0, 1);
	K(1, 0, nu, mu) = area*knodal(1, 0);
	
      }
   
      // thermal forces
      MV_Vector_double tf(area*(BD*alphavec));
      thermal_forces[2*nu] = tf[0]; 
      thermal_forces[2*nu+1] = tf[1];
    }
  }

	
  // convert from cartesian to general coordinates --
  // this can be skipped if all nodes are simple XYNodes
  if(transf) {
    K = K.transform(*transf);	// computes A^T K A
    thermal_forces = (*transf)*thermal_forces;
  }
}

#ifdef THERMAL
void GTElement::setstiffnessT() {
  const SideList side(this);	// Sides of triangle
  const double area = side.area;
  if(area == 0.0)
    garcon()->msout << ms_error << "Element " << index
		    << " has zero area!" << endl << ms_normal;
  if(area < 0.0)
    garcon()->msout << ms_error << "Element " << index
		    << " has negative area! " << area << endl << ms_normal;
  const Gvector G(side); // 3 3-dim vectors
  Kij kij(setKij().transform(orientation));
  for(int nu=0; nu<3; nu++) {
    if(thermal_planarity == TP_IN_PLANE_FLUX) {
      MV_ColMat_double KKij = kij.planestress(); //2x2
      //         MV_Vector_double G_2D(2,0);	
      //         G_2D[0] = G[nu](0);	
      //         G_2D[1] = G[nu](1);	
      //      MV_Vector_double Gk(KKij*G[nu]);
      double Gk0 = KKij(0,0)*G[nu][0] + KKij(0,1)*G[nu][1];
      double Gk1 = KKij(1,0)*G[nu][0] + KKij(1,1)*G[nu][1];
      for(int mu=0; mu<=nu; mu++) {
	double GkG = Gk0*G[mu](0) + Gk1*G[mu](1);
	K_t(nu, mu) = -area*GkG; //this is the thermal part
      } 
    }
    else {			// TP_IN_PLANE_GRADIENTS
      MV_Vector_double Gk (kij*G[nu]);
      for(int mu=0; mu<=nu; mu++) {
	double GkG = 0.0;
	for (int kk = 0; kk<2; kk++) // sum < 2, since no out of plane grad.
	  GkG += Gk[kk]*G[mu](kk);
	K_t(nu, mu) = -area*GkG; //this is the thermal part
      }
    }
  }
}
#endif // THERMAL


//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

CharString GTElement::parameters() const {
  return TriElementT::parameters()
    + " orientation=" + to_charstring(orientation)
    + " planestrain=" + to_charstring(planestrain)
#ifdef THERMAL
    + " T_planarity=" + to_charstring(thermal_planarity)
#endif // THERMAL
    ;
}

//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//==\\==//

// compute out of plane strains, assuming system has been equilibrated

const StrainStress &GTElement::elastic_straintensor() {
  if(current_elastic_strain) return elastic_strain;
  current_elastic_strain = TF_TRUE;
  TriElement::elastic_straintensor();
  // include thermal strains
  if(deltaT() != 0)
    elastic_strain -= deltaT()*(setAlpha().transform(orientation));

  if(!planestrain) {	
    // See cijkl.C for definition of psi and indexing convention for it.
    Cijkl cijkl(setCijkl().transform(orientation));
    MV_ColMat_double psi(cijkl.psi_matrix());
    elastic_strain(0,2) = -0.5*(psi(0,0)*elastic_strain(0,0)
				+ psi(0,1)*elastic_strain(1,1)
				+ 2*psi(0,2)*elastic_strain(0,1));
    elastic_strain(1,2) = -0.5*(psi(1, 0)*elastic_strain(0,0)
				+ psi(1,1)*elastic_strain(1,1)
				+ 2*psi(1,2)*elastic_strain(0,1));
    elastic_strain(2,2) = -(psi(2,0)*elastic_strain(0,0)
			    + psi(2,1)*elastic_strain(1,1)
			    + 2*psi(2,2)*elastic_strain(0,1));
  }
  return elastic_strain;
}

#ifdef THERMAL
const MV_Vector_double &GTElement::gradient_temperature_field() {
  if(current_grad_thermal) return Grad_T_field;
  current_grad_thermal = TF_TRUE;
  TriElement::gradient_temperature_field();
  
  
  if(thermal_planarity == TP_IN_PLANE_FLUX) {
    
    Kij kij(setKij().transform(orientation));
    MV_Vector_double psi(kij.psi_vector());
    Grad_T_field(2) -= psi(0)*Grad_T_field(0)+psi(1)*Grad_T_field(1);
				
  }
  return Grad_T_field;
}

const MV_Vector_double &GTElement::heat_flux_field() {
  if (current_J) return J_field;
  current_J = TF_TRUE;
  gradient_temperature_field();
  J_field = -1*(setKij().transform(orientation)*Grad_T_field);
  return J_field;
}
#endif // THERMAL


StrainStress GTElement::total_straintensor() {
  StrainStress strain(elastic_straintensor());
  if(deltaT() != 0.0)
    strain += deltaT()*(setAlpha().transform(orientation));
  return strain;
}

const StrainStress &GTElement::stresstensor() {
  if(current_stress) return stress;
  current_stress = TF_TRUE;
  elastic_straintensor();
  stress = setCijkl().transform(orientation)*elastic_strain;
  return stress;
}

//--------------

bool GTElement::same_type(const Element *el) const {
  const GTElement *other = dynamic_cast<const GTElement*>(el);
  return other && TriElementT::same_type(el)
    && planestrain == other->planestrain
#ifdef THERMAL
    && thermal_planarity == other->thermal_planarity
#endif	// THERMAL
    && orientation == other->orientation;
}

std::vector<CharString> *GTElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = TriElementT::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Orientation(name='" << pname
     << "', angles=Abg(alpha=" << orientation.alpha()
     << ", beta=" << orientation.beta()
     << ", gamma=" << orientation.gamma() << "))" << endl;
  names->push_back("Orientation:" + pname);
  return names;
}
