// -*- C++ -*-
// $RCSfile: damisoelement.C,v $
// $Revision: 1.9 $
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
#include "damisoelement.h"
#include "eigenvalues.h"
#include "element.h"
#include "grid.h"
#include "inputflags.h"
#include "mvmult.h"
#include "mvvd.h"
#include "node.h"
#include "parameters.h"
#include "readbinary.h"
#include "symeig3.h"
#include <iostream.h>

// static default data
CharString DamIsoElement::nameplate("damisotropic");
double DamIsoElement::young_dflt(1.0);
double DamIsoElement::poisson_dflt(0.33);
double DamIsoElement::t_coeff_dflt(0.0);
TrueFalse DamIsoElement::planestrain_dflt(0);
double DamIsoElement::maxstress_dflt(1.0);
double DamIsoElement::minstress_dflt(-1.0);
double DamIsoElement::maxknockdown_dflt(0.5);
double DamIsoElement::minknockdown_dflt(0.5);
float DamIsoElement::oldgray_dflt(0.0);
double DamIsoElement::total_knockdown_dflt(1.0);
#ifdef THERMAL
double DamIsoElement::kappa_dflt(0.0);
#endif // THERMAL

int DamIsoElement::Nnodes(3);

ElementTypeRegistration DamIsoElement::reg(nameplate,
					   DamIsoElement::binaryread);


DamIsoElement::DamIsoElement(Grid *g)
  : TriElementT(g), 
  young(young_dflt),
  poisson(poisson_dflt), t_coeff(t_coeff_dflt), 
#ifdef THERMAL
  kappa(kappa_dflt),
#endif // THERMAL
  planestrain(planestrain_dflt),
  maxstress(maxstress_dflt), minstress(minstress_dflt),
  maxknockdown(maxknockdown_dflt), minknockdown(minknockdown_dflt),
  total_knockdown(total_knockdown_dflt),
  oldgray(oldgray_dflt)
{
  if(inputformatflag & STIFFNESS) {
    K = K_dflt;
    thermal_forces = thermal_dflt;
#ifdef THERMAL
    K_t = K_t_dflt;
#endif // THERMAL
  }
  else if(inputformatflag & DUPLICATE) {
    K = ((DamIsoElement*)g->interiorE[cloneindex])->K;
    thermal_forces = ((DamIsoElement*)g->interiorE[cloneindex])->thermal_forces;
#ifdef THERMAL
    K_t = ((DamIsoElement*)g->interiorE[cloneindex])->K_t;
#endif // THERMAL
  }
  else {
    setstiffness();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
  }

  // get ready to create next element.

  // total_knockdown_dflt is an optional argument when reading ascii
  // goof files, so it has to be reset to the default value. This
  // plays the role of Element::reset_defaults(), but DamIsoElement
  // doesn't fit in with the general scheme.
  total_knockdown_dflt = 1.0;
} 

Element *DamIsoElement::binaryread(FILE *file, TrueFalse &ok) {
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

    float mxs, mns, mxk, mnk;
    if(!readbinary(file, mxs)) { ok = TF_FALSE; return 0; } // maxstress(float)
    maxstress_dflt = mxs;
    if(!readbinary(file, mns)) { ok = TF_FALSE; return 0; } // minstress(float)
    minstress_dflt = mns;
    if(!readbinary(file, mxk)) { ok = TF_FALSE; return 0; } // maxknock (float)
    maxknockdown_dflt = mxk;
    if(!readbinary(file, mnk)) { ok = TF_FALSE; return 0; } // minknock (float)
    minknockdown_dflt = mnk;
    
    if(!readbinary(file, inputformatflag)) { ok = TF_FALSE; return 0; } // flag (char)
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
    if(inputformatflag & MUTATED) {
      float tkd;
      if(!readbinary(file, tkd)) { ok = TF_FALSE; return 0; } // total_knockdown
      total_knockdown_dflt = tkd;
      if(!readbinary(file, oldgray_dflt)) { ok = TF_FALSE; return 0; }// oldgray
    }
    return create_element((DamIsoElement*)0);
}

void DamIsoElement::binarywrite(FILE *file, char formatflag) const {
  if(planestrain) formatflag |= PLANESTRAIN;
  //if(plane_temp_grad) formatflag |= PLANE_TEMP_GRAD;
  if(total_knockdown != 1.0) formatflag |= MUTATED;
  TriElementT::binarywrite(file, formatflag);
  float psn = poisson;
  float yng = young;
  float tc = t_coeff;
  writebinary(file, psn);	    // poisson (float)
  writebinary(file, yng);	    // young (float)
  writebinary(file, tc);	    // t_coeff (float)
#ifdef THERMAL
  float kpa = kappa;
  writebinary(file, kpa);	// kappa (float)
#endif // THERMAL
  float mxs = maxstress;
  float mns = minstress;
  float mxk = maxknockdown;
  float mnk = minknockdown;
  writebinary(file, mxs);
  writebinary(file, mns);
  writebinary(file, mxk);
  writebinary(file, mnk);
  writebinary(file, formatflag); // flag (char)
  if(formatflag & STIFFNESS) {
    K.binarywrite(file);	// stiffness matrix
#ifdef THERMAL
    K_t.binarywrite(file);
#endif // THERMAL
    for(int j=0; j<6; j++)
      writebinary(file, thermal_forces[j]);	// thermal forces
  }
  if(formatflag & MUTATED) {
    float tnk = total_knockdown;
    writebinary(file, tnk);
    writebinary(file, oldgray);
  }
}

// ------------------------------------------------------- //

// compute local stiffness matrix K and thermal forces

void DamIsoElement::setstiffness() {
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

void DamIsoElement::setstiffness(MV_ColMat_double *transf) {

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


#ifdef THERMAL
void DamIsoElement::setstiffnessT() {
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

// ---------------------------------------------------------- //

// elastic part of the strain

const StrainStress &DamIsoElement::elastic_straintensor() {
  if(current_elastic_strain)
    return elastic_strain;
  current_elastic_strain = TF_TRUE;
  TriElement::elastic_straintensor();
  double dT = deltaT();
  elastic_strain(0,0) -= dT*t_coeff;
  elastic_strain(1,1) -= dT*t_coeff;
  if(planestrain)
    elastic_strain(2,2) -= dT*t_coeff;
  else
    elastic_strain(2,2) =
      -poisson/(1-poisson)*(elastic_strain(0,0) + elastic_strain(1,1));
  return elastic_strain;
}

StrainStress DamIsoElement::total_straintensor() {
  StrainStress strain(elastic_straintensor());
  double dT = deltaT();
  if(dT != 0) {
    strain(0,0) += dT*t_coeff;
    strain(1,1) += dT*t_coeff;
    strain(2,2) += dT*t_coeff;
  }
  return strain;
}

// ---------------------------------------------------------- //

const StrainStress &DamIsoElement::stresstensor() {
  if(current_stress)
    return stress;
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

#ifdef THERMAL
const MV_Vector_double &DamIsoElement::gradient_temperature_field() {
  // this shouldn't have to override
  // TriElement::gradient_temperature_field(), but if it doesn't, then
  // that function would have to set current_grad_thermal, and then it
  // probably ought to cache its value. That would screw up the
  // caching of the GTElement thermal gradient.
  if(current_grad_thermal) return Grad_T_field;
  current_grad_thermal = TF_TRUE;
  return TriElement::gradient_temperature_field();
}

const MV_Vector_double &DamIsoElement::heat_flux_field() {
  if(current_J) return J_field;
  current_J = TF_TRUE;
  J_field = -kappa*gradient_temperature_field();
  return J_field;
}
#endif // THERMAL

// ---------------------------------------------------------- //

CharString DamIsoElement::parameters() const {
  CharString p = TriElementT::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
#ifdef THERMAL
    + " kappa=" + to_charstring(kappa)
#endif // THERMAL
    + " planestrain=" + to_charstring(planestrain)
    + " max_stress=" +  to_charstring(maxstress)
    + " min_stress=" +to_charstring( minstress)
    + " max_knockdown=" + to_charstring(maxknockdown)
    + " min_knockdown=" + to_charstring(minknockdown);
  if(total_knockdown != 1.0)
    p += " oldgray=" + to_charstring(oldgray)
      + " total_knockdown=" + to_charstring(total_knockdown);
  return p;
}

void DamIsoElement::AddArgs(CommandM *cmd) {
    TriElementT::AddArgs(cmd);
    AddArgument(cmd, "planestrain", planestrain_dflt);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "alpha", t_coeff_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa", kappa_dflt);
#endif // THERMAL
    AddArgument(cmd, "max_stress", maxstress_dflt);
    AddArgument(cmd, "min_stress", minstress_dflt);
    AddArgument(cmd, "max_knockdown", maxknockdown_dflt);
    AddArgument(cmd, "min_knockdown", minknockdown_dflt);
    AddArgument(cmd, "oldgray", oldgray_dflt, MOPT_SECRET);
    AddArgument(cmd, "total_knockdown", total_knockdown_dflt, MOPT_SECRET);
  }

int DamIsoElement::mutate() {
  stresstensor();
  EigenValues eig;
  eigenvalues(stress, eig);
  if(eig.max() > maxstress) {
    knockdown(maxknockdown);
    not_current();
    return 1;
  }
  else if(eig.min() < minstress) {
    knockdown(minknockdown);
    return 1;
  }
  return 0;
}

void DamIsoElement::knockdown(double factor) {
  if(total_knockdown == 1.0)
    oldgray = intrinsic_gray;

  total_knockdown *= factor;

  young *= factor;
  K *= factor;
  thermal_forces *= factor;
  t_coeff_dflt *= factor;
#ifdef THERMAL
  kappa *= factor;
#endif // THERMAL
  intrinsic_gray = 0;
}

void DamIsoElement::revert() {
  if(total_knockdown != 1.0) {
    intrinsic_gray = oldgray;
    young /= total_knockdown;
    t_coeff /= total_knockdown;
    thermal_forces *= 1./total_knockdown;
#ifdef THERMAL
    kappa /= total_knockdown;
#endif // THERMAL
    setstiffness();		// recomputes K and thermal_forces
    total_knockdown = 1.0;
  }
}

// ------------------

bool DamIsoElement::same_type(const Element *el) const {
  const DamIsoElement *other = dynamic_cast<const DamIsoElement*>(el);
  return other && TriElementT::same_type(el)
    && young ==  other->young
    && poisson == other->poisson
    && t_coeff == other->t_coeff
#ifdef THERMAL
    && kappa == other->kappa
#endif // THERMAL
    && maxstress == other->maxstress
    && minstress == other->minstress
    && maxknockdown == other->maxknockdown
    && minknockdown == other->minknockdown;
}

std::vector<CharString> *DamIsoElement::print_properties(ostream &os) const {
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
       << "', alpha=" << t_coeff
       << ", T0="
#ifndef THERMAL
       << 0
#else  // THERMAL
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
