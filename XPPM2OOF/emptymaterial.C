// -*- C++ -*-
// $RCSfile: emptymaterial.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2001-04-03 14:28:07 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "materialcmd.h"
#include "emptymaterial.h"
#include "readbinary.h"

CharString EmptyMaterial::type("empty");
MaterialTypeRegistration EmptyMaterial::reg("empty",
					    new MaterialCmd<EmptyMaterial>,
					    new MaterialTriCmd<EmptyMaterial>,
					    0.0);

void EmptyMaterial::output(FILE* fp_goof, unsigned char &flag,
			   int n1, int n2, int n3) const {
    Material::output(fp_goof, flag, n1, n2, n3);
}

void EmptyMaterial::output(FILE* fp_goof, unsigned char &flag,
			   int n1, int n2, int n3, int same) const {
    Material::output(fp_goof, flag, n1, n2, n3, same);
}

int EmptyMaterial::compare(const Material *other) const {
  // empty materials only differ by their gray value
  if(!Material::compare(other)) return 0;
  return 1;  // all empty materials are otherwise the same
}
