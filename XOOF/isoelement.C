// -*- C++ -*-
// $RCSfile: isoelement.C,v $
// $Revision: 1.10 $
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


#include "charstring.h"
#include "createelement.h"
#include "element.h"
#include "grid.h"
#include "inputflags.h"
#include "isoelement.h"
#include "mvmult.h"
#include "node.h"
#include "parameters.h"
#include "readbinary.h"
#include "mvvd.h"
#include "strainstress.h"
#include <iostream.h>

// static default data
CharString IsoElement::nameplate("isotropic");
double IsoElement::young_dflt(1.0);
double IsoElement::poisson_dflt(0.0);
double IsoElement::t_coeff_dflt(0.0);
TrueFalse IsoElement::planestrain_dflt(0);
#ifdef THERMAL
double IsoElement::kappa_dflt(0.0);
#endif // THERMAL

int IsoElement::Nnodes(3);

ElementTypeRegistration IsoElement::reg(nameplate, IsoElement::binaryread);

// ------------ Constructor for Isotropic Element. ----------------//

IsoElement::IsoElement(Grid *g)
  : TriElementT(g), 
  young(young_dflt), poisson(poisson_dflt), t_coeff(t_coeff_dflt), 
#ifdef THERMAL
  kappa(kappa_dflt),
#endif // THERMAL
  planestrain(planestrain_dflt)
{
  if(inputformatflag & STIFFNESS) {
    K = K_dflt;
    thermal_forces = thermal_dflt;
#ifdef THERMAL
    K_t = K_t_dflt;
#endif // THERMAL
  }
  else if(inputformatflag & DUPLICATE) {
    K = ((IsoElement*)g->interiorE[cloneindex])->K;
    thermal_forces = ((IsoElement*)g->interiorE[cloneindex])->thermal_forces;
#ifdef THERMAL
    K_t = ((IsoElement*)g->interiorE[cloneindex])->K_t;
#endif // THERMAL
  }
  else {
    setstiffness();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
  }
}

Element *IsoElement::binaryread(FILE *file, TrueFalse &ok) {
  TriElementT::binaryread(file, ok);
  if(!ok) return 0;
  
  float psn, yng, tc;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;	// float to double
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;	// float to double
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; }  // alpha (float)
  t_coeff_dflt = tc;
#ifdef THERMAL
  float kpa;
  if(!readbinary(file, kpa)) { ok = TF_FALSE; return 0; } // kappa (float)
  kappa_dflt = kpa;
#endif // THERMAL
  
  if(!readbinary(file, inputformatflag)) { ok = TF_FALSE; return 0; }
  // flag (char)
  planestrain_dflt = inputformatflag & PLANESTRAIN;
  
  if(inputformatflag & STIFFNESS) {
    if(!K_dflt.binaryread(file)) {		// stiffness matrix
      ok = TF_FALSE;
      return 0;
    }
#ifdef THERMAL
    if(!K_t_dflt.binaryread(file)) {
      ok = TF_FALSE;
      return 0;
    }
#endif // THERMAL
    for(int i=0; i<6; i++)
      if(!readbinary(file, thermal_dflt(i))) {	// thermal forces
	ok = TF_FALSE;
	return 0;
      }
  }
  else if(inputformatflag & DUPLICATE) {
    if(!readbinary(file, cloneindex)) { ok = TF_FALSE; return 0; }
  }
  return create_element((IsoElement*)0);
}

void IsoElement::binarywrite(FILE *file, char formatflag) const {
  if(planestrain) formatflag |= PLANESTRAIN;
  TriElementT::binarywrite(file, formatflag);
  float psn = poisson;
  float yng = young;
  float tc = t_coeff;
  writebinary(file, psn);	// poisson (float)
  writebinary(file, yng);	// young (float)
  writebinary(file, tc);	// t_coeff (float)
#ifdef THERMAL
  float kpa = kappa;
  writebinary(file, kpa);	// kappa (float)
#endif // THERMAL
  writebinary(file, formatflag); // flag (char)
  if(formatflag & STIFFNESS) {
    K.binarywrite(file);	// stiffness matrix
#ifdef THERMAL
    K_t.binarywrite(file);
#endif // THERMAL
    for(int j=0; j<6; j++)
      writebinary(file, thermal_forces[j]); // thermal forces
  }
}

// ------------------------------------------------------- //

// compute local stiffness matrix K and thermal forces

void IsoElement::setstiffness() {
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

void IsoElement::setstiffness(MV_ColMat_double *transf) {

    // Sides of triangle
    const SideList side(this);

    const double area = side.area;
    if(area <= 0.0)
      garcon()->msout << ms_error << "Element has zero or negative area!"
		      << endl << ms_normal;

    const Bmatrix B(side);
        
    double tmp = young/((1.+poisson)*(1.-2.*poisson));
    double C_11 = tmp*(1.-poisson);
    double C_12 = tmp*poisson;
    double C_44 = 0.5*(C_11 - C_12);

    MV_ColMat_double D(3, 3, 0.0);
    
    D(0, 0) = D(1, 1) = C_11;
    D(1, 0) = D(0, 1) = C_12;
    D(2, 2) = 4*C_44;
    
    if(!planestrain) {
	double correction = C_12*C_12/C_11;
	D(0, 0) -= correction;
	D(1, 1) -= correction;
	D(1, 0) -= correction;
	D(0, 1) -= correction;
    }

    double alpha = t_coeff;
    if(planestrain) alpha *= 1. + poisson;    
    
    for(int nu=0; nu<3; nu++) {
	MV_ColMat_double DB(D*B[nu]);
	
	for(int mu=0; mu<=nu; mu++) {
	    MV_ColMat_double knodal(trans_mult(B[mu], DB));
	    K(0, 0, mu, nu) = area*knodal(0, 0);
	    K(1, 1, mu, nu) = area*knodal(1, 1);
	    K(0, 1, mu, nu) = area*knodal(0, 1);
	    K(1, 0, mu, nu) = area*knodal(1, 0);
	}
	
	/* write out this multiplication, since the thermal expansion
	 * coefficient is alpha_11 = alpha_22 = alpha, alpha_12 = 0
	 * 
	 * this leaves out the factor of deltaT, which is only determined
	 * when the system is equilibrated and is factored in by
	 * loadinterior_rhs and loadboundary_rhs in trielement.C
	 */
	thermal_forces[2*nu] = area*alpha*(DB(0, 0) + DB(1, 0));
	thermal_forces[2*nu+1] = area*alpha*(DB(0, 1) + DB(1, 1));
    }
    
    // convert from cartesian to general coordinates --
    // this can be skipped if all nodes are simple XYNodes
    if(transf) {
	K = K.transform(*transf);	// computes A^T K A
	thermal_forces = (*transf)*thermal_forces;
    }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef THERMAL
void IsoElement::setstiffnessT() {
  const SideList side(this);
  const double area = side.area;
  if(area == 0.0)
    garcon()->msout << ms_error << "Element " << index
		    << " has zero area!" << endl << ms_normal;
  if(area < 0.0)
    garcon()->msout << ms_error << "Element " << index
		    << " has negative area! " << area << endl << ms_normal;
  const Gvector G(side);	// 3 3-vectors whose third component is zero
  for(int nu=0; nu<3; nu++)
    for(int mu=0; mu<=nu; mu++)
      K_t(nu, mu) = -area * kappa * (G[nu](0)*G[mu](0) + G[nu](1)*G[mu](1));
}
#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// elastic part of the strain

const StrainStress &IsoElement::elastic_straintensor() {
  if(current_elastic_strain) return elastic_strain;
  current_elastic_strain = TF_TRUE;
  TriElement::elastic_straintensor();
  double dT = deltaT();
  elastic_strain(0,0) -= dT*t_coeff;
  elastic_strain(1,1) -= dT*t_coeff;
  if(planestrain)
    elastic_strain(2,2) -= dT*t_coeff;
  else				// plane stress
    elastic_strain(2,2) =
      -poisson/(1-poisson)*(elastic_strain(0,0) + elastic_strain(1,1));
  return elastic_strain;
}

StrainStress IsoElement::total_straintensor() {
  StrainStress strain(elastic_straintensor());
  double dT = deltaT();
  if(dT != 0.0) {
    strain(0,0) += dT*t_coeff;
    strain(1,1) += dT*t_coeff;
    strain(2,2) += dT*t_coeff;
  }
  return strain;
}

/// ---------------------------------------------------------- //

// compute stress tensor, given strain tensor
const StrainStress &IsoElement::stresstensor() {
  if(current_stress) return stress;
  current_stress = TF_TRUE;
  elastic_straintensor();
  double tmp = young/((1.+poisson)*(1.-2.*poisson));
  double C_11 = tmp*(1.-poisson);
  double C_12 = tmp*poisson;
  double C_44 = 0.5*(C_11 - C_12);
  stress(0,0) =
    C_11*elastic_strain(0,0) + C_12*(elastic_strain(1,1) + elastic_strain(2,2));
  stress(1,1) =
    C_11*elastic_strain(1,1) + C_12*(elastic_strain(0,0) + elastic_strain(2,2));
  stress(2,2) =
    C_11*elastic_strain(2,2) + C_12*(elastic_strain(0,0) + elastic_strain(1,1));
  stress(1,2) = 2*C_44*elastic_strain(1,2);
  stress(0,2) = 2*C_44*elastic_strain(0,2);
  stress(0,1) = 2*C_44*elastic_strain(0,1);
  return stress;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef THERMAL
const MV_Vector_double &IsoElement::gradient_temperature_field() {
  // this shouldn't have to override
  // TriElement::gradient_temperature_field(), but if it doesn't, then
  // that function would have to set current_grad_thermal, and then it
  // probably ought to cache its value. That would screw up the
  // caching of the GTElement thermal gradient.
  if(current_grad_thermal) return Grad_T_field;
  current_grad_thermal = TF_TRUE;
  return TriElement::gradient_temperature_field();
}

const MV_Vector_double &IsoElement::heat_flux_field() {
  if(current_J) return J_field;
  current_J = TF_TRUE;
  J_field = -kappa*gradient_temperature_field();
  return J_field;
}
#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

CharString IsoElement::parameters() const {
  return TriElementT::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
#ifdef THERMAL
    + " kappa=" + to_charstring(kappa)
#endif // THERMAL
    + " planestrain=" + to_charstring(planestrain);
}

std::vector<CharString> *IsoElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = TriElementT::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Isotropic(name='" << pname
     << "', cijkl=IsotropicRank4TensorEnu(young=" << young
     << ", poisson=" << poisson << "))" << endl;
  names->push_back("Elasticity:Isotropic:" + pname);

  if(t_coeff != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Isotropic(name='"
       << pname
       << "', alpha=" << t_coeff << ", T0="
#ifndef THERMAL
       << 0.0
#else // THERMAL
       << temperature0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Isotropic:" + pname);
  }
#ifdef THERMAL
  if(kappa != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Isotropic(name='" << pname
       << "', kappa=" << kappa << ")" << endl;
    names->push_back("Thermal:Conductivity:Isotropic:" + pname);
  }
#endif // THERMAL
  return names;
}

bool IsoElement::same_type(const Element *el) const {
  const IsoElement *other = dynamic_cast<const IsoElement*>(el);
  return other && TriElementT::same_type(el)
    && young == other->young
    && poisson == other->poisson
    && t_coeff == other->t_coeff
    && planestrain == other->planestrain
#ifdef THERMAL
    && kappa == other->kappa
#endif	// THERMAL
    ;
}

