// -*- C++ -*-
// $RCSfile: material.C,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2005-02-15 20:26:03 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "inputflags.h"
#include "goof.h"
#include "pixelgroups.h"
#include "material.h"
#include "ppm2oof.h"
#include "readbinary.h"
#include "stdlib.h"

float Material::gray_dflt(0.5);
TrueFalse Material::planestrain_dflt(0);

Material *defaultmaterial = 0;

Vec<Material*> Material::allmaterials;

Vec<MaterialTypeRegistration*> &material_registry(void) {
  static Vec<MaterialTypeRegistration*> *list = 0;
  if(!list) list = new Vec<MaterialTypeRegistration*>;
  return *list;
}


void Material::output(FILE *file, unsigned char &flag, int n1, int n2, int n3)
  const
{
  if(planestrain) flag |= PLANESTRAIN;
  writebinary(file, get_reg()->registration_no());
  writebinary(file, gray);
  writebinary(file, n1);
  writebinary(file, n2);
  writebinary(file, n3);
}

void Material::output(FILE *file, unsigned char &flag, int n1, int n2, int n3,
		      int) const
{
  if(planestrain) flag |= PLANESTRAIN;
  flag |= DUPLICATE;
  writebinary(file, get_reg()->registration_no());
  writebinary(file, gray);
  writebinary(file, n1);
  writebinary(file, n2);
  writebinary(file, n3);
}

void Material::write_oof2(ostream &os, const CharString &matname) const {
  std::vector<CharString> *propnames = print_properties(os);
  os << "OOF.LoadData.Material(name='" << matname << "', properties=[";
  for(int i=0; i<propnames->size(); i++) {
    os << "'" << (*propnames)[i] << "'";
    if(i < propnames->size()-1)
      os << ", ";
  }
  os << "])" << endl;
  delete propnames;
}

std::vector<CharString> *Material::print_properties(ostream &os) const {
  std::vector<CharString> *names = new std::vector<CharString>;
  CharString colorname = newPropertyName();
  names->push_back("Color:" + colorname);
  os << "OOF.LoadData.Property.Color(name='" << colorname
     << "', color=Gray(value=" << gray << "))" << endl;
  return names;
}

int operator==(const Material &m1, const Material &m2) {
  // check tags here, so that virtual Material::compare() is only
  // called for objects of the same type.
  return (m1.tag() == m2.tag()) && m1.compare(&m2);
}

int Material::compare(const Material *other) const {
  // check base class attributes
  if(gray != other->gray) return 0;
  if(planestrain != other->planestrain) return 0;
  return 1;
}

// ------------------------------------------------------ //

static CommandFn reset_all() {
  if(goofinit())
    current_goof->reset_materials();
}

static CommandFn reset_selected() {
  if(goofinit())
    current_goof->reset_materials_selected();
}

static CommandFn query_default() {
  if(goofinit()) {
    if(defaultmaterial) {
      garcon()->msout << "The default material is:" << endl
		      << defaultmaterial->tag() + " " +
	                     defaultmaterial->parameters
		      << endl;
    }
  }
}


Menu *defaultmenu() {
  static Menu *dm = 0;
  if(dm) return dm;
  dm = new Menu("default material");
  dm->AddCommand("query", "what is the default material?", query_default);

  Vec<MaterialTypeRegistration*> &registry = material_registry();
  MaterialTypeRegistration::sort();
  for(int i=0; i<registry.capacity(); i++) {
    dm->AddCommand(registry[i]->command);
  }
  return dm;
}

Menu *materialmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("materials");
  
  menu->AddSubMenu("default", "set default material type", defaultmenu());
  menu->AddCommand("reset_all", "remove all materials", reset_all);
  menu->AddCommand("reset_selected", "remove materials from selected pixels",
		   reset_selected);
  
  Vec<MaterialTypeRegistration*> &registry = material_registry();
  MaterialTypeRegistration::sort();
  for(int i=0; i<registry.capacity(); i++)
    menu->AddCommand(registry[i]->command);
  
  return menu;
}

int MaterialTypeRegistration::sorted = 0;

void MaterialTypeRegistration::sort() {
  // sort materials in the registry by their menuposition
  if(sorted) return;
  Vec<MaterialTypeRegistration*> &registry = material_registry();
  qsort(registry, registry.capacity(), sizeof(MaterialTypeRegistration*),
	MaterialTypeRegistration::sortcompare);
  for(int i=0; i<registry.capacity(); i++)
    registry[i]->mynumber = i;
  sorted = 0;
}

int MaterialTypeRegistration::sortcompare(const void *a, const void *b) {
  MaterialTypeRegistration *aa = *(MaterialTypeRegistration**) a;
  MaterialTypeRegistration *bb = *(MaterialTypeRegistration**) b;
  if(aa->menuposition < bb->menuposition) return -1;
  if(aa->menuposition > bb->menuposition) return 1;
  return 0;
}

CharString newPropertyName() {
  static int n = 0;
  return "property" + to_charstring(++n);
}
