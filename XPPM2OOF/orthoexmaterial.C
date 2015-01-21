// -*- C++ -*-
// $RCSfile: orthoexmaterial.C,v $
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
#include "orthoexmaterial.h"
#include "readbinary.h"

CharString OrthoExMaterial::type("eds_el");
MaterialTypeRegistration
    OrthoExMaterial::reg("eds_el",
			 new MaterialCmd<OrthoExMaterial>,
			 new MaterialTriCmd<OrthoExMaterial>,
			 7.0);

float OrthoExMaterial::young_dflt(1.0);
float OrthoExMaterial::poisson_dflt(0.33);
float OrthoExMaterial::a1_dflt(0.0);
float OrthoExMaterial::a2_dflt(0.0);
float OrthoExMaterial::a3_dflt(0.0);
#ifdef THERMAL
float OrthoExMaterial::k1_dflt(0.0);
float OrthoExMaterial::k2_dflt(0.0);
float OrthoExMaterial::k3_dflt(0.0);
#endif // THERMAL
EulerAngle OrthoExMaterial::orientation_dflt;

void OrthoExMaterial::output(FILE* fp_goof,  unsigned char &flag,
			 int n1, int n2, int n3) const
{
    MaterialT::output(fp_goof, flag, n1, n2, n3);
    writebinary(fp_goof, float(orientation.alpha()));
    writebinary(fp_goof, float(orientation.beta()));
    writebinary(fp_goof, float(orientation.gamma()));
    writebinary(fp_goof, flag);
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, a1);
    writebinary(fp_goof, a2);
    writebinary(fp_goof, a3);
#ifdef THERMAL
    writebinary(fp_goof, k1);
    writebinary(fp_goof, k2);
    writebinary(fp_goof, k3);
#endif // THERMAL
}

void OrthoExMaterial::output(FILE* fp_goof,  unsigned char &flag,
			 int n1, int n2, int n3,
			   int same_element) const
{
    MaterialT::output(fp_goof, flag, n1, n2, n3, same_element);
    writebinary(fp_goof, float(orientation.alpha()));
    writebinary(fp_goof, float(orientation.beta()));
    writebinary(fp_goof, float(orientation.gamma()));
    writebinary(fp_goof, flag);
    writebinary(fp_goof, same_element);
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, a1);
    writebinary(fp_goof, a2);
    writebinary(fp_goof, a3);
#ifdef THERMAL
    writebinary(fp_goof, k1);
    writebinary(fp_goof, k2);
    writebinary(fp_goof, k3);
#endif // THERMAL
}

int OrthoExMaterial::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  OrthoExMaterial *m2 = (OrthoExMaterial*) other;
  if(poisson != m2->poisson) return 0;
  if(young != m2->young) return 0;
  if(a1 != m2->a1) return 0;
  if(a2 != m2->a2) return 0;
  if(a3 != m2->a3) return 0;
#ifdef THERMAL
  if(k1 != m2->k1) return 0;
  if(k2 != m2->k2) return 0;
  if(k3 != m2->k3) return 0;
#endif // THERMAL
  if(!(orientation == m2->orientation)) return 0;
  return 1;
}

// ---------------

std::vector<CharString> *OrthoExMaterial::print_properties(ostream &os) const {
  std::vector<CharString> *names = MaterialT::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Isotropic(name='" << pname
     << "', cijkl=IsotropicRank4TensorEnu(young=" << young
     << ", poisson=" << poisson << "))" << endl;
  names->push_back("Elasticity:Isotropic:" + pname);
  pname = newPropertyName();
  os << "OOF.LoadData.Property.Orientation(name='" << pname
     << "', angles=Abg(alpha=" << orientation.alpha()
     << ", beta=" << orientation.beta()
     << ", gamma=" << orientation.gamma() << "))" << endl;
  names->push_back("Orientation:" + pname);
  if(a1 != 0.0 || a2 != 0.0 || a3 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Orthorhombic(name='"
       << pname
       << "', alpha=OrthorhombicRank2Tensor(xx=" << a1
       << ", yy=" << a2
       << ", zz=" << a3 << "), T0="
#ifndef THERMAL
       << 0.0
#else
       << t0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Anisotropic:Orthorhombic:" + pname);
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
