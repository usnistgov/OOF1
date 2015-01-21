// -*- C++ -*-
// $RCSfile: orthoexmaterial.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2004-10-22 13:51:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef ORTHOEXMATERIAL_H
#define ORTHOEXMATERIAL_H

#include "materialT.h"
#include "menuDef.h"
#include "eulerangle.h"


class OrthoExMaterial : public MaterialT {
protected:
  static float young_dflt, poisson_dflt, a1_dflt, a2_dflt, a3_dflt;
  static EulerAngle orientation_dflt;
  float young, poisson, a1, a2, a3;
  EulerAngle orientation;
#ifdef THERMAL
  static float k1_dflt, k2_dflt, k3_dflt;
  float k1, k2, k3;
#endif // THERMAL
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  
  OrthoExMaterial() :
    young(young_dflt),
    poisson(poisson_dflt),
    a1(a1_dflt),
    a2(a2_dflt),
    a3(a3_dflt),
#ifdef THERMAL
    k1(k1_dflt),
    k2(k2_dflt),
    k3(k3_dflt),
#endif // THERMAL
    orientation(orientation_dflt)
  {}
  
  virtual void output(FILE*,  unsigned char &flag,
		      int n1, int n2, int n3) const;
  
  virtual void output(FILE*, unsigned char &flag,
		      int n1, int n2, int n3, int same_element) const;
  
  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "orientation", orientation_dflt);
    AddArgument(cmd, "young", young_dflt);
    AddArgument(cmd, "poisson", poisson_dflt);
    AddArgument(cmd, "a1", a1_dflt);
    AddArgument(cmd, "a2", a2_dflt);
    AddArgument(cmd, "a3", a3_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa1", k1_dflt);
    AddArgument(cmd, "kappa2", k2_dflt);
    AddArgument(cmd, "kappa3", k3_dflt);
#endif // THERMAL
  }
  
  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};
    
#endif

