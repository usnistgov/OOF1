// -*- C++ -*-
// $RCSfile: orthorhombic.C,v $
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



// Orthorhombic element, based on GTElement

#include "orthorhombic.h"
#include "readbinary.h"
#include "createelement.h"

// static default data
CharString OrthoElement::nameplate("orthorhombic");
double OrthoElement::c11_dflt(1.0);
double OrthoElement::c12_dflt(0.0);
double OrthoElement::c13_dflt(0.0);
double OrthoElement::c22_dflt(1.0);
double OrthoElement::c23_dflt(0.0);
double OrthoElement::c33_dflt(1.0);
double OrthoElement::c44_dflt(0.5);
double OrthoElement::c55_dflt(0.5);
double OrthoElement::c66_dflt(0.5);
double OrthoElement::alpha11_dflt(1.0);
double OrthoElement::alpha22_dflt(1.0);
double OrthoElement::alpha33_dflt(1.0);
#ifdef THERMAL
double OrthoElement::kappa11_dflt(1.0);
double OrthoElement::kappa22_dflt(1.0);
double OrthoElement::kappa33_dflt(1.0);
#endif // THERMAL
int OrthoElement::Nnodes(3);

ElementTypeRegistration OrthoElement::reg(nameplate,
					  OrthoElement::binaryread);

// Constructor

OrthoElement::OrthoElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c22(c22_dflt),
    c23(c23_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    c55(c55_dflt),
    c66(c66_dflt),
    alpha11(alpha11_dflt),
    alpha22(alpha22_dflt),
    alpha33(alpha33_dflt)
#ifdef THERMAL
  ,
    kappa11(kappa11_dflt),
    kappa22(kappa22_dflt),
    kappa33(kappa33_dflt)
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

void OrthoElement::binarywrite(FILE *file, char formatflag) const {
  GTElement::binarywrite(file, formatflag);
  float c;
  c = c11;  writebinary(file, c);         // c11 (float)
  c = c12;  writebinary(file, c);         // c12 (float)
  c = c13;  writebinary(file, c);         // c13 (float)
  c = c22;  writebinary(file, c);         // c22 (float)
  c = c23;  writebinary(file, c);         // c23 (float)
  c = c33;  writebinary(file, c);         // c33 (float)
  c = c44;  writebinary(file, c);         // c44 (float)
  c = c55;  writebinary(file, c);         // c55 (float)
  c = c66;  writebinary(file, c);         // c66 (float)
  c = alpha11; writebinary(file, c);      // alpha11 (float)
  c = alpha22; writebinary(file, c);      // alpha22 (float)
  c = alpha33; writebinary(file, c);      // alpha33 (float)
#ifdef THERMAL
  c = kappa11; writebinary(file, c);      // kappa11 (float)
  c = kappa22; writebinary(file, c);      // kappa22 (float)
  c = kappa33; writebinary(file, c);      // kappa33 (float)
#endif // THERMAL
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *OrthoElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c11 (float)
  c11_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c12 (float)
  c12_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c13 (float)
  c13_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c22 (float)
  c22_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c23 (float)
  c23_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c33 (float)
  c33_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c44 (float)
  c44_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c55 (float)
  c55_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c66 (float)
  c66_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha11 (float)
  alpha11_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha22 (float)
  alpha22_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha33 (float)
  alpha33_dflt = c;  // float to double 

#ifdef THERMAL  
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa11 (float)
  kappa11_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa22 (float)
  kappa22_dflt = c;  // float to double 

  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa33 (float)
  kappa33_dflt = c;  // float to double 
#endif // THERMAL

  // create the element
  return create_element((OrthoElement*) 0);
}

CharString OrthoElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c22=" + to_charstring(c22)
    + " c23=" + to_charstring(c23)
    + " c33=" + to_charstring(c33)
    + " c44=" + to_charstring(c44)
    + " c55=" + to_charstring(c55)
    + " c66=" + to_charstring(c66)
    + " alpha11=" + to_charstring(alpha11)
    + " alpha22=" + to_charstring(alpha22)
    + " alpha33=" + to_charstring(alpha33)
#ifdef THERMAL
    + " kappa11=" + to_charstring(kappa11)
    + " kappa22=" + to_charstring(kappa22)
    + " kappa33=" + to_charstring(kappa33)
#endif // THERMAL
    ;
}

Cijkl OrthoElement::setCijkl() const {
  Cijkl c;
  c(0, 0) = c11;
  c(0, 1) = c12;
  c(0, 2) = c13;
  c(1, 1) = c22;
  c(1, 2) = c23;
  c(2, 2) = c33;
  c(3, 3) = c44;
  c(4, 4) = c55;
  c(5, 5) = c66;
  return c;
}

#ifdef THERMAL
Kij OrthoElement::setKij() const {
  Kij k;
  k(0, 0) = kappa11;
  k(1, 1) = kappa22;
  k(2, 2) = kappa33;
  return k;
}
#endif // THERMAL

SymmMatrix OrthoElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = alpha11;
  a(1, 1) = alpha22;
  a(2, 2) = alpha33;
  return a;
}

// ---------------

bool OrthoElement::same_type(const Element *el) const {
  const OrthoElement *other = dynamic_cast<const OrthoElement*>(el);
  return other && GTElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c22 == other->c22
    && c23 == other->c23
    && c33 == other->c33
    && c44 == other->c44
    && c55 == other->c55
    && c66 == other->c66
    && alpha11 == other->alpha11
    && alpha22 == other->alpha22
    && alpha33 == other->alpha33
#ifdef THERMAL
    && kappa11 == other->kappa11
    && kappa22 == other->kappa22
    && kappa33 == other->kappa33
#endif // THERMAL
    ;
}

std::vector<CharString> *OrthoElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = GTElement::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Anisotropic.Orthorhombic(name='"
     << pname
     << "', cijkl=OrthorhombicRank4TensorCij(c11=" << c11
     << ", c12=" << c12
     << ", c13=" << c13
     << ", c22=" << c22
     << ", c23=" << c23
     << ", c33=" << c33
     << ", c44=" << c44
     << ", c55=" << c55
     << ", c66=" << c66 << "))" << endl;
  names->push_back("Elasticity:Anisotropic:Orthorhombic:" + pname);
  if(alpha11 != 0.0 || alpha22 != 0.0 || alpha33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Orthorhombic(name='"
       << pname 
       << "', alpha=OrthorhombicRank2Tensor(xx=" << alpha11
       << ", yy=" << alpha22
       << ", zz=" << alpha33 << "), T0="
#ifndef THERMAL
       << 0.0
#else // THERMAL
       << temperature0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Anisotropic:Orthorhombic:"
		     + pname);
  }
#ifdef THERMAL
  if(kappa11 != 0.0 || kappa22 != 0.0 || kappa33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Anisotropic.Orthorhombic(name='"
       << pname
       << "', kappa=OrthorhombicRank2Tensor(xx=" << kappa11
       << ", yy=" << kappa22
       << ", zz=" << kappa33 << "))" << endl;
    names->push_back("Thermal:Conductivity:Anisotropic:Orthorhombic:" + pname);
  }
#endif
  return names;
}
