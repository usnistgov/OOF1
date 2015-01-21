// -*- C++ -*-
// $RCSfile: trigonal.C,v $
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

// Trigonal element

#include "createelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "trigonal.h"
#include "symeig3.h"

// static default data
CharString TrigonalAElement::nameplate("trigonalA");
double TrigonalAElement::c11_dflt(1.0);
double TrigonalAElement::c12_dflt(0.0);
double TrigonalAElement::c13_dflt(0.0);
double TrigonalAElement::c14_dflt(0.0);
double TrigonalAElement::c15_dflt(0.0);
double TrigonalAElement::c33_dflt(1.0);
double TrigonalAElement::c44_dflt(0.5);
double TrigonalAElement::alpha11_dflt(1.0);
double TrigonalAElement::alpha33_dflt(1.0);
#ifdef THERMAL
double TrigonalAElement::kappa11_dflt(1.0);
double TrigonalAElement::kappa33_dflt(1.0);
#endif // THERMAL
int TrigonalAElement::Nnodes(3);

ElementTypeRegistration TrigonalAElement::reg(nameplate,
					     TrigonalAElement::binaryread);

// Constructor

TrigonalAElement::TrigonalAElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c14(c14_dflt),
    c15(c15_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    alpha11(alpha11_dflt),
    alpha33(alpha33_dflt)
#ifdef THERMAL
  ,
    kappa11(kappa11_dflt),
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

void TrigonalAElement::binarywrite(FILE *file, char formatflag) const {
  GTElement::binarywrite(file, formatflag);
  float c;
  c = c11; writebinary(file, c); // c11 (float)
  c = c12; writebinary(file, c); // c12 (float)
  c = c13; writebinary(file, c); // c13 (float)
  c = c14; writebinary(file, c); // c14 (float)
  c = c15; writebinary(file, c); // c15 (float)
  c = c33; writebinary(file, c); // c33 (float)
  c = c44; writebinary(file, c); // c44 (float)
  c = alpha11; writebinary(file, c); // alpha11 (float)
  c = alpha33; writebinary(file, c); // alpha33 (float)
#ifdef THERMAL
  c = kappa11; writebinary(file, c); // kappa11 (float)
  c = kappa33; writebinary(file, c); // kappa33 (float)
#endif // THERMAL
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *TrigonalAElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables
  // that will be used by the constructor.

  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c11 (float)
  c11_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c12 (float)
  c12_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c13 (float)
  c13_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c14 (float)
  c14_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c15 (float)
  c15_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c33 (float)
  c33_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c44 (float)
  c44_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha11 (float)
  alpha11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha33 (float)
  alpha33_dflt = c;		// float to double
#ifdef THERMAL
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// kappa11 (float)
  kappa11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// kappa33 (float)
  kappa33_dflt = c;		// float to double
#endif // THERMAL

  // create the element
  return create_element((TrigonalAElement*) 0);
}

CharString TrigonalAElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c14=" + to_charstring(c14)
    + " c15=" + to_charstring(c15)
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

Cijkl TrigonalAElement::setCijkl() const {
  Cijkl c;
  c(0,0) = c(1,1) = c11;
  c(0,1) = c12;
  c(0,2) = c(1,2) = c13;
  c(2,2) = c33;
  c(3,3) = c(4,4) = c44;
  c(5,5) = 0.5*(c11 - c12);

  c(0,3) = c14;
  c(1,3) = -c14;
  c(4,5) = c14;

  c(0,4) = c15;
  c(1,4) = -c15;
  c(3,5) = -c15;
  return c;
}

#ifdef THERMAL
Kij TrigonalAElement::setKij() const {
  Kij k;
  k(0,0) = k(1,1) = kappa11;
  k(2,2) = kappa33;
  return k;
}
#endif // THERMAL

SymmMatrix TrigonalAElement::setAlpha() const {
  SymmMatrix a(3);
  a(0,0) = a(1,1) = alpha11;
  a(2,2) = alpha33;
  return a;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// static default data
CharString TrigonalBElement::nameplate("trigonalB");
double TrigonalBElement::c11_dflt(1.0);
double TrigonalBElement::c12_dflt(0.0);
double TrigonalBElement::c13_dflt(0.0);
double TrigonalBElement::c14_dflt(0.0);
double TrigonalBElement::c33_dflt(1.0);
double TrigonalBElement::c44_dflt(0.5);
double TrigonalBElement::alpha11_dflt(1.0);
double TrigonalBElement::alpha33_dflt(1.0);
#ifdef THERMAL
double TrigonalBElement::kappa11_dflt(1.0);
double TrigonalBElement::kappa33_dflt(1.0);
#endif // THERMAL
int TrigonalBElement::Nnodes(3);

ElementTypeRegistration TrigonalBElement::reg(nameplate,
					     TrigonalBElement::binaryread);

// Constructor

TrigonalBElement::TrigonalBElement(Grid *g)
  : GTElement(g),
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c14(c14_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    alpha11(alpha11_dflt),
    alpha33(alpha33_dflt)
#ifdef THERMAL
  ,
    kappa11(kappa11_dflt),
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

void TrigonalBElement::binarywrite(FILE *file, char formatflag) const {
  GTElement::binarywrite(file, formatflag);
  float c;
  c = c11; writebinary(file, c); // c11 (float)
  c = c12; writebinary(file, c); // c12 (float)
  c = c13; writebinary(file, c); // c13 (float)
  c = c14; writebinary(file, c); // c14 (float)
  c = c33; writebinary(file, c); // c33 (float)
  c = c44; writebinary(file, c); // c44 (float)
  c = alpha11; writebinary(file, c); // alpha11 (float)
  c = alpha33; writebinary(file, c); // alpha33 (float)
#ifdef THERMAL
  c = kappa11; writebinary(file, c); // kappa11 (float)
  c = kappa33; writebinary(file, c); // kappa33 (float)
#endif // THERMAL
}

// static member function that reads the goof file BEFORE creating an
// element.

Element *TrigonalBElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables
  // that will be used by the constructor.

  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c11 (float)
  c11_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c12 (float)
  c12_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c13 (float)
  c13_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c14 (float)
  c14_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c33 (float)
  c33_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c44 (float)
  c44_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha11 (float)
  alpha11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha33 (float)
  alpha33_dflt = c;		// float to double
#ifdef THERMAL
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// kappa11 (float)
  kappa11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// kappa33 (float)
  kappa33_dflt = c;		// float to double
#endif // THERMAL

  // create the element
  return create_element((TrigonalBElement*) 0);
}

CharString TrigonalBElement::parameters() const {
  return GTElement::parameters()
    + " c11=" + to_charstring(c11)
    + " c12=" + to_charstring(c12)
    + " c13=" + to_charstring(c13)
    + " c14=" + to_charstring(c14)
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

Cijkl TrigonalBElement::setCijkl() const {
  Cijkl c;
  c(0,0) = c(1,1) = c11;
  c(0,1) = c12;
  c(0,2) = c(1,2) = c13;
  c(2,2) = c33;
  c(3,3) = c(4,4) = c44;
  c(5,5) = 0.5*(c11 - c12);
  c(0,3) = c(4,5) = c14;
  c(1,3) = -c14;
  return c;
}

#ifdef THERMAL
Kij TrigonalBElement::setKij() const {
  Kij k;
  k(0,0) = k(1,1) = kappa11;
  k(2,2) = kappa33;
  return k;
}
#endif // THERMAL

SymmMatrix TrigonalBElement::setAlpha() const {
  SymmMatrix a(3);
  a(0,0) = a(1,1) = alpha11;
  a(2,2) = alpha33;
  return a;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

double TrigonalCrack::knockdown1_dflt(0.1);
double TrigonalCrack::knockdown2_dflt(0.2);
double TrigonalCrack::gamma_dflt(0.1);
CharString TrigonalCrack::nameplate("trigonalCrack");
ElementTypeRegistration TrigonalCrack::reg(TrigonalCrack::nameplate,
					      TrigonalCrack::binaryread);

TrigonalCrack::TrigonalCrack(Grid *g)
  : TrigonalAElement(g),
    knockdown1(knockdown1_dflt),
    knockdown2(knockdown2_dflt),
    gamma(gamma_dflt)
{
  if(mutated)
    oldgray = oldgray_dflt;
  else
    cijkl = virgin_cijkl();

  // This is a kludge. The local stiffness matrix is calculated in the
  // TrigonalAElement constructor, but that's done without knowledge
  // of the MutableElement.  To do this correctly, setstiffness should
  // be called for ALL elements only after the derived class
  // constructor is called, but that would require too great a rewrite
  // to be feasible now. We'll do it right in version 2.0.
  Kset = TF_FALSE;
  setstiffness();
#ifdef THERMAL
  setstiffnessT();
#endif // THERMAL
}

void TrigonalCrack::binarywrite(FILE *file, char formatflag) const {
  if(mutated) formatflag |= MUTATED;
  TrigonalAElement::binarywrite(file, formatflag);
  float gma = gamma;
  float kd1 = knockdown1;
  float kd2 = knockdown2;
  writebinary(file, gma);	// gamma (float)
  writebinary(file, kd1);	// knockdown 1 (float)
  writebinary(file, kd2);	// knockdown 2 (float)
  MutableElement::binarywrite(file);
}

Element *TrigonalCrack::binaryread(FILE *file, TrueFalse &ok) {
  // kludge: We can't just use TrigonalAElement::binaryread() because
  // that calls create_element(). The concrete element classes weren't
  // meant to be used as base classes!

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables
  // that will be used by the constructor.

  // This section is copied from TrigonalAElement::binaryread().
  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c11 (float)
  c11_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c12 (float)
  c12_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c13 (float)
  c13_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c14 (float)
  c14_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c15 (float)
  c15_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c33 (float)
  c33_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c44 (float)
  c44_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha11 (float)
  alpha11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha33 (float)
  alpha33_dflt = c;		// float to double
#ifdef THERMAL
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// kappa11 (float)
  kappa11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// kappa33 (float)
  kappa33_dflt = c;		// float to double
#endif // THERMAL


  float kd1, kd2, gma;
  if(!readbinary(file, gma)) { ok = TF_FALSE; return 0; } // gamma (float)
  gamma_dflt = gma;
  if(!readbinary(file, kd1)) { ok = TF_FALSE; return 0; } // knockdown1 (float)
  knockdown1_dflt = kd1;
  if(!readbinary(file, kd2)) { ok = TF_FALSE; return 0; } // knockdown2 (float)
  knockdown2_dflt = kd2;
  if(!MutableElement::binaryread(file))
    return 0;
  return create_element((TrigonalCrack*) 0);
}

CharString TrigonalCrack::parameters() const {
  return TrigonalAElement::parameters() 
    + " gamma=" + to_charstring(gamma)
    + " knockdown1=" + to_charstring(knockdown1)
    + " knockdown2=" + to_charstring(knockdown2)
    + MutableElement::parameters();
}

Cijkl TrigonalCrack::setCijkl() const {
  return cijkl;
}

Cijkl TrigonalCrack::virgin_cijkl() const {
  return TrigonalAElement::setCijkl();
}

int TrigonalCrack::mutate() {
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

void TrigonalCrack::revert() {
  if(mutated) {
    intrinsic_gray = oldgray;
    orientation = oldorientation;
    cijkl = virgin_cijkl();
    setstiffness();
#ifdef THERMAL
    setstiffnessT();
#endif // THERMAL
    mutated = 0;
    not_current();
  }
}

// ----------------

bool TrigonalAElement::same_type(const Element *el) const {
  const TrigonalAElement *other = dynamic_cast<const TrigonalAElement*>(el);
  return other && GTElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c14 == other->c14
    && c15 == other->c15
    && c33 == other->c33
    && c44 == other->c44
    && alpha11 == other->alpha11
    && alpha33 == other->alpha33
#ifdef THERMAL
    && kappa11 == other->kappa11
    && kappa33 == other->kappa33
#endif // THERMAL
    ;
}

std::vector<CharString> *TrigonalAElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = GTElement::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Anisotropic.TrigonalA(name='" << pname
     << "', cijkl=TrigonalARank4TensorCij(c11=" << c11
     << ", c12=" << c12
     << ", c13=" << c13
     << ", c14=" << c14
     << ", c15=" << c15
     << ", c33=" << c33
     << ", c44=" << c44 << "))" << endl;
  names->push_back("Elasticity:Anisotropic:TrigonalA:" + pname);
  if(alpha11 != 0.0 || alpha33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Trigonal(name='"
       << pname
       << "', alpha=TrigonalRank2Tensor(xx=" << alpha11
       << ", zz=" << alpha33 << "), T0="
#ifndef THERMAL
       << 0.0
#else // THERMAL
       << temperature0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Anisotropic:Trigonal:" + pname);
  }
#ifdef THERMAL
  if(kappa11 != 0.0 || kappa33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Anisotropic.Trigonal(name='"
       << pname
       << "', kappa=TrigonalRank2Tensor(xx=" << kappa11
       << ", zz=" << kappa33 << "))" << endl;
    names->push_back("Thermal:Conductivity:Anisotropic:Trigonal:" + pname);
  }
#endif // THERMAL
  return names;
}


bool TrigonalBElement::same_type(const Element *el) const {
  const TrigonalBElement *other = dynamic_cast<const TrigonalBElement*>(el);
  return other && GTElement::same_type(el)
    && c11 == other->c11
    && c12 == other->c12
    && c13 == other->c13
    && c14 == other->c14
    && c33 == other->c33
    && c44 == other->c44
    && alpha11 == other->alpha11
    && alpha33 == other->alpha33
#ifdef THERMAL
    && kappa11 == other->kappa11
    && kappa33 == other->kappa33
#endif // THERMAL
    ;
}

std::vector<CharString> *TrigonalBElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = GTElement::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Anisotropic.TrigonalB(name='" << pname
     << "', cijkl=TrigonalBRank4TensorCij(c11=" << c11
     << ", c12=" << c12
     << ", c13=" << c13
     << ", c14=" << c14
     << ", c33=" << c33
     << ", c44=" << c44 << "))" << endl;
  names->push_back("Elasticity:Anisotropic:TrigonalB:" + pname);
  if(alpha11 != 0.0 || alpha33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Trigonal(name='"
       << pname
       << "', alpha=TrigonalRank2Tensor(xx=" << alpha11
       << ", zz=" << alpha33 << "), T0="
#ifndef THERMAL
       << 0.0
#else // THERMAL
       << temperature0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Anisotropic:Trigonal:" + pname);
  }
#ifdef THERMAL
  if(kappa11 != 0.0 || kappa33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Anisotropic.Trigonal(name='"
       << pname
       << "', kappa=TrigonalRank2Tensor(xx=" << kappa11
       << ", zz=" << kappa33 << "))" << endl;
    names->push_back("Thermal:Conductivity:Anisotropic:Trigonal:" + pname);
  }
#endif // THERMAL
  

  return names;
}
