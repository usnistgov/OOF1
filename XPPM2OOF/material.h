// -*- C++ -*-
// $RCSfile: material.h,v $
// $Revision: 1.7 $
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

#ifndef MATERIAL_H
#define MATERIAL_H

class Material;
class MaterialTypeRegistration;

#include <stdio.h>
#include "charstring.h"
#include "menuDef.h"
#include "ppm2oof.h"
#include "vec.h"
#include <vector>


Menu *materialmenu();

/* Each derived Material class must have a static MaterialTypeRegistration.
 * All the registration forms are stored in the material_registry.
 * The material_registry is returned by a function, since if it were
 * a global object there would be no guarantee that it would be created
 * before the registrations.
 */

Vec<MaterialTypeRegistration*> &material_registry();

Menu *defaultmenu();
Menu *materialmenu();

class MaterialTypeRegistration {
private:
  CharString label;
  CommandM *command;		// for setting pixel's Material
  CommandM *tricommand;		// for setting triangle's Material
  int mynumber;
  double menuposition;
  static int sortcompare(const void*, const void*);
  static int sorted;
public:
  MaterialTypeRegistration(const CharString &nm, CommandM *cmd,
			   CommandM *tcmd, double mp)
    : label(nm), command(cmd), tricommand(tcmd), menuposition(mp)
  {
    mynumber = material_registry().grow(1, this);
    sorted = 0;
  }
  static void sort();
  const CharString &name() { return label; }
  int registration_no() { sort(); return mynumber; }
  friend Menu *materialmenu();
  friend Menu *meshmaterialmenu();
  friend Menu *defaultmenu();
};

class Material {
protected:
  static float gray_dflt;
  float gray;
  static TrueFalse planestrain_dflt;
  TrueFalse planestrain;
private:
  int index_;			// position in allmaterials list
public:
  Material() :
    gray(gray_dflt), 
    planestrain(planestrain_dflt)
  {}
  virtual ~Material() {}
  static Vec<Material*> allmaterials;
  int index() const { return index_; }
  void set_index(int i) { index_ = i; }

  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3) const;
  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3,
		      int) const;
  virtual void AddArgs(CommandM *cmd) {
    AddArgument(cmd, "gray", gray_dflt);
    AddArgument(cmd, "planestrain", planestrain_dflt);
  }
  virtual MaterialTypeRegistration *get_reg(void) const = 0;
  virtual CharString &tag(void) const = 0;
  CharString parameters;
  int query_gray() const { return int(255*gray); }
  bool query_planestrain() const { return planestrain; }

  // oof2 output stuff
  void write_oof2(ostream&, const CharString &name) const;
  virtual std::vector<CharString> *print_properties(ostream&) const;
  
  virtual int compare(const Material*) const;
  friend int operator==(const Material&, const Material&);
};


extern Material *defaultmaterial;

CharString newPropertyName();	// for oof2 output

#endif
