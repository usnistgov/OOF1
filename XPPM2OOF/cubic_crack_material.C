// -*- C++ -*-
// $RCSfile: cubic_crack_material.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2005-02-17 19:11:30 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Griffith element with cubic symmetry,
// based on GTElement with energy release criterion

#include "materialcmd.h"
#include "cubic_crack_material.h"
#include "readbinary.h"

CharString CubicCrackMaterial::type("cubic_crack");
MaterialTypeRegistration
   CubicCrackMaterial::reg("cubic_crack",
			   new MaterialCmd<CubicCrackMaterial>,
			   new MaterialTriCmd<CubicCrackMaterial>,
			   11.0);

float CubicCrackMaterial::young_dflt(1.0);
float CubicCrackMaterial::poisson_dflt(0.0);
float CubicCrackMaterial::t_coeff_dflt(0.0);
float CubicCrackMaterial::anisotropy_dflt(1.0);
#ifdef THERMAL
float CubicCrackMaterial::kappa_dflt(1.0);
#endif // THERMAL
float CubicCrackMaterial::knockdown1_dflt(0.1);
float CubicCrackMaterial::knockdown2_dflt(0.2);
float CubicCrackMaterial::gamma_dflt(0.1);
EulerAngle CubicCrackMaterial::orientation_dflt;
TrueFalse CubicCrackMaterial::onlyonce_dflt;

void CubicCrackMaterial::output(FILE* fp_goof,  unsigned char &flag,
			     int n1, int n2, int n3) const
{
  MaterialT::output(fp_goof, flag, n1, n2, n3);
  writebinary(fp_goof, float(orientation.alpha()));
  writebinary(fp_goof, float(orientation.beta()));
  writebinary(fp_goof, float(orientation.gamma()));
  writebinary(fp_goof, flag);
  writebinary(fp_goof, poisson);
  writebinary(fp_goof, young);
  writebinary(fp_goof, t_coeff);
  writebinary(fp_goof, anisotropy);
#ifdef THERMAL
  writebinary(fp_goof, kappa);
#endif // THERMAL
  writebinary(fp_goof, gamma);
  writebinary(fp_goof, knockdown1);
  writebinary(fp_goof, knockdown2);
  writebinary(fp_goof, onlyonce?1:0);
}

void CubicCrackMaterial::output(FILE* fp_goof,  unsigned char &flag,
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
  writebinary(fp_goof, t_coeff);
  writebinary(fp_goof, anisotropy);
#ifdef THERMAL
  writebinary(fp_goof, kappa);
#endif // THERMAL
  writebinary(fp_goof, gamma);
  writebinary(fp_goof, knockdown1);
  writebinary(fp_goof, knockdown2);
  writebinary(fp_goof, onlyonce?1:0);
}

int CubicCrackMaterial::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  CubicCrackMaterial *m2 = (CubicCrackMaterial*) other;
  if(poisson != m2->poisson) return 0;
  if(young != m2->young) return 0;
  if(t_coeff != m2->t_coeff) return 0;
  if(anisotropy != m2->anisotropy) return 0;
#ifdef THERMAL
  if(kappa != m2->kappa) return 0;
#endif // THERMAL
  if(gamma != m2->gamma) return 0;
  if(knockdown1 != m2->knockdown1) return 0;
  if(knockdown2 != m2->knockdown2) return 0;
  if(onlyonce != m2->onlyonce) return 0;
  if(!(orientation == m2->orientation)) return 0;
  return 1;
}

// ------------

std::vector<CharString> *CubicCrackMaterial::print_properties(ostream &os) const
{
  std::vector<CharString> *names = MaterialT::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Anisotropic.Cubic(name='" << pname
     << "', cijkl=CubicRank4TensorEnu(young=" << young
     << ", poisson=" << poisson
     << ", aniso=" << anisotropy << "))" << endl;
  names->push_back("Elasticity:Anisotropic:Cubic:" + pname);
  pname = newPropertyName();
  os << "OOF.LoadData.Property.Orientation(name='" << pname
     << "', angles=Abg(alpha=" << orientation.alpha()
     << ", beta=" << orientation.beta()
     << ", gamma=" << orientation.gamma() << "))" << endl;
  names->push_back("Orientation:" + pname);
  if(t_coeff != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Cubic(name='"
       << pname
       << "', alpha=CubicRank2Tensor(xx=" << t_coeff
       << "), T0="
#ifndef THERMAL
       << 0.0
#else  // THERMAL
       << t0
#endif // THERMAL
       << ")" << endl;
    names->push_back("Thermal:ThermalExpansion:Anisotropic:Cubic:" + pname);
  }
#ifdef THERMAL
  if(kappa != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.Conductivity.Anisotropic.Cubic(name='"
       << pname
       << "', kappa=CubicRank2Tensor(xx=" << kappa << "))" << endl;
    names->push_back("Thermal:Conductivity:Anisotropic:Cubic:" + pname);
  }
#endif // THERMAL
  return names;
}
