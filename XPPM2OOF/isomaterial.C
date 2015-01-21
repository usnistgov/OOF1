// -*- C++ -*-
// $RCSfile: isomaterial.C,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2004-10-23 00:48:47 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "isomaterial.h"
#include "materialcmd.h"
#include "readbinary.h"

CharString IsoMaterial::type("isotropic");

MaterialTypeRegistration IsoMaterial::reg("isotropic",
					  new MaterialCmd<IsoMaterial>,
					  new MaterialTriCmd<IsoMaterial>,
					  1.0);

float IsoMaterial::young_dflt(1.0);
float IsoMaterial::poisson_dflt(0.33);
float IsoMaterial::t_coeff_dflt(0.0);
#ifdef THERMAL
float IsoMaterial::kappa_dflt(0.0);
float IsoMaterial::t0_dflt(0.0);
#endif // THERMAL

void IsoMaterial::output(FILE* fp_goof,  unsigned char &flag,
			 int n1, int n2, int n3) const
{
    Material::output(fp_goof, flag, n1, n2, n3);
#ifdef THERMAL
    writebinary(fp_goof, t0);
#endif // THERMAL
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, t_coeff);
#ifdef THERMAL
    writebinary(fp_goof, kappa);
#endif // THERMAL
    writebinary(fp_goof, flag);
}

void IsoMaterial::output(FILE* fp_goof,  unsigned char &flag,
			 int n1, int n2, int n3, int same_element_type) const
{
    Material::output(fp_goof, flag, n1, n2, n3, same_element_type);
#ifdef THERMAL
    writebinary(fp_goof, t0);
#endif // THERMAL
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, t_coeff);
#ifdef THERMAL
    writebinary(fp_goof, kappa);
#endif // THERMAL
    writebinary(fp_goof, flag);
    writebinary(fp_goof,same_element_type);
}

int IsoMaterial::compare(const Material *other) const {
  if(!Material::compare(other)) return 0;
  IsoMaterial *m2 = (IsoMaterial*) other;
  if(poisson != m2->poisson) return 0;
  if(young != m2->young) return 0;
  if(t_coeff != m2->t_coeff) return 0;
#ifdef THERMAL
  if(t0 != m2->t0) return 0;
  if(kappa != m2->kappa) return 0;
#endif // THERMAL
  return 1;
}

// ----------

std::vector<CharString> *IsoMaterial::print_properties(ostream &os) const
{
  std::vector<CharString> *names = Material::print_properties(os);
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
       << t0
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
