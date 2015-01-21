// -*- C++ -*-
// $RCSfile: orthorhombicmaterial.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2004-10-22 13:51:09 $


#ifndef ORTHORHOMBICMATERIAL_H
#define ORTHORHOMBICMATERIAL_H

#include "materialT.h"
#include "menuDef.h"
#include "eulerangle.h"


class OrthoRhombicMaterial : public MaterialT {
protected:
  static float c11_dflt, c12_dflt, c13_dflt, c22_dflt, c23_dflt, 
    c33_dflt, c44_dflt, c55_dflt, c66_dflt;
  static float alpha11_dflt, alpha22_dflt, alpha33_dflt;
  float c11, c12, c13, c22, c23, c33, c44, c55, c66;
  float alpha11, alpha22, alpha33;
  static EulerAngle orientation_dflt;
  EulerAngle orientation;
#ifdef THERMAL
  float kappa11, kappa22, kappa33;
  static float kappa11_dflt, kappa22_dflt, kappa33_dflt;
#endif // THERMAL
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  
  OrthoRhombicMaterial() :
    c11(c11_dflt),
    c12(c12_dflt),
    c13(c13_dflt),
    c22(c22_dflt),
    c23(c23_dflt),
    c33(c33_dflt),
    c44(c44_dflt),
    c55(c55_dflt),
    c66(c66_dflt),
    alpha11(alpha11_dflt),
    alpha22(alpha22_dflt),
    alpha33(alpha33_dflt),
#ifdef THERMAL
    kappa11(kappa11_dflt),
    kappa22(kappa22_dflt),
    kappa33(kappa33_dflt),
#endif // THERMAL
    orientation(orientation_dflt)
      {}
  
  virtual void output(FILE*, unsigned char &,
		      int n1, int n2, int n3) const;
  
  virtual void output(FILE*, unsigned char &,
		      int n1, int n2, int n3, int same_element) const;
  
  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "orientation", orientation_dflt);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c22", c22_dflt);
    AddArgument(cmd, "c23", c23_dflt);
    AddArgument(cmd, "c33", c33_dflt);
    AddArgument(cmd, "c44", c44_dflt);
    AddArgument(cmd, "c55", c55_dflt);
    AddArgument(cmd, "c66", c66_dflt);
    AddArgument(cmd, "alpha11", alpha11_dflt);
    AddArgument(cmd, "alpha22", alpha22_dflt);
    AddArgument(cmd, "alpha33", alpha33_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa11", kappa11_dflt);
    AddArgument(cmd, "kappa22", kappa22_dflt);
    AddArgument(cmd, "kappa33", kappa33_dflt);
#endif // THERMAL
  }
  
  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};
    
#endif
