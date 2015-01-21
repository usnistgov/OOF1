// -*- C++ -*-
// $RCSfile: oof2oof2.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2005-02-16 16:17:19 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Functions for exporting OOF1 data to OOF2.

#include "elementgroups.h"
#include "grid.h"
#include "griditerator.h"
#include "menuDef.h"
#include "nodegroups.h"
#include "version.h"
#include "parameters.h"

//------------------------

static ElementGroup *makeMgroup(const CharString &label,
				Vec<ElementGroup*> &mgrps)
{
  ElementGroup *newgroup = new ElementGroup(label);
  mgrps.grow(1, newgroup);
  return newgroup;
}

static ElementGroup *getMgroup(const Element *el, Vec<ElementGroup*> &mgrps) {
  // loop over groups
  for(int i=0; i<mgrps.capacity(); i++)
    // compare to first element in the group.  Element equality tests
    // for the same type and same (material) parameters, not the same
    // node positions.
    if(*((*mgrps[i])[0]) == *el)
      return mgrps[i];
  return 0;
}

// generate a material name for the i^th material group
static CharString materialname(int i) {
  return "material" + to_charstring(i+1);
}

void Grid::find_material_groups(Vec<ElementGroup*> &mgrps) const {
  ElementIterator iter(EI_ALL, this);
  Element *el;
  while(el = iter()) {
    ElementGroup *grp = getMgroup(el, mgrps);
    if(grp)
      grp->Add(el);
    else {
      ElementGroup *newgroup = makeMgroup(materialname(mgrps.capacity()),mgrps);
      newgroup->Add(el);
    }
  }
}

static void print_materials(ostream &os, const Vec<ElementGroup*> &mgrps) {
  os << endl;
  os << "## The materials defined in this data file come from the following"
     << endl;
  os << "## OOF1 element definitions.  The data file also defines an element"
     << endl;
  os << "## group with the same name as the material." << endl;
  for(int i=0; i<mgrps.capacity(); i++) {
    ElementGroup &grp = *mgrps[i];
    if(grp.size() > 0) {
      Element *el = grp[0];
      os << "##" << endl << "##" << endl;
      os << "## " << grp.name() << ": " << el->tag()
	 << " gray=" << el->intrinsic_gray << " " << el->parameters() << endl;
      os << "##" << endl << "##" << endl;
    }
  }
  for(int i=0; i<mgrps.capacity(); i++) {
    ElementGroup &grp = *mgrps[i];
    if(grp.size() > 0) {
      Element *el = grp[0];
      el->print_material(os, grp.name());
    }
  }
}

//------------------------

bool Grid::check_plane_strain(bool &ok) const {
  ElementIterator iter(EI_ALL, this);
  Element *el;
  bool started = false;
  bool plane_strain;
  while(el = iter()) {
    if(!started) {
      if(el->defines_planestrain()) {
	plane_strain = el->get_planestrain();
	started = true;
      }
    }
    else {			// started
      if(el->defines_planestrain()) {
	if(el->get_planestrain() != plane_strain) {
	  ok = false;
	  return plane_strain;
	}
      }
    }
    
  }
  ok = true;
  return plane_strain;
}

#ifdef THERMAL
bool Grid::check_Tplanarity(bool &ok) const {
  ElementIterator iter(EI_ALL, this);
  Element *el;
  bool started = false;
  bool Tplanarity;
  while(el = iter()) {
    if(!started) {
      if(el->defines_Tplanarity()) {
	Tplanarity = el->get_Tplanarity();
	started = true;
      }
    }
    else {			// started
      if(el->defines_Tplanarity()) {
	if(el->get_Tplanarity() != Tplanarity) {
	  ok = false;
	  return Tplanarity;
	}
      }
    }
    
  }
  ok = true;
  return Tplanarity;
}
#endif // THERMAL

void Grid::print_nodes(ostream &os, const CharString &skeletonName) const {
  os << "OOF.LoadData.Skeleton.Nodes(skeleton='" << skeletonName
     << "', points=[" << node[0]->original();
  for(int i=1; i<node.capacity(); i++)
    os << ", " << node[i]->original();
  os << "])" << endl;
}

void Grid::print_elements(ostream &os, const CharString &skeletonName) const {
  os << "OOF.LoadData.Skeleton.Elements(skeleton='" << skeletonName
     << "', nodes=[";
  ElementIterator iter(EI_ALL, this);
  Element *el = iter();
  el->oof2write(os);
  while(el = iter()) {
    os << ",";
    el->oof2write(os);
  }
  os << "])" << endl;
}

void Grid::print_nodegroups(ostream &os, const CharString &skeletonName) const {
  for(int i=0; i<nodegroup.capacity(); i++) {
    NodeGroup *grp = nodegroup[i];
    os << "OOF.LoadData.Skeleton.NodeGroup(skeleton='" << skeletonName
       << "', name='" << grp->name()
       << "', nodes=[";
    for(int idx=0; idx<grp->size(); idx++) {
      os << (*grp)[idx]->index;
      if(idx < grp->size()-1)
	os << ", ";
    }
    os << "])" << endl;
  }
}

void Grid::print_elementgroups(ostream &os, const CharString &skeletonName,
			       const Vec<ElementGroup*> &egroups) const
{
  for(int i=0; i<egroups.capacity(); i++) {
    ElementGroup *grp = egroups[i];
    os << "OOF.LoadData.Skeleton.ElementGroup(skeleton='" << skeletonName
       << "', name='" << grp->name()
       << "', elements=[";
    for(int idx=0; idx<grp->size(); idx++) {
      os << (*grp)[idx]->index;
      if(idx < grp->size()-1)
	os << ", ";
    }
    os << "])" << endl;
  }
}

static void make_boundaries(ostream &os, const CharString &skeletonName) {
  // The top, bottom, right, and left boundaries are constructed as
  // OOF2 point boundaries, rather than OOF2 edge boundaries, because
  // that's the way to reproduce the OOF1 behavior.
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='top', constructor=PointFromNodes(group='top'))" << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='left', constructor=PointFromNodes(group='left'))" << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='bottom', constructor=PointFromNodes(group='bottom'))" << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='right', constructor=PointFromNodes(group='right'))" << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='lowerleft', constructor=PointFromNodes(group='lowerleft'))"
     << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='lowerright', constructor=PointFromNodes(group='lowerright'))"
     << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='upperleft', constructor=PointFromNodes(group='upperleft'))"
     << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='upperright', constructor=PointFromNodes(group='upperright'))"
     << endl;
}

static void assign_materials(ostream &os, const CharString &meshName,
			     const Vec<ElementGroup*> &mgrps)
{
  for(int i=0; i<mgrps.capacity(); i++) {
    ElementGroup &grp = *mgrps[i];
    Element *el = grp[0];
    os << "OOF.Mesh.Modify(mesh='" << meshName
       << "', modifier=AssignMaterial(material='" << grp.name()
       << "', target=MeshElementGroup(group='" << grp.name() << "')))" << endl;
  }
}

void Grid::print_bcs(ostream &os, const CharString &meshName) const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->print_bc(os, meshName);
}

void Grid::print_fields(ostream &os, const CharString &meshName) const 
{
  os << "OOF.LoadData.Mesh.Load_Field(mesh='" << meshName
     << "', fields=['Displacement'";
#ifdef THERMAL
  os << ", 'Temperature'";
#endif
  os << "], field_values=[";
  MeshCoord disp = node[0]->displacement();
  os << "(0, " << disp.x << "," << disp.y
#ifdef THERMAL
     << "," << node[0]->voltage()
#endif // THERMAL
     << ")";

  for(int i=1; i<node.capacity(); i++) {
    MeshCoord disp = node[i]->displacement();
    os << ", (" << i << ", " << disp.x << "," << disp.y
#ifdef THERMAL
       << "," << node[i]->voltage()
#endif // THERMAL
       << ")";
    }
  os << "])" << endl;
}

//------------------
//-------------------------
//------------------

void Grid::oof2oof2(ostream &os, const CharString &name, int width, int height)
  const
{
  bool ok;
  bool plane_strain = check_plane_strain(ok);
  if(!ok) {
    garcon()->msout << ms_error <<
      "Elements must all be plane stress or all be plane strain!"
		    << endl << ms_normal;
    return;
  }
#ifdef THERMAL
  bool Tplanarity = check_Tplanarity(ok);
  if(!ok) {
    garcon()->msout << ms_error <<
      "Elements must all have T_planarity=true or all have T_planarity=false!"
		    << endl << ms_normal;
    return;
  }
#endif // THERMAL

  // Print header
  os << "# OOF version 2.0" << endl;
  os << "# OOF2 data file created by OOF" << oofversion() << endl;
  os << "OOF.LoadData.FileVersion(number=1.0, format='script')" << endl;

  // Define materials
  Vec<ElementGroup*> mgrps;
  find_material_groups(mgrps);
  print_materials(os, mgrps);

  // Create a Microstructure
  // First, find its size
  double xmax = 0;
  double ymax = 0;
  for(int i=0; i<node.capacity(); i++) {
    MeshCoord pos = node[i]->original();
    if(pos.x > xmax) xmax = pos.x;
    if(pos.y > ymax) ymax = pos.y;
  }
  // Create the Microstructure
  os << "OOF.LoadData.Microstructure.New(name='" << name
     << "', size=Point(" << xmax << "," << ymax
     << "), isize=iPoint(" << width << "," << height
     << "))" << endl;
  // Give all pixels the same category, since we're just faking the
  // Microstructure anyway.  All elements will have explicitly
  // assigned materials.
  os << "OOF.LoadData.Microstructure.Categories(microstructure='" << name
     << "', categories=[";
  for(int j=0; j<height; j++) {
    os << "[";
    for(int i=0; i<width; i++) {
      os << "0";
      if(i < width-1)
	os << ",";
    }
    os << "]";
    if(j < height-1)
      os << ",";
  }
  os << "])" << endl;
  os << "OOF.LoadData.Microstructure.EndCategories(microstructure='" << name
     << "')" << endl;

  // Create the Skeleton
  CharString skeletonName = name + ":skeleton";
  os << "OOF.LoadData.Skeleton.New(name='skeleton', microstructure='" << name
     << "')" << endl;
  print_nodes(os, skeletonName);
  print_elements(os, skeletonName);
  print_nodegroups(os, skeletonName);
  print_elementgroups(os, skeletonName, elementgroup);
  print_elementgroups(os, skeletonName, mgrps);
  make_boundaries(os, skeletonName);

  // Create the Mesh
  CharString meshName = skeletonName + ":mesh";
  os << "OOF.Mesh.New(name='mesh', skeleton='" << skeletonName
     << "', element3='T3_3', element4='Q4_4')" << endl;
  assign_materials(os, meshName, mgrps);

  // Define and Activate Fields and Equations
  os << "OOF.Mesh.Field.Define(mesh='" << meshName
     << "', field=Displacement)" << endl;
  os << "OOF.Mesh.Field.Activate(mesh='" << meshName
     << "', field=Displacement)" << endl;
  os << "OOF.Mesh.Equation.Activate(mesh='" << meshName
     << "', equation=Force_Balance)" << endl;
  if(plane_strain)
    os << "OOF.Mesh.Field.In_Plane(mesh='" << meshName
       << "', field=Displacement)" << endl;
  else
    os << "OOF.Mesh.Equation.Activate(mesh='" << meshName
       << "', equation=Plane_Stress)" << endl;
#ifdef THERMAL
  os << "OOF.Mesh.Field.Define(mesh='" << meshName
     << "', field=Temperature)" << endl;
  os << "OOF.Mesh.Field.Activate(mesh='" << meshName
     << "', field=Temperature)" << endl;
  os << "OOF.Mesh.Equation.Activate(mesh='" << meshName
     << "', equation=Heat_Eqn)" << endl;
  if(Tplanarity)
    os << "OOF.Mesh.Field.In_Plane(mesh='" << meshName
       << "', field=Temperature)" << endl;
  else
    os << "OOF.Mesh.Equation.Activate(mesh='" << meshName
       << "', equation=Plane_Heat_Flux)" << endl;
#endif // THERMAL

  // Boundary conditions
  print_bcs(os, meshName);

  // Field Values at Nodes
  print_fields(os, meshName);
#ifndef THERMAL
  if(delta_T != 0.0) {
    os << "OOF.Mesh.Initialize(mesh='" << meshName
       << "', field=Temperature, initializer=ConstScalarFieldInit(value="
       << delta_T << "))" << endl;
  }
#endif	// !THERMAL
}

