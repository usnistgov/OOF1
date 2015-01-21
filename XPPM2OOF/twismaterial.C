// -*- C++ -*-
// $RCSfile: twismaterial.C,v $
// $Revision: 1.5 $
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

#include "twismaterial.h"
#include "materialcmd.h"
#include "readbinary.h"

CharString TwisMaterial::type("twissel");

MaterialTypeRegistration
TwisMaterial::reg("twissel",
		  new MaterialCmd<TwisMaterial>,
		  new MaterialTriCmd<TwisMaterial>,
		  10.0);

float TwisMaterial::young_dflt(1.0);
float TwisMaterial::poisson_dflt(0.33);
float TwisMaterial::t_coeff_dflt(0.0);
float TwisMaterial::sfs_coeff_dflt(0.0);
#ifdef THERMAL
float TwisMaterial::kappa_dflt(0.0);
float TwisMaterial::t0_dflt(0.0);
#endif // THERMAL

void TwisMaterial::output(FILE* fp_goof, unsigned char &flag,
			  int n1, int n2, int n3) const
{
    Material::output(fp_goof, flag, n1, n2, n3);
#ifdef THERMAL
    writebinary(fp_goof, t0);
#endif // THERMAL
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, t_coeff);
    writebinary(fp_goof, sfs_coeff);
#ifdef THERMAL
    writebinary(fp_goof, kappa);
#endif // THERMAL
    writebinary(fp_goof, flag);
}

void TwisMaterial::output(FILE* fp_goof, unsigned char &flag,
			  int n1, int n2, int n3, int same_element_type) const
{
    Material::output(fp_goof, flag, n1, n2, n3, same_element_type);
#ifdef THERMAL
    writebinary(fp_goof, t0);
#endif // THERMAL
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, t_coeff);
    writebinary(fp_goof, sfs_coeff);
#ifdef THERMAL
    writebinary(fp_goof, kappa);
#endif // THERMAL
    writebinary(fp_goof, flag);
    writebinary(fp_goof,same_element_type);
}

int TwisMaterial::compare(const Material *other) const {
  if(!Material::compare(other)) return 0;
  TwisMaterial *m2 = (TwisMaterial*) other;
  if(poisson != m2->poisson) return 0;
  if(young != m2->young) return 0;
  if(t_coeff != m2->t_coeff) return 0;
  if(sfs_coeff != m2->sfs_coeff) return 0;
#ifdef THERMAL
  if(t0 != m2->t0) return 0;
  if(kappa != m2->kappa) return 0;
#endif // THERMAL
  return 1;
}

// -------------

std::vector<CharString> *TwisMaterial::print_properties(ostream &os) const {
  // Treated as isotropic!
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
