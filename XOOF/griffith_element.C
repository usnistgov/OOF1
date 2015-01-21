// -*- C++ -*-
// $RCSfile: griffith_element.C,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2005-02-17 22:26:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Griffith element, based on GTElement with energy release criterion

#include "createelement.h"
#include "griffith_element.h"
#include "inputflags.h"
#include "readbinary.h"
#include "symeig3.h"
#include <math.h>

// static default data
CharString GriffithElement::nameplate("griffith");
CharString GriffithElement2::nameplate("griffith2");

double GriffithElement::young_dflt(1.0);
double GriffithElement::poisson_dflt(0.0);
double GriffithElement::t_coeff_dflt(0.0);
#ifdef THERMAL
double GriffithElement::kappa_dflt(0.0);
#endif // THERMAL
double GriffithElement::knockdown1_dflt(0.1);
double GriffithElement::knockdown2_dflt(0.2);
double GriffithElement::gamma_dflt(0.1);
int GriffithElement::Nnodes(3);

ElementTypeRegistration GriffithElement::reg(nameplate,
					     GriffithElement::binaryread);
ElementTypeRegistration GriffithElement2::reg(nameplate,
					      GriffithElement2::binaryread);

// Constructor

GriffithElement::GriffithElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    gamma(gamma_dflt)
{
  orientation = 0;
  if(mutated)
    oldgray = oldgray_dflt;
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

Cijkl GriffithElement::virgin_cijkl() const
{
  Cijkl c;
  double tmp = young/((1.+poisson)*(1.-2*poisson));
  // if this fn is changed to be like the one in zimmer_element.C,
  // don't forget to call cijkl.clear()
  c(0, 0) = c(1, 1) = c(2, 2) = tmp*(1.-poisson);
  c(0, 1) = c(0, 2) = c(1, 2) = tmp*poisson;
  c(3, 3) = c(4, 4) = c(5, 5) = 0.5*young/(1.+poisson);
  return c;
}

void GriffithElement::binarywrite(FILE *file, char formatflag) const {
  if(mutated) formatflag |= MUTATED;  
  GTElement::binarywrite(file, formatflag);
  float psn = poisson;
  float yng = young;
  float tc = t_coeff;
  float gma = gamma;
  float kd1 = knockdown1;
  float kd2 = knockdown2;
  writebinary(file, psn);         // poisson (float)
  writebinary(file, yng);         // young (float)
  writebinary(file, tc);          // alpha (float)
#ifdef THERMAL
  float kpa = kappa;
  writebinary(file, kpa);	// kappa (float)
#endif // THERMAL
  writebinary(file, gma);         // gamma (float)
  writebinary(file, kd1);	  // knockdown 1 (float)
  writebinary(file, kd2);	  // knockdown 2 (float)
  MutableElement::binarywrite(file);
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *GriffithElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, kd1, kd2, gma;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = tc;
#ifdef THERMAL
  float kpa;
  if(!readbinary(file, kpa)) { ok = TF_FALSE; return 0; } // kappa (float)
  kappa_dflt = kpa;
#endif // THERMAL
  if(!readbinary(file, gma)) { ok = TF_FALSE; return 0; } // gamma (float)
  gamma_dflt = gma;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;

  // create the element
  return create_element((GriffithElement*) 0);
}

Element *GriffithElement2::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, kd1, kd2, gma;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = tc;
#ifdef THERMAL
  float kpa;
  if(!readbinary(file, kpa)) { ok = TF_FALSE; return 0; } // alpha (float)
  kappa_dflt = kpa;
#endif // THERMAL
  if(!readbinary(file, gma)) { ok = TF_FALSE; return 0; } // gamma (float)
  gamma_dflt = gma;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;

  // create the element
  return create_element((GriffithElement2*) 0);
}

CharString GriffithElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
#ifdef THERMAL
    + " kappa=" + to_charstring(kappa)
#endif // THERMAL
    + " gamma=" + to_charstring(gamma)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

Cijkl GriffithElement::setCijkl() const {
  return cijkl;
}

#ifdef THERMAL
Kij GriffithElement::setKij() const {
  Kij k;
  k(0, 0) = k(1, 1) = k(2, 2) = kappa;
  return k;
}
#endif // THERMAL

SymmMatrix GriffithElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a(1, 1) = a(2, 2) = t_coeff;
  return a;
}

int GriffithElement::mutate() {
  if(onceonly && mutated)
    return 0;

  // the total stored energy must exceed the surface energy times a
  // characteristic element length, for the element length use the
  // diameter of a circle with equivalent element area
  // e >= 2 gamma l/area = 8 gamma/(pi l) = 4 gamma/sqrt(pi*area)
  stresstensor();
  EigenVector eig1, eig2, eig3;
  eigensystem(stress, eig1, eig2, eig3);
  // ensure that the max tensile stress is pos.
  if(eig1.eigenvalue <= 0)
    return 0;
  
  double stored_e = elastic_energy();
  double release_rate = 4*gamma/sqrt(M_PI*original_area());

  if (stored_e < release_rate)
     return 0;

  orientation = eig1.orientation();

  Cijkl aligned_c = Cijkl(cijkl.transform(orientation));
  // The old method assumed that in the new coords the principal
  // stress was in the x direction. Now it's in the z direction. So
  // the following transformation has been applied to the indices:
  // 0 <--> 2  (xx <--> zz)
  // 3 <--> 5  (yz <--> xy)
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

  // old method  
  //   aligned_c(0,0) *= knockdown1;
  //   aligned_c(0,1) *= knockdown2;
  //   aligned_c(0,2) *= knockdown2;
  //   aligned_c(0,4) *= knockdown2;   
  //   aligned_c(0,5) *= knockdown2;
  //   aligned_c(1,4) *= knockdown2;
  //   aligned_c(1,5) *= knockdown2;
  //   aligned_c(2,5) *= knockdown2;
  //   aligned_c(2,4) *= knockdown2;
  //   aligned_c(3,5) *= knockdown2;
  //   aligned_c(3,4) *= knockdown2;
  //   aligned_c(4,5) *= knockdown2;
  //   aligned_c(4,4) *= knockdown2;
  //   aligned_c(5,5) *= knockdown2;
  cijkl = Cijkl(aligned_c.transform(-orientation));
  not_current();
  garcon()->msout << ms_info << "fracturing " << index
		  << " at stress " << eig1.eigenvalue
		  << " and orientation " << orientation << endl << ms_normal;
  orientation = 0;
  if (intrinsic_gray > 0.0) oldgray = intrinsic_gray;
  intrinsic_gray = 0.0;
  mutated = 1;
  return 1;
}

int GriffithElement2::mutate() {
  if(onceonly && mutated)
    return 0;

  // the total stored energy must exceed the surface energy times a
  // characteristic element length, for the element length use the
  // diameter of a circle with equivalent element area
  // e >= 2 gamma l/area = 8 gamma/(pi l) = 4 gamma/sqrt(pi*area)
  stresstensor();
  EigenVector stresseig1, stresseig2, stresseig3;
  eigensystem(stress, stresseig1, stresseig2, stresseig3);
  // ensure that the max tensile stress is pos.
  if(stresseig1.eigenvalue <= 0)
    return 0;

  EulerAngle orient(stresseig1.orientation());

  // transform elastic strain tensor so that zz component is in the
  // direction of the principal stress
  elastic_straintensor();
  StrainStress estrain(elastic_strain.transform(orient));

  // use only the part of the energy that
  // will be released when the modulus is zeroed in the
  // direction of the principal stress.
  double stored_e = 0.5*(1-knockdown1)*stresseig1.eigenvalue*estrain(2,2);

  double release_rate = 4*gamma/sqrt(M_PI*original_area());

  if (stored_e < release_rate)
     return 0;


  orientation = orient;		// don't set unless really mutating

  Cijkl aligned_c = Cijkl(cijkl.transform(orientation));
  // The old method assumed that in the new coords the principal
  // stress was in the x direction. Now it's in the z direction. So
  // the following transformation has been applied to the indices:
  // 0 <--> 2  (xx <--> zz)
  // 3 <--> 5  (yz <--> xy)
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

  // old method  
  //   aligned_c(0,0) *= knockdown1;
  //   aligned_c(0,1) *= knockdown2;
  //   aligned_c(0,2) *= knockdown2;
  //   aligned_c(0,4) *= knockdown2;   
  //   aligned_c(0,5) *= knockdown2;
  //   aligned_c(1,4) *= knockdown2;
  //   aligned_c(1,5) *= knockdown2;
  //   aligned_c(2,5) *= knockdown2;
  //   aligned_c(2,4) *= knockdown2;
  //   aligned_c(3,5) *= knockdown2;
  //   aligned_c(3,4) *= knockdown2;
  //   aligned_c(4,5) *= knockdown2;
  //   aligned_c(4,4) *= knockdown2;
  //   aligned_c(5,5) *= knockdown2;

  cijkl = Cijkl(aligned_c.transform(-orientation));
  not_current();
  garcon()->msout << ms_info << "fracturing " << index
		  << " at stress " << stresseig1.eigenvalue
		  << " and orientation " << orientation
		  << endl << ms_normal;
  orientation = 0;
  if (intrinsic_gray > 0.0) oldgray = intrinsic_gray;
  intrinsic_gray = 0.0;
  mutated = 1;
  return 1;
}

void GriffithElement::revert() {
  if(mutated) {
    orientation = 0;
    intrinsic_gray = oldgray;
    cijkl = virgin_cijkl();
    mutated = 0;
    setstiffness();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
    not_current();
  }
}

// --------------------

bool GriffithElement::same_type(const Element *el) const {
  const GriffithElement *other = dynamic_cast<const GriffithElement*>(el);
  return other && GTElement::same_type(el) && MutableElement::same_type(el)
    && young == other->young
    && poisson == other->poisson
    && t_coeff == other->t_coeff
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2
    && gamma == other->gamma
#ifdef THERMAL
    && kappa == other->kappa
#endif
    ;
}

std::vector<CharString> *GriffithElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = GTElement::print_properties(os);
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
#else
       << temperature0
#endif	// THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Isotropic:" + pname);
  }
#ifdef THERMAL
  if(kappa != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Isotropic(name='" << pname
       << "', kappa=" << kappa
       << ")" << endl;
    names->push_back("Thermal:Conductivity:Isotropic:" + pname);
  }
#endif
  return names;
}
