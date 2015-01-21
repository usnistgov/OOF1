// -*- C++ -*-
// $RCSfile: damageelement.C,v $
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


// Damage element, based on GTElement and MutableElement

#include "damageelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "createelement.h"
#include "symeig3.h"
#include <math.h>

// static default data
CharString DamageElement::nameplate("damage");
double DamageElement::young_dflt(1.0);
double DamageElement::poisson_dflt(0.0);
double DamageElement::t_coeff_dflt(0.0);
#ifdef THERMAL
double DamageElement::kappa_dflt(0.0);
#endif // THERMAL
double DamageElement::knockdown1_dflt(0.1);
double DamageElement::knockdown2_dflt(0.2);
double DamageElement::maxstress_dflt(0.1);
int DamageElement::Nnodes(3);

ElementTypeRegistration DamageElement::reg(nameplate,
					   DamageElement::binaryread);

// Constructor

DamageElement::DamageElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    maxstress(maxstress_dflt)
{
  orientation = 0;
  if(mutated)
    // cijkl has been created by MutableElement constructor
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

Cijkl DamageElement::virgin_cijkl() const
{
  Cijkl c;
  double tmp = young/((1.+poisson)*(1.-2*poisson));
  c(0, 0) = c(1, 1) = c(2, 2) = tmp*(1.-poisson);
  c(0, 1) = c(0, 2) = c(1, 2) = tmp*poisson;
  c(3, 3) = c(4, 4) = c(5, 5) = 0.5*young/(1.+poisson);
  return c;
}

void DamageElement::binarywrite(FILE *file, char formatflag) const {
  if(mutated) formatflag |= MUTATED;
  GTElement::binarywrite(file, formatflag);
  float psn = poisson;
  float yng = young;
  float tc = t_coeff;
  float mxs = maxstress;
  float kd1 = knockdown1;
  float kd2 = knockdown2;
  writebinary(file, psn);         // poisson (float)
  writebinary(file, yng);         // young (float)
  writebinary(file, tc);          // alpha (float)
#ifdef THERMAL
  float kpa = kappa;
  writebinary(file, kpa);	// kappa (float)
#endif // THERMAL
  writebinary(file, mxs);         // maxstress (float)
  writebinary(file, kd1);	  // knockdown 1 (float)
  writebinary(file, kd2);	  // knockdown 2 (float)
  MutableElement::binarywrite(file);
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *DamageElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, kd1, kd2, mxs;
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
  if(!readbinary(file, mxs)) { ok = TF_FALSE; return 0; } // maxstress (float)
  maxstress_dflt = mxs;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;
  
  // create the element
  return create_element((DamageElement*) 0);
}

CharString DamageElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
#ifdef THERMAL
    + " kappa=" + to_charstring(kappa)
#endif // THERMAL
    + " maxstress=" + to_charstring(maxstress)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

Cijkl DamageElement::setCijkl() const {
  return cijkl;
}

#ifdef THERMAL
Kij DamageElement::setKij() const {
  Kij k;
  k(0, 0) = k(1, 1) = k(2, 2) = kappa;
  return k;
}
#endif // THERMAL

SymmMatrix DamageElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a(1, 1) = a(2, 2) = t_coeff;
  return a;
}

int DamageElement::mutate() {
  if(onceonly && mutated)
    return 0;

  stresstensor();
  EigenVector eig1, eig2, eig3;
  eigensystem(stress, eig1, eig2, eig3);
  if(eig1.eigenvalue < maxstress)
    return 0;

  not_current();
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
  cijkl = Cijkl(aligned_c.transform(-orientation));

  garcon()->msout << ms_info << "mutating " << index
		  << " at stress " << eig1.eigenvalue
		  << " and orientation " << orientation << endl << ms_normal;
  orientation = 0;
  if (intrinsic_gray > 0.0) oldgray = intrinsic_gray;
  intrinsic_gray = 0.0;
  mutated = 1;
  return 1;
}

void DamageElement::revert() {
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

// ----------------------

std::vector<CharString> *DamageElement::print_properties(ostream &os) const {
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
       << "', alpha=" << t_coeff
       << ", t0="
#ifndef THERMAL
       << 0.0
#else  // THERMAL
       << temperature0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Isotropic:" + pname);
  }
#ifdef THERMAL
  if(kappa != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Isotropic(name=" << pname
       << ", kappa=" << kappa << ")" << endl;
    names->push_back("Thermal:Conductivity:Isotropic:" + pname);
  }
#endif // THERMAL
  return names;
}

bool DamageElement::same_type(const Element *el) const {
  const DamageElement *other = dynamic_cast<const DamageElement*>(el);
  return other && GTElement::same_type(el) && MutableElement::same_type(el)
    && young == other->young
    && poisson == other->poisson
    && t_coeff == other->t_coeff
#ifdef THERMAL
    && kappa == other->kappa
#endif // THERMAL
    && maxstress == other->maxstress
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2;
}
