// -*- C++ -*-
// $RCSfile: gasmaterial.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2004-10-22 13:51:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef GASMATERIAL_H
#define GASMATERIAL_H

#include "material.h"
#include "menuDef.h"

#ifdef THERMAL

// This isn't derived from MaterialT, because thermal_planarity is
// irrelevant for isotropic materials.

class GasMaterial : public Material {
protected:
  static float kappa_dflt;
  float kappa;
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
    
  GasMaterial()
    : kappa(kappa_dflt)
  {}

  virtual void output(FILE*, unsigned char &flag,
		      int n1, int n2, int n3) const;

  virtual void output(FILE*, unsigned char &flag,
		      int n1, int n2, int n3, int same_element) const;
       
  virtual void AddArgs(CommandM *cmd) {
    Material::AddArgs(cmd);
    AddArgument(cmd, "kappa", kappa_dflt);
  }
	
  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

#endif // THERMAL
    
#endif
