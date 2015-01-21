// -*- C++ -*-
// $RCSfile: zimmermaterial.h,v $
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


#ifndef ZIMMERMATERIAL_H
#define ZIMMERMATERIAL_H

#include "materialT.h"
#include "menuDef.h"
#include "eulerangle.h"

class ZimmerMaterial : public MaterialT {
private:
  static float c11_dflt, c12_dflt, c13_dflt, c33_dflt, c44_dflt;
  static float alpha11_dflt, alpha33_dflt;
  static float knockdown1_dflt, knockdown2_dflt, gamma_dflt;
  static TrueFalse onlyonce_dflt;
  static EulerAngle orientation_dflt;
  float c11, c12, c13, c33, c44;
  float alpha11, alpha33;
#ifdef THERMAL
  static float kappa11_dflt, kappa33_dflt;
  float kappa11, kappa33;
#endif // THERMAL
  float knockdown1, knockdown2, gamma;
  EulerAngle orientation;
  TrueFalse onlyonce;

  static MaterialTypeRegistration reg;
  static CharString type;

public:
  ZimmerMaterial() :
    c11(c11_dflt), c12(c12_dflt), c13(c13_dflt), c33(c33_dflt), c44(c44_dflt),
    alpha11(alpha11_dflt), alpha33(alpha33_dflt),
#ifdef THERMAL
    kappa11(kappa11_dflt), kappa33(kappa33_dflt),
#endif // THERMAL
    knockdown1(knockdown1_dflt), knockdown2(knockdown2_dflt),
    gamma(gamma_dflt),
    orientation(orientation_dflt),
    onlyonce(onlyonce_dflt)
    {}

  virtual void output(FILE*, unsigned char&,  int n1, int n2, int n3) const;

  virtual void output(FILE*, unsigned char&, int n1, int n2, int n3,
		      int same_element) const;

  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "orientation", orientation_dflt);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c33", c33_dflt);
    AddArgument(cmd, "c44", c44_dflt);
    AddArgument(cmd, "alpha11", alpha11_dflt);
    AddArgument(cmd, "alpha33", alpha33_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa11", kappa11_dflt);
    AddArgument(cmd, "kappa33", kappa33_dflt);
#endif // THERMAL
    AddArgument(cmd, "gamma", gamma_dflt);
    AddArgument(cmd, "knockdown1", knockdown1_dflt);
    AddArgument(cmd, "knockdown2", knockdown2_dflt);
    AddArgument(cmd, "only_once", onlyonce_dflt);
  }

  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

// As far as ppm2oof is concerned, the two zimmer materials differ
// only by name. They have the same parameter lists.

class ZimmerMaterial2 : public ZimmerMaterial {
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  virtual MaterialTypeRegistration *get_reg() const { return &reg; }
  virtual CharString &tag() const { return type; }
};

#endif
