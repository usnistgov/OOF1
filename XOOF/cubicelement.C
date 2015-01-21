// -*- C++ -*-
// $RCSfile: cubicelement.C,v $
// $Revision: 1.11 $
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


// Cubic element, based on GTElement

#include "cubicelement.h"
#include "readbinary.h"
#include "createelement.h"

// static default data
CharString CubicElement::nameplate("cubic");
double CubicElement::young_dflt(1.0);
double CubicElement::poisson_dflt(0.0);
double CubicElement::t_coeff_dflt(0.0);
#ifdef THERMAL
double CubicElement::kappa_dflt(0.0);
#endif // THERMAL
double CubicElement::anisotropy_dflt(1.0);
int CubicElement::Nnodes(3);

ElementTypeRegistration CubicElement::reg(nameplate,
					  CubicElement::binaryread);

// Constructor

CubicElement::CubicElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    anisotropy(anisotropy_dflt)
{
  if(!Kset) {
    
    setstiffness();
    Kset = TF_TRUE;
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
  }
}

void CubicElement::binarywrite(FILE *file, char formatflag) const {
  GTElement::binarywrite(file, formatflag);
  float psn = poisson;
  float yng = young;
  float tc = t_coeff;
  float aniso = anisotropy;
  writebinary(file, psn);	// poisson (float)
  writebinary(file, yng);	// young (float)
  writebinary(file, tc);	// alpha (float)
#ifdef THERMAL	
  float kpa = kappa;
  writebinary(file, kpa);	// kappa (float)
#endif // THERMAL
  writebinary(file, aniso);	// anisotropy (float)
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *CubicElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, aniso;
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
  if(!readbinary(file, aniso)) { ok = TF_FALSE; return 0; } //anisotropy(float)
  anisotropy_dflt = aniso;

  // create the element
  return create_element((CubicElement*) 0);
}

CharString CubicElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
#ifdef THERMAL
    + " kappa=" + to_charstring(kappa)
#endif // THERMAL
    + " anisotropy=" + to_charstring(anisotropy);
}

Cijkl CubicElement::setCijkl() const {
  Cijkl c;
  double tmp = young/((1.+poisson)*(1.-2*poisson));
  c(0, 0) = c(1, 1) = c(2, 2) = tmp*(1.-poisson);
  c(0, 1) = c(0, 2) = c(1, 2) = tmp*poisson;
  c(3, 3) = c(4, 4) = c(5, 5) = 0.5*anisotropy*young/(1.+poisson);
  return c;
}

#ifdef THERMAL
Kij CubicElement::setKij() const {
  Kij k;
  k(0,0) = k(1,1) = k(2,2) = kappa;
  return k;
}
#endif // THERMAL

SymmMatrix CubicElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a(1, 1) = a(2, 2) = t_coeff;
  return a;
}

std::vector<CharString> *CubicElement::print_properties(ostream &os) const {
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
       << "', alpha=CubicRank2Tensor(xx=" << t_coeff << "), T0="
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

bool CubicElement::same_type(const Element *el) const {
  const CubicElement *other = dynamic_cast<const CubicElement*>(el);
  return other && TriElementT::same_type(el)
    && young == other->young
    && poisson == other->poisson
    && anisotropy == other->anisotropy
    && t_coeff == other->t_coeff
#ifdef THERMAL
    && kappa == other->kappa
#endif	// THERMAL
    ;
}
