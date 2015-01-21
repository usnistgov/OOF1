// -*- C++ -*-
// $RCSfile: valeria.C,v $
// $Revision: 1.5 $
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
#include "valeria.h"
#include "readbinary.h"

CharString ValeriaMaterial::type("valeria");
MaterialTypeRegistration
ValeriaMaterial::reg("valeria",
		     new MaterialCmd<ValeriaMaterial>,
		     new MaterialTriCmd<ValeriaMaterial>,
		     13.0);

float ValeriaMaterial::young_dflt(1.0);
float ValeriaMaterial::poisson_dflt(0.0);
float ValeriaMaterial::t_coeff_dflt(0.0);
#ifdef THERMAL
float ValeriaMaterial::kappa_dflt(0.0);
#endif // THERMAL
float ValeriaMaterial::knockdown1_dflt(0.1);
float ValeriaMaterial::knockdown2_dflt(0.2);
float ValeriaMaterial::weibull_m_dflt(5.0);
float ValeriaMaterial::weibull_so_dflt(0.5);
float ValeriaMaterial::weibull_ao_dflt(0.1);
TrueFalse ValeriaMaterial::onlyonce_dflt(0);

void ValeriaMaterial::output(FILE *fp_goof, unsigned char &flag,
			     int n1, int n2, int n3) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3);
  float orient = 0;
  writebinary(fp_goof, orient);
  writebinary(fp_goof, orient);
  writebinary(fp_goof, orient);
  writebinary(fp_goof, flag);
  writebinary(fp_goof, poisson);
  writebinary(fp_goof, young);
  writebinary(fp_goof, t_coeff);
#ifdef THERMAL
  writebinary(fp_goof, kappa);
#endif // THERMAL
  writebinary(fp_goof, weibull_m);
  writebinary(fp_goof, weibull_so);
  writebinary(fp_goof, weibull_ao);
  writebinary(fp_goof, knockdown1);
  writebinary(fp_goof, knockdown2);
  writebinary(fp_goof, onlyonce?1:0);
}

void ValeriaMaterial::output(FILE *fp_goof, unsigned char &flag,
			     int n1, int n2, int n3, int same_element) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3, same_element);
  float orient = 0;
  writebinary(fp_goof, orient);
  writebinary(fp_goof, orient);
  writebinary(fp_goof, orient);
  writebinary(fp_goof, flag);
  writebinary(fp_goof, same_element);
  writebinary(fp_goof, poisson);
  writebinary(fp_goof, young);
  writebinary(fp_goof, t_coeff);
#ifdef THERMAL
  writebinary(fp_goof, kappa);
#endif // THERMAL
  writebinary(fp_goof, weibull_m);
  writebinary(fp_goof, weibull_so);
  writebinary(fp_goof, weibull_ao);
  writebinary(fp_goof, knockdown1);
  writebinary(fp_goof, knockdown2);
  writebinary(fp_goof, onlyonce?1:0);
}

int ValeriaMaterial::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  ValeriaMaterial *m2 = (ValeriaMaterial*) other;
  if(poisson != m2->poisson) return 0;
  if(young != m2->young) return 0;
  if(t_coeff != m2->t_coeff) return 0;
#ifdef THERMAL
  if(kappa != m2->kappa) return 0;
#endif // THERMAL
  if(weibull_m != m2->weibull_m) return 0;
  if(weibull_so != m2->weibull_so) return 0;
  if(weibull_ao != m2->weibull_ao) return 0;
  if(knockdown1 != m2->knockdown1) return 0;
  if(knockdown2 != m2->knockdown2) return 0;
  if(onlyonce != m2->onlyonce) return 0;
  return 1;
}

// -------------

std::vector<CharString> *ValeriaMaterial::print_properties(ostream &os) const {
  // Treated as isotropic!
  std::vector<CharString> *names = MaterialT::print_properties(os);
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
