// -*- C++ -*-
// $RCSfile: trigonal.h,v $
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

#ifndef TRIGONAL_H
#define TRIGONAL_H

#include "materialT.h"
#include "menuDef.h"
#include "eulerangle.h"

class TrigonalAMaterial : public MaterialT {
protected:
  static float c11_dflt, c12_dflt, c13_dflt, c14_dflt, c15_dflt,
    c33_dflt, c44_dflt;
  static float alpha11_dflt, alpha33_dflt;
  float c11, c12, c13, c14, c15, c33, c44;
  float alpha11, alpha33;
  static EulerAngle orientation_dflt;
  EulerAngle orientation;
#ifdef THERMAL
  float kappa11, kappa33;
  static float kappa11_dflt, kappa33_dflt;
#endif // THERMAL
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  TrigonalAMaterial()
    : c11(c11_dflt),
      c12(c12_dflt),
      c13(c13_dflt),
      c14(c14_dflt),
      c15(c15_dflt),
      c33(c33_dflt),
      c44(c44_dflt),
      alpha11(alpha11_dflt),
      alpha33(alpha33_dflt),
#ifdef THERMAL
      kappa11(kappa11_dflt),
      kappa33(kappa33_dflt),
#endif // THERMAL
      orientation(orientation_dflt)
  {}

  virtual void output(FILE*, unsigned char &, int n1, int n2, int n3) const;
  virtual void output(FILE*, unsigned char &, int n1, int n2, int n3,
		      int same_element) const;

  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "orientation", orientation_dflt);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c14", c14_dflt);
    AddArgument(cmd, "c15", c15_dflt);
    AddArgument(cmd, "c33", c33_dflt);
    AddArgument(cmd, "c44", c44_dflt);
    AddArgument(cmd, "alpha11", alpha11_dflt);
    AddArgument(cmd, "alpha33", alpha33_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa11", kappa11_dflt);
    AddArgument(cmd, "kappa33", kappa33_dflt);
#endif // THERMAL
  }

  virtual CharString &tag() const { return type; }
  virtual MaterialTypeRegistration *get_reg() const { return &reg; }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

class TrigonalBMaterial : public MaterialT {
protected:
  static float c11_dflt, c12_dflt, c13_dflt, c33_dflt, c44_dflt, c14_dflt;
  static float alpha11_dflt, alpha33_dflt;
  static EulerAngle orientation_dflt;
  float c11, c12, c13, c33, c44, c14;
  float alpha11, alpha33;
#ifdef THERMAL
  static float kappa11_dflt, kappa33_dflt;
  float kappa11, kappa33;
#endif // THERMAL
  EulerAngle orientation;
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  TrigonalBMaterial()
    : c11(c11_dflt),
      c12(c12_dflt),
      c13(c13_dflt),
      c33(c33_dflt),
      c44(c44_dflt),
      c14(c14_dflt),
      alpha11(alpha11_dflt),
      alpha33(alpha33_dflt),
#ifdef THERMAL
      kappa11(kappa11_dflt),
      kappa33(kappa33_dflt),
#endif // THERMAL
      orientation(orientation_dflt)
  {}

  virtual void output(FILE*, unsigned char &, int n1, int n2, int n3) const;
  virtual void output(FILE*, unsigned char &, int n1, int n2, int n3,
		      int same_element) const;

  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "orientation", orientation_dflt);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c14", c14_dflt);
    AddArgument(cmd, "c33", c33_dflt);
    AddArgument(cmd, "c44", c44_dflt);
    AddArgument(cmd, "alpha11", alpha11_dflt);
    AddArgument(cmd, "alpha33", alpha33_dflt);
#ifdef THERMAL
    AddArgument(cmd, "kappa11", kappa11_dflt);
    AddArgument(cmd, "kappa33", kappa33_dflt);
#endif // THERMAL
  }

  virtual CharString &tag() const { return type; }
  virtual MaterialTypeRegistration *get_reg() const { return &reg; }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

class TrigonalCrackMaterial : public MaterialT {
protected:
  static float c11_dflt, c12_dflt, c13_dflt, c14_dflt, c15_dflt,
    c33_dflt, c44_dflt;
  static float alpha11_dflt, alpha33_dflt;
  static EulerAngle orientation_dflt;
  static float gamma_dflt, knockdown1_dflt, knockdown2_dflt;
  static TrueFalse onlyonce_dflt;
  float c11, c12, c13, c14, c15, c33, c44;
  float alpha11, alpha33;
#ifdef THERMAL
  static float kappa11_dflt, kappa33_dflt;
  float kappa11, kappa33;
#endif // THERMAL
  EulerAngle orientation;
  float gamma, knockdown1, knockdown2;
  TrueFalse onlyonce;
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  TrigonalCrackMaterial()
    : c11(c11_dflt),
      c12(c12_dflt),
      c13(c13_dflt),
      c14(c14_dflt),
      c15(c15_dflt),
      c33(c33_dflt),
      c44(c44_dflt),
      alpha11(alpha11_dflt),
      alpha33(alpha33_dflt),
#ifdef THERMAL
      kappa11(kappa11_dflt),
      kappa33(kappa33_dflt),
#endif // THERMAL
      orientation(orientation_dflt),
      gamma(gamma_dflt),
      knockdown1(knockdown1_dflt),
      knockdown2(knockdown2_dflt),
      onlyonce(onlyonce_dflt)
  {}

  virtual void output(FILE*, unsigned char &, int n1, int n2, int n3) const;
  virtual void output(FILE*, unsigned char &, int n1, int n2, int n3,
		      int same_element) const;

  virtual void AddArgs(CommandM *cmd) {
    MaterialT::AddArgs(cmd);
    AddArgument(cmd, "orientation", orientation_dflt);
    AddArgument(cmd, "c11", c11_dflt);
    AddArgument(cmd, "c12", c12_dflt);
    AddArgument(cmd, "c13", c13_dflt);
    AddArgument(cmd, "c14", c14_dflt);
    AddArgument(cmd, "c15", c15_dflt);
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

  virtual CharString &tag() const { return type; }
  virtual MaterialTypeRegistration *get_reg() const { return &reg; }
  virtual int compare(const Material*) const;

  virtual std::vector<CharString> *print_properties(ostream&) const;
};

#endif
