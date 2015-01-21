// -*- C++ -*-
// $RCSfile: hexelement.C,v $
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


// Hexagonal element, based on GTElement

#include "hexelement.h"
#include "readbinary.h"
#include "createelement.h"

// static default data
CharString HexElement::nameplate("hexagonal");
double HexElement::c11_dflt(1.0);
double HexElement::c12_dflt(0.0);
double HexElement::c13_dflt(0.0);
double HexElement::c33_dflt(1.0);
double HexElement::c44_dflt(0.5);
double HexElement::alpha11_dflt(1.0);
double HexElement::alpha33_dflt(1.0);
#ifdef THERMAL
double HexElement::kappa11_dflt(1.0);
double HexElement::kappa33_dflt(1.0);
#endif // THERMAL
int HexElement::Nnodes(3);

ElementTypeRegistration HexElement::reg(nameplate, HexElement::binaryread);

// Constructor

HexElement::HexElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    alpha11(alpha11_dflt),
    alpha33(alpha33_dflt)
#ifdef THERMAL
  , kappa11(kappa11_dflt),
    kappa33(kappa33_dflt)
#endif // THERMAL
{
  if(!Kset) {
    setstiffness();
    Kset = TF_TRUE;
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
  }
}

void HexElement::binarywrite(FILE *file, char formatflag) const {
  GTElement::binarywrite(file, formatflag);
  float c;
  c = c11;  writebinary(file, c);         // c11 (float)
  c = c12;  writebinary(file, c);         // c12 (float)
  c = c13;  writebinary(file, c);         // c13 (float)
  c = c33;  writebinary(file, c);         // c33 (float)
  c = c44;  writebinary(file, c);         // c44 (float)
  c = alpha11; writebinary(file, c);      // alpha11 (float)
  c = alpha33; writebinary(file, c);      // alpha33 (float)
#ifdef THERMAL
  c = kappa11; writebinary(file, c);      // kappa11 (float)
  c = kappa33; writebinary(file, c);      // kappa33 (float)
#endif // THERMAL
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *HexElement::binaryread(FILE *file, TrueFalse &ok) {

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
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c33 (float)
  c33_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // c44 (float)
  c44_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha11 (float)
  alpha11_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // alpha33 (float)
  alpha33_dflt = c;  // float to double 
#ifdef THERMAL
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa11 (float)
  kappa11_dflt = c;  // float to double 
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; } // kappa33 (float)
  kappa33_dflt = c;  // float to double 
#endif // THERMAL
  // create the element
  return create_element((HexElement*) 0);
}

CharString HexElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c33=" + to_charstring(c33)
    + " c44=" + to_charstring(c44)
    + " alpha11=" + to_charstring(alpha11)
    + " alpha33=" + to_charstring(alpha33)
#ifdef THERMAL
    + " kappa11=" + to_charstring(kappa11)
    + " kappa33=" + to_charstring(kappa33)
#endif // THERMAL
    ;
}

Cijkl HexElement::setCijkl() const {
  Cijkl c;
  c(0, 0) = c(1, 1) = c11;
  c(2, 2) = c33;
  c(0, 1) = c12;
  c(0, 2) = c(1, 2) = c13;
  c(3, 3) = c(4, 4) = c44;
  c(5, 5) = 0.5*(c11 - c12);
  return c;
}

#ifdef THERMAL
Kij HexElement::setKij() const {
  Kij k;
  k(0,0) = k(1,1) = kappa11;
  k(2,2) = kappa33;
  return k;
}
#endif // THERMAL

SymmMatrix HexElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a(1, 1) = alpha11;
  a(2, 2) = alpha33;
  return a;
}

// ------------

bool HexElement::same_type(const Element *el) const {
  const HexElement *other = dynamic_cast<const HexElement*>(el);
  return other && GTElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c33 == other->c33
    && c44 == other->c44
    && alpha11 == other->alpha11
    && alpha33 == other->alpha33
#ifdef THERMAL
    && kappa11 == other->kappa11
    && kappa33 == other->kappa33
#endif
    ;
}

std::vector<CharString> *HexElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = GTElement::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Anisotropic.Hexagonal(name='" << pname
     << "', cijkl=HexagonalRank4TensorCij(c11=" << c11
     << ", c12=" << c12
     << ", c13=" << c13
     << ", c33=" << c33
     << ", c44=" << c44 << "))" << endl;
  names->push_back("Elasticity:Anisotropic:Hexagonal:" + pname);

  if(alpha11 != 0.0 || alpha33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Hexagonal(name='"
       << pname
       << "', alpha=HexagonalRank2Tensor(xx=" << c11
       << ", zz=" << 33 << "), T0="
#ifndef THERMAL
       << 0.0
#else  // THERMAL
       << temperature0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Anisotropic:Hexagonal:" + pname);
  }
#ifdef THERMAL
  if(kappa11 != 0.0 || kappa33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Anisotropic.Hexagonal(name='" << pname
       << "', kappa=HexagonalRank2Tensor(xx=" << kappa11
       << ", zz=" << kappa33 << "))" << endl;
    names->push_back("Thermal:Conductivity:Anisotropic:Hexagonal:" + pname);
  }

#endif	// THERMAL
  return names;
}
