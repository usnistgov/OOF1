// -*- C++ -*-
// $RCSfile: materialcmd.C,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2005-07-30 20:16:09 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef MATERIALCMD_C
#define MATERIALCMD_C

#include "adaptmesh.h"
#include "goof.h"
#include "cell_coordinate.h"
#include "material.h"
#include "materialcmd.h"
#include "meshcmds.h"
#include "pixelgroups.h"

template <class TYPE>
MaterialCmd<TYPE>::MaterialCmd()
  : CommandM(TYPE::tag(), "create " + TYPE::tag() + " material")
{
  TYPE::AddArgs(this); // puts static default params in arg list
}

template <class TYPE>  
void MaterialCmd<TYPE>::func() {
  CharString invocation = garcon()->get_command() + " " + garcon()->get_args();
  if(garcon()->get_command(1) == "default") {
    if(defaultmaterial) delete defaultmaterial;
    defaultmaterial = new TYPE;
    defaultmaterial->parameters = invocation;
  }
  else {
    if(!goofinit()) return;
    // assign material properties to selected elements

    // Allocate one instance of a new Material, and check to see if
    // it's the same as a previously allocated material. If it is,
    // delete the new one and use the old one instead. It's necessary
    // to do this so that distinct materials can be distinguished
    // easily.
    Material *newmaterial = new TYPE;
    int match = 0;		// is this material the same as a previous one?
    int i;
    for(i=0; i<Material::allmaterials.capacity() && !match; i++) {
      if(*newmaterial == *Material::allmaterials[i]) {
	delete newmaterial;
	newmaterial = Material::allmaterials[i];
	match = 1;
      }
    }
    if(!match) {
      newmaterial->parameters = invocation;
      newmaterial->set_index(Material::allmaterials.capacity());
      Material::allmaterials.grow(1, newmaterial);
    }
    // make all selected pixels point to the new Material
    for(i=0; i<current_goof->selected().size(); i++) {
      Cell_coordinate which = current_goof->selected()[i];
      current_goof->assign_material(which, newmaterial);
    }
    current_goof->redraw();
  }
}

template <class TYPE>  
CommandM *MaterialCmd<TYPE>::clone() const {
  MaterialCmd<TYPE> *mc = new MaterialCmd<TYPE>;
  return mc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

template <class TYPE>
MaterialTriCmd<TYPE>::MaterialTriCmd()
  : CommandM(TYPE::tag(),
	     "assign material " + TYPE::tag() + " to selected triangles")
{
  TYPE::AddArgs(this);
}

template <class TYPE>
void MaterialTriCmd<TYPE>::func() {
  CharString invocation = garcon()->get_command() + " " + garcon()->get_args();
  if(meshexists()) {
    Material *newmaterial = new TYPE;
    int match = 0;		// is this material the same as a previous one?
    int i;
    for(i=0; i<Material::allmaterials.capacity() && !match; i++) {
      if(*newmaterial == *Material::allmaterials[i]) {
	delete newmaterial;
	newmaterial = Material::allmaterials[i];
	match = 1;
      }
    }
    if(!match) {
      newmaterial->parameters = invocation;
      newmaterial->set_index(Material::allmaterials.capacity());
      Material::allmaterials.grow(1, newmaterial);
    }
    // give newmaterial to all selected triangles
    current_goof->dup_mesh();
    current_goof->mesh()->assign_material_selected(newmaterial);
    current_goof->redraw();
  }
}

template <class TYPE>
CommandM *MaterialTriCmd<TYPE>::clone() const {
  MaterialTriCmd<TYPE> *mtc = new MaterialTriCmd<TYPE>;
  return mtc;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// include template definitions

#include "groups.C"

#endif
