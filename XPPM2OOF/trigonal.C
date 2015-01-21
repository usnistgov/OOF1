// -*- C++ -*-
// $RCSfile: trigonal.C,v $
// $Revision: 1.11 $
// $Author: langer $
// $Date: 2005-02-17 19:11:31 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "materialcmd.h"
#include "trigonal.h"
#include "readbinary.h"

CharString TrigonalAMaterial::type("trigonalA");
MaterialTypeRegistration
TrigonalAMaterial::reg("trigonalA",
		       new MaterialCmd<TrigonalAMaterial>,
		       new MaterialTriCmd<TrigonalAMaterial>,
		       3.1);

float TrigonalAMaterial::c11_dflt(1.0);
float TrigonalAMaterial::c12_dflt(0.0);
float TrigonalAMaterial::c13_dflt(0.0);
float TrigonalAMaterial::c14_dflt(0.0);
float TrigonalAMaterial::c15_dflt(0.0);
float TrigonalAMaterial::c33_dflt(1.0);
float TrigonalAMaterial::c44_dflt(0.5);
float TrigonalAMaterial::alpha11_dflt(1.0);
float TrigonalAMaterial::alpha33_dflt(1.0);
#ifdef THERMAL
float TrigonalAMaterial::kappa11_dflt(1.0);
float TrigonalAMaterial::kappa33_dflt(1.0);
#endif // THERMAL
EulerAngle TrigonalAMaterial::orientation_dflt;

void TrigonalAMaterial::output(FILE *fp_goof, unsigned char &flag,
			      int n1, int n2, int n3) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3);
  writebinary(fp_goof, float(orientation.alpha()));
  writebinary(fp_goof, float(orientation.beta()));
  writebinary(fp_goof, float(orientation.gamma()));
  writebinary(fp_goof, flag);
  writebinary(fp_goof, c11);
  writebinary(fp_goof, c12);
  writebinary(fp_goof, c13);
  writebinary(fp_goof, c14);
  writebinary(fp_goof, c15);
  writebinary(fp_goof, c33);
  writebinary(fp_goof, c44);
  writebinary(fp_goof, alpha11);
  writebinary(fp_goof, alpha33);
#ifdef THERMAL
  writebinary(fp_goof, kappa11);
  writebinary(fp_goof, kappa33);
#endif // THERMAL
}

void TrigonalAMaterial::output(FILE *fp_goof, unsigned char &flag,
			       int n1, int n2, int n3,
			       int same_element) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3, same_element);
  writebinary(fp_goof, float(orientation.alpha()));
  writebinary(fp_goof, float(orientation.beta()));
  writebinary(fp_goof, float(orientation.gamma()));
  writebinary(fp_goof, flag);
  writebinary(fp_goof, same_element);
  writebinary(fp_goof, c11);
  writebinary(fp_goof, c12);
  writebinary(fp_goof, c13);
  writebinary(fp_goof, c14);
  writebinary(fp_goof, c15);
  writebinary(fp_goof, c33);
  writebinary(fp_goof, c44);
  writebinary(fp_goof, alpha11);
  writebinary(fp_goof, alpha33);
#ifdef THERMAL
  writebinary(fp_goof, kappa11);
  writebinary(fp_goof, kappa33);
#endif // THERMAL
}

int TrigonalAMaterial::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  TrigonalAMaterial *m2 = (TrigonalAMaterial*) other;
  if(c11 != m2->c11) return 0;
  if(c12 != m2->c12) return 0;
  if(c13 != m2->c13) return 0;
  if(c14 != m2->c14) return 0;
  if(c15 != m2->c15) return 0;
  if(c33 != m2->c33) return 0;
  if(c44 != m2->c44) return 0;
  if(alpha11 != m2->alpha11) return 0;
  if(alpha33 != m2->alpha33) return 0;
#ifdef THERMAL
  if(kappa11 != m2->kappa11) return 0;
  if(kappa33 != m2->kappa33) return 0;
#endif // THERMAL
  if(!(orientation == m2->orientation)) return 0;
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

CharString TrigonalBMaterial::type("trigonalB");
MaterialTypeRegistration
TrigonalBMaterial::reg("trigonalB",
		       new MaterialCmd<TrigonalBMaterial>,
		       new MaterialTriCmd<TrigonalBMaterial>,
		       3.2);

float TrigonalBMaterial::c11_dflt(1.0);
float TrigonalBMaterial::c12_dflt(0.0);
float TrigonalBMaterial::c13_dflt(0.0);
float TrigonalBMaterial::c14_dflt(0.0);
float TrigonalBMaterial::c33_dflt(1.0);
float TrigonalBMaterial::c44_dflt(0.5);
float TrigonalBMaterial::alpha11_dflt(1.0);
float TrigonalBMaterial::alpha33_dflt(1.0);
#ifdef THERMAL
float TrigonalBMaterial::kappa11_dflt(1.0);
float TrigonalBMaterial::kappa33_dflt(1.0);
#endif // THERMAL
EulerAngle TrigonalBMaterial::orientation_dflt;

void TrigonalBMaterial::output(FILE *fp_goof, unsigned char &flag,
			      int n1, int n2, int n3) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3);
  writebinary(fp_goof, float(orientation.alpha()));
  writebinary(fp_goof, float(orientation.beta()));
  writebinary(fp_goof, float(orientation.gamma()));
  writebinary(fp_goof, flag);
  writebinary(fp_goof, c11);
  writebinary(fp_goof, c12);
  writebinary(fp_goof, c13);
  writebinary(fp_goof, c14);
  writebinary(fp_goof, c33);
  writebinary(fp_goof, c44);
  writebinary(fp_goof, alpha11);
  writebinary(fp_goof, alpha33);
#ifdef THERMAL
  writebinary(fp_goof, kappa11);
  writebinary(fp_goof, kappa33);
#endif // THERMAL
}

void TrigonalBMaterial::output(FILE *fp_goof, unsigned char &flag,
			       int n1, int n2, int n3,
			       int same_element) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3, same_element);
  writebinary(fp_goof, float(orientation.alpha()));
  writebinary(fp_goof, float(orientation.beta()));
  writebinary(fp_goof, float(orientation.gamma()));
  writebinary(fp_goof, flag);
  writebinary(fp_goof, same_element);
  writebinary(fp_goof, c11);
  writebinary(fp_goof, c12);
  writebinary(fp_goof, c13);
  writebinary(fp_goof, c14);
  writebinary(fp_goof, c33);
  writebinary(fp_goof, c44);
  writebinary(fp_goof, alpha11);
  writebinary(fp_goof, alpha33);
#ifdef THERMAL
  writebinary(fp_goof, kappa11);
  writebinary(fp_goof, kappa33);
#endif // THERMAL
}

int TrigonalBMaterial::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  TrigonalBMaterial *m2 = (TrigonalBMaterial*) other;
  if(c11 != m2->c11) return 0;
  if(c12 != m2->c12) return 0;
  if(c13 != m2->c13) return 0;
  if(c14 != m2->c14) return 0;
  if(c33 != m2->c33) return 0;
  if(c44 != m2->c44) return 0;
  if(alpha11 != m2->alpha11) return 0;
  if(alpha33 != m2->alpha33) return 0;
#ifdef THERMAL
  if(kappa11 != m2->kappa11) return 0;
  if(kappa33 != m2->kappa33) return 0;
#endif // THERMAL
  if(!(orientation == m2->orientation)) return 0;
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

CharString TrigonalCrackMaterial::type("trigonalCrack");
MaterialTypeRegistration
TrigonalCrackMaterial::reg(TrigonalCrackMaterial::type,
			   new MaterialCmd<TrigonalCrackMaterial>,
			   new MaterialTriCmd<TrigonalCrackMaterial>,
			   3.3);

float TrigonalCrackMaterial::c11_dflt(1.0);
float TrigonalCrackMaterial::c12_dflt(0.0);
float TrigonalCrackMaterial::c13_dflt(0.0);
float TrigonalCrackMaterial::c14_dflt(0.0);
float TrigonalCrackMaterial::c15_dflt(0.0);
float TrigonalCrackMaterial::c33_dflt(1.0);
float TrigonalCrackMaterial::c44_dflt(0.5);
float TrigonalCrackMaterial::alpha11_dflt(1.0);
float TrigonalCrackMaterial::alpha33_dflt(1.0);
#ifdef THERMAL
float TrigonalCrackMaterial::kappa11_dflt(1.0);
float TrigonalCrackMaterial::kappa33_dflt(1.0);
#endif // THERMAL
EulerAngle TrigonalCrackMaterial::orientation_dflt;
float TrigonalCrackMaterial::gamma_dflt(0.1);
float TrigonalCrackMaterial::knockdown1_dflt(0.1);
float TrigonalCrackMaterial::knockdown2_dflt(0.2);
TrueFalse TrigonalCrackMaterial::onlyonce_dflt(0);

void TrigonalCrackMaterial::output(FILE *fp_goof, unsigned char &flag,
				   int n1, int n2, int n3) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3);
  writebinary(fp_goof, float(orientation.alpha()));
  writebinary(fp_goof, float(orientation.beta()));
  writebinary(fp_goof, float(orientation.gamma()));
  writebinary(fp_goof, flag);
  writebinary(fp_goof, c11);
  writebinary(fp_goof, c12);
  writebinary(fp_goof, c13);
  writebinary(fp_goof, c14);
  writebinary(fp_goof, c15);
  writebinary(fp_goof, c33);
  writebinary(fp_goof, c44);
  writebinary(fp_goof, alpha11);
  writebinary(fp_goof, alpha33);
#ifdef THERMAL
  writebinary(fp_goof, kappa11);
  writebinary(fp_goof, kappa33);
#endif // THERMAL
  writebinary(fp_goof, gamma);
  writebinary(fp_goof, knockdown1);
  writebinary(fp_goof, knockdown2);
  writebinary(fp_goof, onlyonce?1:0);
}

void TrigonalCrackMaterial::output(FILE *fp_goof, unsigned char &flag,
				   int n1, int n2, int n3,
				   int same_element) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3, same_element);
  writebinary(fp_goof, float(orientation.alpha()));
  writebinary(fp_goof, float(orientation.beta()));
  writebinary(fp_goof, float(orientation.gamma()));
  writebinary(fp_goof, flag);
  writebinary(fp_goof, same_element);
  writebinary(fp_goof, c11);
  writebinary(fp_goof, c12);
  writebinary(fp_goof, c13);
  writebinary(fp_goof, c14);
  writebinary(fp_goof, c15);
  writebinary(fp_goof, c33);
  writebinary(fp_goof, c44);
  writebinary(fp_goof, alpha11);
  writebinary(fp_goof, alpha33);
#ifdef THERMAL
  writebinary(fp_goof, kappa11);
  writebinary(fp_goof, kappa33);
#endif // THERMAL
  writebinary(fp_goof, gamma);
  writebinary(fp_goof, knockdown1);
  writebinary(fp_goof, knockdown2);
  writebinary(fp_goof, onlyonce?1:0);
}

int TrigonalCrackMaterial::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  TrigonalCrackMaterial *m2 = (TrigonalCrackMaterial*) other;
  if(c11 != m2->c11) return 0;
  if(c12 != m2->c12) return 0;
  if(c13 != m2->c13) return 0;
  if(c14 != m2->c14) return 0;
  if(c15 != m2->c15) return 0;
  if(c33 != m2->c33) return 0;
  if(c44 != m2->c44) return 0;
  if(alpha11 != m2->alpha11) return 0;
  if(alpha33 != m2->alpha33) return 0;
#ifdef THERMAL
  if(kappa11 != m2->kappa11) return 0;
  if(kappa33 != m2->kappa33) return 0;
#endif // THERMAL
  if(!(orientation == m2->orientation)) return 0;
  if(gamma != m2->gamma) return 0;
  if(knockdown1 != m2->knockdown1) return 0;
  if(knockdown2 != m2->knockdown2) return 0;
  return 1;
}

// ------------------

std::vector<CharString> *TrigonalAMaterial::print_properties(ostream &os) const
{
  std::vector<CharString> *names = MaterialT::print_properties(os);
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
  pname = newPropertyName();
  os << "OOF.LoadData.Property.Orientation(name='" << pname
     << "', angles=Abg(alpha=" << orientation.alpha()
     << ", beta=" << orientation.beta()
     << ", gamma=" << orientation.gamma() << "))" << endl;
  names->push_back("Orientation:" + pname);
  if(alpha11 != 0.0 || alpha33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Trigonal(name='"
       << pname
       << "', alpha=TrigonalRank2Tensor(xx=" << alpha11
       << ", zz=" << alpha33 << "), T0="
#ifndef THERMAL
       << 0.0
#else // THERMAL
       << t0
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

std::vector<CharString> *TrigonalBMaterial::print_properties(ostream &os) const
{
  std::vector<CharString> *names = MaterialT::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Anisotropic.TrigonalB(name='" << pname
     << "', cijkl=TrigonalBRank4TensorCij(c11=" << c11
     << ", c12=" << c12
     << ", c13=" << c13
     << ", c14=" << c14
     << ", c33=" << c33
     << ", c44=" << c44 << "))" << endl;
  names->push_back("Elasticity:Anisotropic:TrigonalB:" + pname);
  pname = newPropertyName();
  os << "OOF.LoadData.Property.Orientation(name='" << pname
     << "', angles=Abg(alpha=" << orientation.alpha()
     << ", beta=" << orientation.beta()
     << ", gamma=" << orientation.gamma() << "))" << endl;
  names->push_back("Orientation:" + pname);
  if(alpha11 != 0.0 || alpha33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Trigonal(name='"
       << pname
       << "', alpha=TrigonalRank2Tensor(xx=" << alpha11
       << ", zz=" << alpha33 << "), T0="
#ifndef THERMAL
       << 0.0
#else // THERMAL
       << t0
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

std::vector<CharString> *TrigonalCrackMaterial::print_properties(ostream &os)
  const
{
  std::vector<CharString> *names = MaterialT::print_properties(os);
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
  pname = newPropertyName();
  os << "OOF.LoadData.Property.Orientation(name='" << pname
     << "', angles=Abg(alpha=" << orientation.alpha()
     << ", beta=" << orientation.beta()
     << ", gamma=" << orientation.gamma() << "))" << endl;
  names->push_back("Orientation:" + pname);
  if(alpha11 != 0.0 || alpha33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Trigonal(name='"
       << pname
       << "', alpha=TrigonalRank2Tensor(xx=" << alpha11
       << ", zz=" << alpha33 << "), T0="
#ifndef THERMAL
       << 0.0
#else // THERMAL
       << t0
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
