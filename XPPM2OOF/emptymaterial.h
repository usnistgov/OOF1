// -*- C++ -*-
// $RCSfile: emptymaterial.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-10-13 20:03:09 $


#ifndef EMPTYMATERIAL_H
#define EMPTYMATERIAL_H

#include "material.h"
#include "menuDef.h"


class EmptyMaterial : public Material {
private:
  static MaterialTypeRegistration reg;
  static CharString type;
public:
  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3) const;
  
  virtual void output(FILE*, unsigned char&,
		      int n1, int n2, int n3,
		      int same_element) const;
  
  virtual void AddArgs(CommandM *cmd) {
    Material::AddArgs(cmd);
  }
  
  virtual CharString &tag() const { return(type); }
  virtual MaterialTypeRegistration *get_reg() const { return(&reg); }
  virtual int compare(const Material*) const;
};
    
#endif
