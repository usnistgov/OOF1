// -*- C++ -*-
// $RCSfile: valeria.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2005-02-15 22:19:50 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@nist.gov. 
 */


// Valeria element, based on GTElement with a random failure mode
// from Weibull statistics

#include "createelement.h"
#include "valeria.h"
#include "inputflags.h"
#include "readbinary.h"
#include "symeig3.h"
#include <math.h>
#include <sys/types.h>
#include <time.h>

// static default data
CharString ValeriaElement::nameplate("valeria");


double ValeriaElement::young_dflt(1.0);
double ValeriaElement::poisson_dflt(0.0);
double ValeriaElement::t_coeff_dflt(0.0);
#ifdef THERMAL
double ValeriaElement::kappa_dflt(0.0);
#endif // THERMAL
double ValeriaElement::weibull_m_dflt(5.0);
double ValeriaElement::weibull_so_dflt(0.5);
double ValeriaElement::weibull_ao_dflt(0.1);
double ValeriaElement::knockdown1_dflt(0.1);
double ValeriaElement::knockdown2_dflt(0.2);
int ValeriaElement::Nnodes(3);

ElementTypeRegistration ValeriaElement::reg(nameplate,
					     ValeriaElement::binaryread);


// Constructor

ValeriaElement::ValeriaElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    weibull_m(weibull_m_dflt),
    weibull_so(weibull_so_dflt),
    weibull_ao(weibull_ao_dflt),
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt)
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
 if (!valeria_seed)
  {
     time_t a_clock = time(NULL);
     valeria_seed =   (localtime(&a_clock)->tm_sec +
              60*(localtime(&a_clock)->tm_min +
          60*(localtime(&a_clock)->tm_hour +
             24*localtime(&a_clock)->tm_yday)));
      srand48(valeria_seed);
   }

}

Cijkl ValeriaElement::virgin_cijkl() const
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

void ValeriaElement::binarywrite(FILE *file, char formatflag) const {
  if(mutated) formatflag |= MUTATED;  
  GTElement::binarywrite(file, formatflag);
  float psn = poisson;
  float yng = young;
  float tc = t_coeff;
  float wm = weibull_m;
  float wso = weibull_so;
  float wao = weibull_ao;
  float kd1 = knockdown1;
  float kd2 = knockdown2;
  writebinary(file, psn);         // poisson (float)
  writebinary(file, yng);         // young (float)
  writebinary(file, tc);          // alpha (float)
#ifdef THERMAL
  float kpa = kappa;
  writebinary(file, kpa);	// kappa(float)
#endif // THERMAL
  writebinary(file, wm);         // weibull_m (float)
  writebinary(file, wso);         // weibull_so (float)
  writebinary(file, wao);         // weibull_ao (float)
  writebinary(file, kd1);	  // knockdown 1 (float)
  writebinary(file, kd2);	  // knockdown 2 (float)
  MutableElement::binarywrite(file);
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *ValeriaElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, kd1, kd2, wm, wso, wao;
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
  if(!readbinary(file, wm)) { ok = TF_FALSE; return 0; } // weibull_m (float)
  weibull_m_dflt = wm;
  if(!readbinary(file, wso)) { ok = TF_FALSE; return 0; } // weibull_so (float)
  weibull_so_dflt = wso;
  if(!readbinary(file, wao)) { ok = TF_FALSE; return 0; } // weibull_ao (float)
  weibull_ao_dflt = wao;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;

  if(!MutableElement::binaryread(file))
    return 0;

  // create the element
  return create_element((ValeriaElement*) 0);
}


CharString ValeriaElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
#ifdef THERMAL
    + " kappa=" + to_charstring(kappa)
#endif //  THERMAL
    + " weibull_m=" + to_charstring(weibull_m)
    + " weibull_so=" + to_charstring(weibull_so)
    + " weibull_ao=" + to_charstring(weibull_ao)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

Cijkl ValeriaElement::setCijkl() const {
  return cijkl;
}

SymmMatrix ValeriaElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a(1, 1) = a(2, 2) = t_coeff;
  return a;
}

#ifdef THERMAL
Kij ValeriaElement::setKij() const {
  Kij k;
  k(0,0) = k(1,1) = k(2,2) = kappa;
  return k;
}
#endif // THERMAL

int ValeriaElement::mutate() {
  if(onceonly && mutated)
    return 0;

  //Check to see if a randomly chosen number is greater
  // than the probability that this element will fail
  // ***** NOT SURE IF RANDOM SEED IS SET YET, NEED TO CHECK
  // NO, IT IS NOT, WILL WANT A STATIC VARIABLE...
  stresstensor();
  EigenVector eig1, eig2, eig3;
  eigensystem(stress, eig1, eig2, eig3);
  // ensure that the max tensile stress is pos.
  if(eig1.eigenvalue <= 0)
    return 0;
  
  double myprob = 1.0 -
    exp(
	-pow(eig1.eigenvalue/weibull_so,weibull_m)*
	original_area()/weibull_ao);
  
  double mytestprob = drand48();
  if (myprob <= mytestprob) return 0; //doesn't mutate in this case


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
  garcon()->msout << ms_info << "weibulling " << index
		  << " at probability " << myprob
                  << " is greater than test " << mytestprob
                  << " with max. princ. stress " << eig1.eigenvalue
		  << " and orientation " << orientation << endl << ms_normal;
  orientation = 0;
  if (intrinsic_gray > 0.0) oldgray = intrinsic_gray;
  intrinsic_gray = 0.0;
  mutated = 1;
  return 1;
}



void ValeriaElement::revert() {
  if(mutated) {
    orientation = 0;
    intrinsic_gray = oldgray;
    cijkl = virgin_cijkl();
    mutated = 0;
    setstiffness();
#ifdef THERMAL
    setstiffnessT();
#endif //  THERMAL
    not_current();
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Include template definitions
#include "tocharstring.C"
#include "readbinary.C"
#include "symeig3.C"

// ------

bool ValeriaElement::same_type(const Element *el) const {
  const ValeriaElement *other = dynamic_cast<const ValeriaElement*>(el);
  return other && GTElement::same_type(el) && MutableElement::same_type(el)
    && young == other->young
    && poisson == other->poisson
    && t_coeff == other->t_coeff
    && knockdown1 == other->knockdown1
    && knockdown2 == other->knockdown2
    && weibull_m == other->weibull_m
    && weibull_so == other->weibull_so
    && weibull_ao == other->weibull_ao
#ifdef THERMAL
    && kappa == other->kappa
#endif // THERMAL
    ;
}

std::vector<CharString> *ValeriaElement::print_properties(ostream &os) const {
  // Treated as isotropic!
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
