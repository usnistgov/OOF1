// -*- C++ -*-
// $RCSfile: orthorhombicmaterial.C,v $
// $Revision: 1.10 $
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
#include "orthorhombicmaterial.h"
#include "readbinary.h"

CharString OrthoRhombicMaterial::type("orthorhombic");
MaterialTypeRegistration
    OrthoRhombicMaterial::reg("orthorhombic",
			      new MaterialCmd<OrthoRhombicMaterial>,
			      new MaterialTriCmd<OrthoRhombicMaterial>,
			      4.0);

float OrthoRhombicMaterial::c11_dflt(1.0);
float OrthoRhombicMaterial::c12_dflt(0.0);
float OrthoRhombicMaterial::c13_dflt(0.0);
float OrthoRhombicMaterial::c22_dflt(0.0);
float OrthoRhombicMaterial::c23_dflt(0.0);
float OrthoRhombicMaterial::c33_dflt(1.0);
float OrthoRhombicMaterial::c44_dflt(0.5);
float OrthoRhombicMaterial::c55_dflt(0.5);
float OrthoRhombicMaterial::c66_dflt(0.5);
float OrthoRhombicMaterial::alpha11_dflt(0.0);
float OrthoRhombicMaterial::alpha22_dflt(0.0);
float OrthoRhombicMaterial::alpha33_dflt(0.0);
#ifdef THERMAL
float OrthoRhombicMaterial::kappa11_dflt(0.0);
float OrthoRhombicMaterial::kappa22_dflt(0.0);
float OrthoRhombicMaterial::kappa33_dflt(0.0);
#endif // THERMAL
EulerAngle OrthoRhombicMaterial::orientation_dflt;

void OrthoRhombicMaterial::output(FILE* fp_goof, unsigned char &flag,
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
  writebinary(fp_goof, c22);
  writebinary(fp_goof, c23);
  writebinary(fp_goof, c33);
  writebinary(fp_goof, c44);
  writebinary(fp_goof, c55);
  writebinary(fp_goof, c66);
  writebinary(fp_goof, alpha11);
  writebinary(fp_goof, alpha22);
  writebinary(fp_goof, alpha33);
#ifdef THERMAL
  writebinary(fp_goof, kappa11);
  writebinary(fp_goof, kappa22);
  writebinary(fp_goof, kappa33);
#endif // THERMAL
}

void OrthoRhombicMaterial::output(FILE* fp_goof, unsigned char &flag,
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
    writebinary(fp_goof, c22);
    writebinary(fp_goof, c23);
    writebinary(fp_goof, c33);
    writebinary(fp_goof, c44);
    writebinary(fp_goof, c55);
    writebinary(fp_goof, c66);
    writebinary(fp_goof, alpha11);
    writebinary(fp_goof, alpha22);
    writebinary(fp_goof, alpha33);
#ifdef THERMAL
    writebinary(fp_goof, kappa11);
    writebinary(fp_goof, kappa22);
    writebinary(fp_goof, kappa33);
#endif // THERMAL
}

int OrthoRhombicMaterial::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  OrthoRhombicMaterial *m2 = (OrthoRhombicMaterial*) other;
  if(c11 != m2->c11) return 0;
  if(c12 != m2->c12) return 0;
  if(c13 != m2->c13) return 0;
  if(c22 != m2->c22) return 0;
  if(c23 != m2->c23) return 0;
  if(c33 != m2->c33) return 0;
  if(c44 != m2->c44) return 0;
  if(c55 != m2->c55) return 0;
  if(c66 != m2->c66) return 0;
  if(alpha11 != m2->alpha11) return 0;
  if(alpha22 != m2->alpha22) return 0;
  if(alpha33 != m2->alpha33) return 0;
#ifdef THERMAL
  if(kappa11 != m2->kappa11) return 0;
  if(kappa22 != m2->kappa22) return 0;
  if(kappa33 != m2->kappa33) return 0;
#endif // THERMAL
  if(!(orientation == m2->orientation)) return 0;
  return 1;
}

// ----------------

std::vector<CharString> *OrthoRhombicMaterial::print_properties(ostream &os)
  const
{
  std::vector<CharString> *names = MaterialT::print_properties(os);
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
  pname = newPropertyName();
  os << "OOF.LoadData.Property.Orientation(name='" << pname
     << "', angles=Abg(alpha=" << orientation.alpha()
     << ", beta=" << orientation.beta()
     << ", gamma=" << orientation.gamma() << "))" << endl;
  names->push_back("Orientation:" + pname);
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
       << t0
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
