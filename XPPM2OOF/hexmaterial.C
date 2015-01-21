// -*- C++ -*-
// $RCSfile: hexmaterial.C,v $
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
#include "hexmaterial.h"
#include "readbinary.h"

CharString HexMaterial::type("hexagonal");
MaterialTypeRegistration HexMaterial::reg("hexagonal",
					  new MaterialCmd<HexMaterial>,
					  new MaterialTriCmd<HexMaterial>,
					  3.0);

float HexMaterial::c11_dflt(1.0);
float HexMaterial::c12_dflt(0.0);
float HexMaterial::c13_dflt(0.0);
float HexMaterial::c33_dflt(1.0);
float HexMaterial::c44_dflt(0.5);
float HexMaterial::alpha11_dflt(0.0);
float HexMaterial::alpha33_dflt(0.0);
#ifdef THERMAL
float HexMaterial::kappa11_dflt(0.0);
float HexMaterial::kappa33_dflt(0.0);
#endif // THERMAL
EulerAngle HexMaterial::orientation_dflt;

void HexMaterial::output(FILE* fp_goof,  unsigned char &flag,
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
    writebinary(fp_goof, c33);
    writebinary(fp_goof, c44);
    writebinary(fp_goof, alpha11);
    writebinary(fp_goof, alpha33);
#ifdef THERMAL
    writebinary(fp_goof, kappa11);
    writebinary(fp_goof, kappa33);
#endif // THERMAL
}

void HexMaterial::output(FILE* fp_goof, unsigned char &flag,
			 int n1, int n2, int n3, int same_element) const
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
    writebinary(fp_goof, c33);
    writebinary(fp_goof, c44);
    writebinary(fp_goof, alpha11);
    writebinary(fp_goof, alpha33);
#ifdef THERMAL
    writebinary(fp_goof, kappa11);
    writebinary(fp_goof, kappa33);
#endif // THERMAL
}

int HexMaterial::compare(const Material *other) const {
  if(!MaterialT::compare(other)) return 0;
  HexMaterial *m2 = (HexMaterial*) other;
  if(c11 != m2->c11) return 0;
  if(c12 != m2->c12) return 0;
  if(c13 != m2->c13) return 0;
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

// --------------

std::vector<CharString> *HexMaterial::print_properties(ostream &os) const {
  std::vector<CharString> *names = MaterialT::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Elasticity.Anisotropic.Hexagonal(name='" << pname
     << "', cijkl=HexagonalRank4TensorCij(c11=" << c11
     << ", c12=" << c12
     << ", c13=" << c13
     << ", c33=" << c33
     << ", c44=" << c44 << "))" << endl;
  names->push_back("Elasticity:Anisotropic:Hexagonal:" + pname);
  pname = newPropertyName();
  os << "OOF.LoadData.Property.Orientation(name='" << pname
     << "', angles=Abg(alpha=" << orientation.alpha()
     << ", beta=" << orientation.beta()
     << ", gamma=" << orientation.gamma() << "))" << endl;
  names->push_back("Orientation:" + pname);

  if(alpha11 != 0.0 || alpha33 != 0.0) {
    pname = newPropertyName();
    os << "OOF.LoadData.Property.Thermal.ThermalExpansion.Anisotropic.Hexagonal(name='"
       << pname
       << "', alpha=HexagonalRank2Tensor(xx=" << c11
       << ", zz=" << 33 << "), T0="
#ifndef THERMAL
       << "0.0"
#else  // THERMAL
       << t0
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
