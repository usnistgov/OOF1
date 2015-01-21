// -*- C++ -*-
// $RCSfile: damagematerial.C,v $
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
#include "damagematerial.h"
#include "readbinary.h"
#include "random.h"

CharString DamIsoMaterial::type("damisotropic");
MaterialTypeRegistration
    DamIsoMaterial::reg("damisotropic",
			new MaterialCmd<DamIsoMaterial>,
			new MaterialTriCmd<DamIsoMaterial>,
			5.0);

float DamIsoMaterial::young_dflt(1.0);
float DamIsoMaterial::poisson_dflt(0.33);
float DamIsoMaterial::t_coeff_dflt(0.0);
#ifdef THERMAL
float DamIsoMaterial::kappa_dflt(0.0);
float DamIsoMaterial::t0_dflt(0.0);
#endif // THERMAL
float DamIsoMaterial::maxstress_dflt(1.0);
float DamIsoMaterial::minstress_dflt(-1.0);
float DamIsoMaterial::maxknockdown_dflt(0.5);
float DamIsoMaterial::minknockdown_dflt(0.5);


void DamIsoMaterial::output(FILE* fp_goof, unsigned char &flag, 
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
    writebinary(fp_goof, maxstress);
    writebinary(fp_goof, minstress);
    writebinary(fp_goof, maxknockdown);
    writebinary(fp_goof, minknockdown);
    writebinary(fp_goof, flag);
}


void DamIsoMaterial::output(FILE* fp_goof, unsigned char &flag,
			    int n1, int n2, int n3, int same_element) const
{
    Material::output(fp_goof, flag, n1, n2, n3, same_element);
#ifdef THERMAL
    writebinary(fp_goof, t0);
#endif // THERMAL
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, t_coeff);
#ifdef THERMAL
    writebinary(fp_goof, kappa);
#endif // THERMAL
    writebinary(fp_goof, maxstress);
    writebinary(fp_goof, minstress);
    writebinary(fp_goof, maxknockdown);
    writebinary(fp_goof, minknockdown);
    writebinary(fp_goof, flag);
    writebinary(fp_goof,same_element);
}

int DamIsoMaterial::compare(const Material *other) const {
  if(!Material::compare(other)) return 0;
  DamIsoMaterial *m2 = (DamIsoMaterial*) other;
  if(poisson != m2->poisson) return 0;
  if(young != m2->young) return 0;
  if(t_coeff != m2->t_coeff) return 0;
#ifdef THERMAL
  if(kappa != m2->kappa) return 0;
  if(t0 != m2->t0) return 0;
#endif // THERMAL
  if(maxstress != m2->maxstress) return 0;
  if(minstress != m2->minstress) return 0;
  if(maxknockdown != m2->maxknockdown) return 0;
  if(minknockdown != m2->minknockdown) return 0;
  return 1;
}

//-/-\-/-/-\-/-/-\-/-/-\-/-/-\-/-/-\-/-/-\-/-/-\-/-/-\-/-/-\-/-/-\-/

Damage::Damage()
  : young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
#ifdef THERMAL
    kappa(kappa_dflt),
#endif // THERMAL
    maxstress(maxstress_dflt),
    knockdown1(knockdown1_dflt), knockdown2(knockdown2_dflt),
    onlyonce(onlyonce_dflt)
{
  maxstress += (0.5-rndm())*maxstressvar;
}

CharString Damage::type("damage");
MaterialTypeRegistration Damage::reg("damage",
				     new MaterialCmd<Damage>,
				     new MaterialTriCmd<Damage>,
				     6.0);

float Damage::young_dflt(1.0);
float Damage::poisson_dflt(0.33);
float Damage::t_coeff_dflt(0.0);
#ifdef THERMAL
float Damage::kappa_dflt(0.0);
#endif // THERMAL
float Damage::maxstress_dflt(0.1);
float Damage::knockdown1_dflt(0.5);
float Damage::knockdown2_dflt(0.5);
float Damage::maxstressvar(0.01);
TrueFalse Damage::onlyonce_dflt(0);



void Damage::output(FILE* fp_goof, unsigned char &flag,
		    int n1, int n2, int n3) const
{
    MaterialT::output(fp_goof, flag, n1, n2, n3);
    writebinary(fp_goof, float(0));  // orientation
    writebinary(fp_goof, float(0));
    writebinary(fp_goof, float(0));
    writebinary(fp_goof, flag);
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, t_coeff);
#ifdef THERMAL
    writebinary(fp_goof, kappa);
#endif // THERMAL
    writebinary(fp_goof, maxstress);
    writebinary(fp_goof, knockdown1);
    writebinary(fp_goof, knockdown2);
    writebinary(fp_goof, onlyonce?1:0);
}


void Damage::output(FILE* fp_goof, unsigned char &flag,
			 int n1, int n2, int n3, int same_element) const
{
    MaterialT::output(fp_goof, flag, n1, n2, n3, same_element);
    writebinary(fp_goof, float(0));  // orientation
    writebinary(fp_goof, float(0));
    writebinary(fp_goof, float(0));
    writebinary(fp_goof, flag);
    writebinary(fp_goof,same_element);
    writebinary(fp_goof, poisson);
    writebinary(fp_goof, young);
    writebinary(fp_goof, t_coeff);
#ifdef THERMAL
    writebinary(fp_goof, kappa);
#endif // THERMAL
    writebinary(fp_goof, maxstress);
    writebinary(fp_goof, knockdown1);
    writebinary(fp_goof, knockdown2);
    writebinary(fp_goof, onlyonce?1:0);
}

int Damage::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  Damage *m2 = (Damage*) other;
  if(maxstress != m2->maxstress) return 0;
  if(poisson != m2->poisson) return 0;
  if(young != m2->young) return 0;
  if(t_coeff != m2->t_coeff) return 0;
#ifdef THERMAL
  if(kappa != m2->kappa) return 0;
#endif // THERMAL
  if(knockdown1 != m2->knockdown1) return 0;
  if(knockdown2 != m2->knockdown2) return 0;
  if(onlyonce != m2->onlyonce) return 0;
  return 1;
}

// --------------

std::vector<CharString> *DamIsoMaterial::print_properties(ostream &os) const {
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
       << "', alpha=" << t_coeff
       << ", T0="
#ifndef THERMAL
       << 0
#else  // THERMAL
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

std::vector<CharString> *Damage::print_properties(ostream &os) const {
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
       << "', alpha=" << t_coeff
       << ", T0="
#ifndef THERMAL
       << 0
#else  // THERMAL
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


