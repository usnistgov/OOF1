// -*- C++ -*-
// $RCSfile: cubic_crack_element.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2005-02-16 16:17:19 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



// Element with Cubic symmetry,
// based on GTElement with energy release criterion

 //note that the thermal coefficients are not knocked down

#include "createelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "symeig3.h"
#include "cubic_crack_element.h"
#include <math.h>

// static default data
CharString CubicCrackElement::nameplate("cubic_crack");

double CubicCrackElement::young_dflt(1.0);
double CubicCrackElement::poisson_dflt(0.0);
double CubicCrackElement::t_coeff_dflt(0.0);
double CubicCrackElement::anisotropy_dflt(1.0);
#ifdef THERMAL
double CubicCrackElement::kappa_dflt(1.0);
#endif // THERMAL
double CubicCrackElement::knockdown1_dflt(0.1);
double CubicCrackElement::knockdown2_dflt(0.2);
double CubicCrackElement::gamma_dflt(0.1);
int CubicCrackElement::Nnodes(3);

ElementTypeRegistration CubicCrackElement::reg(nameplate,
					       CubicCrackElement::binaryread);

CubicCrackElement::CubicCrackElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    anisotropy(anisotropy_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    gamma(gamma_dflt)
{
  if( mutated) {
    oldgray = oldgray_dflt;
    oldorientation = oldorientation_dflt;
  }
  else
    cijkl = virgin_cijkl();
  if(!Kset) {
    setstiffness();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
    Kset = TF_TRUE;
  }
}

Cijkl CubicCrackElement::virgin_cijkl() const {
  Cijkl c;
  double tmp = young/((1.+poisson)*(1.-2*poisson));
  c(0, 0) = c(1, 1) = c(2, 2) = tmp*(1.-poisson);
  c(0, 1) = c(0, 2) = c(1, 2) = tmp*poisson;
  c(3, 3) = c(4, 4) = c(5, 5) = 0.5*anisotropy*young/(1.+poisson);
  return c;
}

void CubicCrackElement::binarywrite(FILE *file, char formatflag) const {
  if(mutated) formatflag |= MUTATED;
  GTElement::binarywrite(file, formatflag);

  float c;
  c = poisson;  writebinary(file, c);	   // poisson (float)
  c = young;  writebinary(file, c);	   // young (float)
  c = t_coeff;  writebinary(file, c);	   // alpha (float)
  c = anisotropy;  writebinary(file, c);   // anisotropy (float)
#ifdef THERMAL
  c = kappa; writebinary(file, c);	   // kappa (float)
#endif // THERMAL
  float gma = gamma;
  float kd1 = knockdown1;
  float kd2 = knockdown2;
  writebinary(file, gma);         // gamma (float)
  writebinary(file, kd1);	  // knockdown 1 (float)
  writebinary(file, kd2);	  // knockdown 2 (float)
  MutableElement::binarywrite(file);
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *CubicCrackElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.
  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // anisotropy (float)
  anisotropy_dflt = c;  // float to double 
#ifdef THERMAL
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa (float)
  kappa_dflt = c;  // float to double 
#endif // THERMAL

  float  kd1, kd2, gma;

  if(!readbinary(file, gma)) { ok = TF_FALSE; return 0; } // maxstress (float)
  gamma_dflt = gma;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;

  // create the element
  return create_element((CubicCrackElement*) 0);
}


CharString CubicCrackElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
#ifdef THERMAL
    + " kappa=" + to_charstring(kappa)
#endif // THERMAL
    + " anisotropy=" + to_charstring(anisotropy)
    + " gamma=" + to_charstring(gamma)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

Cijkl CubicCrackElement::setCijkl() const {
  return cijkl;
}

#ifdef THERMAL
Kij CubicCrackElement::setKij() const {
  Kij k;
  k(0, 0) = k(1, 1) = k(2, 2) = kappa;
  return k;
}
#endif // THERMAL

SymmMatrix CubicCrackElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a(1, 1) = a(2, 2) = t_coeff;
  return a;
}

int CubicCrackElement::mutate() {
  // the total stored energy must exceed the surface energy times a
  // characteristic element length, for the element length use the
  // diameter of a circle with equivalent element area e >= 2 gamma
  // l/area = 8 gamma/(pi l) = 4 gamma/sqrt(pi*area)
  if(onceonly && mutated)
    return 0;
  stresstensor();
  EigenVector stresseig1, stresseig2, stresseig3;
  eigensystem(stress, stresseig1, stresseig2, stresseig3);
  // ensure that the max tensile stress is pos.
  if(stresseig1.eigenvalue <= 0)
    return 0;

  EulerAngle orient(stresseig1.orientation());

  // transform the elastic strain tensor so that the zz component is
  // in the direction of the principal stress
  elastic_straintensor();
  StrainStress estrain(elastic_strain.transform(orient));

  // use only the part of the energy that will be released when the
  // modulus is zeroed in the direction of the principal stress.
  double stored_e = 0.5*(1-knockdown1)*stresseig1.eigenvalue*estrain(2,2);

  double release_rate = 4*gamma/sqrt(M_PI*original_area());

  if (stored_e < release_rate)
     return 0;

  if(!mutated)
    oldorientation = orientation; // only save *original* orientation
  orientation = orient;

  Cijkl aligned_c = Cijkl(cijkl.transform(orientation));
  aligned_c(0,2) *= knockdown2;
  aligned_c(0,3) *= knockdown2;
  aligned_c(0,4) *= knockdown2;
  aligned_c(1,2) *= knockdown2;
  aligned_c(1,3) *= knockdown2;
  aligned_c(1,4) *= knockdown2;
  aligned_c(2,2) *= knockdown1;
  aligned_c(2,3) *= knockdown2;
  aligned_c(2,4) *= knockdown2;   
  aligned_c(2,5) *= knockdown2;   
  aligned_c(3,3) *= knockdown2;
  aligned_c(3,4) *= knockdown2;
  aligned_c(3,5) *= knockdown2;
  aligned_c(4,4) *= knockdown2;
  aligned_c(4,5) *= knockdown2;
 
  cijkl = Cijkl(aligned_c.transform(-orientation));
  not_current();
  garcon()->msout << ms_info << "fracturing " << index
		  << " at stress " << stresseig1.eigenvalue
		  << " and orientation " << orientation << endl << ms_normal;
  orientation = 0;
  if (intrinsic_gray > 0.0) oldgray = intrinsic_gray;
  intrinsic_gray = 0.0;
  mutated = 1;
  return 1;
}

void CubicCrackElement::revert() {
  if(mutated) {
    intrinsic_gray = oldgray;
    orientation = oldorientation;
    cijkl = virgin_cijkl();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
    setstiffness();
    mutated = 0;
    not_current();
  }
}

// --------------------

bool CubicCrackElement::same_type(const Element *el) const {
  const CubicCrackElement *other = dynamic_cast<const CubicCrackElement*>(el);
  return other && GTElement::same_type(el) && MutableElement::same_type(el)
    && young == other->young
    && poisson == other->poisson
    && anisotropy == other->anisotropy
    && t_coeff == other->t_coeff
    && gamma == other->gamma
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2
#ifdef THERMAL
    && kappa == other->kappa
#endif	// THERMAL
    ;
}

std::vector<CharString> *CubicCrackElement::print_properties(ostream &os) const 
{
  std::vector<CharString> *names = GTElement::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Anisotropic.Cubic(name='" << pname
     << "', cijkl=CubicRank4TensorEnu(young=" << young
     << ", poisson=" << poisson
     << ", aniso=" << anisotropy << "))" << endl;
  names->push_back("Elasticity:Anisotropic:Cubic:" + pname);
  if(t_coeff != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Cubic(name='"
       << pname
       << "', alpha=CubicRank2Tensor(xx=" << t_coeff
       << "), T0="
#ifndef THERMAL
       << 0.0
#else  // THERMAL
       << temperature0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Anisotropic:Cubic:" + pname);
  }
#ifdef THERMAL
  if(kappa != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Anisotropic.Cubic(name='"
       << pname
       << "', kappa=CubicRank2Tensor(xx=" << kappa << "))" << endl;
    names->push_back("Thermal:Conductivity:Anisotropic:Cubic:" + pname);
  }
#endif // THERMAL
  return names;
}
