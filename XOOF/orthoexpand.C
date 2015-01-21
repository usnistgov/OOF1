// -*- C++ -*-
// $RCSfile: orthoexpand.C,v $
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



// Orthotropic Expanding element, based on GTElement

#include "orthoexpand.h"
#include "readbinary.h"
#include "createelement.h"

// static default data
CharString OrthoExElement::nameplate("eds_el");
double OrthoExElement::young_dflt(1.0);
double OrthoExElement::poisson_dflt(0.0);
double OrthoExElement::a1_dflt(0.0);
double OrthoExElement::a2_dflt(0.0);
double OrthoExElement::a3_dflt(0.0);
#ifdef THERMAL
double OrthoExElement::k1_dflt(0.0);
double OrthoExElement::k2_dflt(0.0);
double OrthoExElement::k3_dflt(0.0);
#endif // THERMAL
int OrthoExElement::Nnodes(3);

ElementTypeRegistration OrthoExElement::reg(nameplate,
					    OrthoExElement::binaryread);

// Constructor

OrthoExElement::OrthoExElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    a1(a1_dflt),
    a2(a2_dflt),
    a3(a3_dflt)
#ifdef THERMAL
  ,
    k1(k1_dflt),
    k2(k2_dflt),
    k3(k3_dflt)
#endif // THERMAL
{
  if(!Kset) {
    setstiffness();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
    Kset = TF_TRUE;
  }
}

void OrthoExElement::binarywrite(FILE *file, char formatflag) const {
  GTElement::binarywrite(file, formatflag);
  float psn = poisson;
  float yng = young;
  float aa1 = a1;
  float aa2 = a2;
  float aa3 = a3;
  writebinary(file, psn);         // poisson (float)
  writebinary(file, yng);         // young (float)
  writebinary(file, aa1);          // alpha (float)
  writebinary(file, aa2);          // alpha (float)
  writebinary(file, aa3);          // alpha (float)
#ifdef THERMAL
  float kk1 = k1;
  float kk2 = k2;
  float kk3 = k3;
  writebinary(file, kk1);          // kappa (float)
  writebinary(file, kk2);          // kappa (float)
  writebinary(file, kk3);          // kappa (float)
#endif // THERMAL
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *OrthoExElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, aa1, aa2, aa3;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, aa1)) { ok = TF_FALSE; return 0; } // alpha (float)
  a1_dflt = aa1;
  if(!readbinary(file, aa2)) { ok = TF_FALSE; return 0; } // alpha (float)
  a2_dflt = aa2;
  if(!readbinary(file, aa3)) { ok = TF_FALSE; return 0; } // alpha (float)
  a3_dflt = aa3;
#ifdef THERMAL
  float kk1, kk2, kk3;
  if(!readbinary(file, kk1)) { ok = TF_FALSE; return 0; } // kappa (float)
  k1_dflt = kk1;
  if(!readbinary(file, kk2)) { ok = TF_FALSE; return 0; } // kappa (float)
  k2_dflt = kk2;
  if(!readbinary(file, kk3)) { ok = TF_FALSE; return 0; } // kappa (float)
  k3_dflt = kk3;
#endif // THERMAL

  // create the element
  return create_element((OrthoExElement*) 0);
}

CharString OrthoExElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" +  to_charstring(young)
    + " a1=" +  to_charstring(a1)
    + " a2=" +  to_charstring(a2)
    + " a3=" +  to_charstring(a3)
#ifdef THERMAL
    + " kappa1=" +  to_charstring(k1)
    + " kappa2=" +  to_charstring(k2)
    + " kappa3=" +  to_charstring(k3)
#endif // THERMAL
    ;
}

Cijkl OrthoExElement::setCijkl() const {
  Cijkl c;
  double tmp = young/((1.+poisson)*(1.-2*poisson));
  c(0, 0) = c(1, 1) = c(2, 2) = tmp*(1.-poisson);
  c(0, 1) = c(0, 2) = c(1, 2) = tmp*poisson;
  c(3, 3) = c(4, 4) = c(5, 5) = 0.5*young/(1.+poisson);
  return c;
}

SymmMatrix OrthoExElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a1;
  a(1, 1) = a2;
  a(2, 2) = a3;
  return a;
}

#ifdef THERMAL
Kij OrthoExElement::setKij() const {
  Kij k;
  k(0, 0) = k1;
  k(1, 1) = k2;
  k(2, 2) = k3;
  return k;
}
#endif // THERMAL

// ------------------------

bool OrthoExElement::same_type(const Element *el) const {
  const OrthoExElement *other = dynamic_cast<const OrthoExElement*>(el);
  return other && GTElement::same_type(el)
    && young == other->young
    && poisson == other->poisson
    && a1 == other->a1
    && a2 == other->a2
    && a3 == other->a3
#ifdef THERMAL
    && k1 == other->k1
    && k2 == other->k2
    && k3 == other->k3
#endif // THERMAL
    ;
};

std::vector<CharString> *OrthoExElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = GTElement::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Isotropic(name='" << pname
     << "', cijkl=IsotropicRank4TensorEnu(young=" << young
     << ", poisson=" << poisson << "))" << endl;
  names->push_back("Elasticity:Isotropic:" + pname);
  if(a1 != 0.0 || a2 != 0.0 || a3 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Orthorhombic('"
       << pname
       << "', alpha=OrthorhombicRank2Tensor(xx=" << a1
       << ", yy=" << a2
       << ", zz=" << a3 << "), T0="
#ifndef THERMAL
       << 0.0
#else
       << temperature0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Anisotropic:Orthorhombic" + pname);
  }
#ifdef THERMAL
  if(k1 != 0.0 || k2 != 0.0 || k3 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Anisotropic.Orthorhombic(name='"
       << pname
       << "', kappa=OrthorhombicRank2Tensor(xx=" << k1
       << ", yy=" << k2 
       << ", zz=" << k3 << "))" << endl;
    names->push_back("Thermal:Conductivity:Anisotropic:Orthorhombic:" + pname);
  }
#endif	// THERMAL
  return names;
}
