// -*- C++ -*-
// $RCSfile: gasmaterial.C,v $
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

#include "gasmaterial.h"
#include "materialcmd.h"
#include "readbinary.h"

#ifdef THERMAL

CharString GasMaterial::type("gas");

MaterialTypeRegistration
GasMaterial::reg("gas",
		 new MaterialCmd<GasMaterial>,
		 new MaterialTriCmd<GasMaterial>,
		 0.1);

float GasMaterial::kappa_dflt(1.0);

void GasMaterial::output(FILE* fp_goof,  unsigned char &flag,
			 int n1, int n2, int n3) const
{
    Material::output(fp_goof, flag, n1, n2, n3);
    writebinary(fp_goof, kappa);
    writebinary(fp_goof, flag);
}

void GasMaterial::output(FILE* fp_goof,  unsigned char &flag,
			 int n1, int n2, int n3, int same_element_type) const
{
    Material::output(fp_goof, flag, n1, n2, n3, same_element_type);
    writebinary(fp_goof, kappa);
    writebinary(fp_goof, flag);
    writebinary(fp_goof,same_element_type);
}

int GasMaterial::compare(const Material *other) const {
  if(!Material::compare(other)) return 0;
  GasMaterial *m2 = (GasMaterial*) other;
  if(kappa != m2->kappa) return 0;
  return 1;
}

std::vector<CharString> *GasMaterial::print_properties(ostream &os) const {
  std::vector<CharString> *names = Material::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Thermal.Conductivity.Isotropic(name='" << pname
     << "', kappa=" << kappa << ")" << endl;
  names->push_back("Thermal:Conductivity:Isotropic:" + pname);
  return names;
}

#endif // THERMAL
