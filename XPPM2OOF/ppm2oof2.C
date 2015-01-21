// -*- C++ -*-
// $RCSfile: ppm2oof2.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2005-02-17 19:11:31 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Functions for exporting OOF1 data to OOF2.

#include "adaptmesh.h"
#include "amtriangle.h"
#include "filename.h"
#include "material.h"
#ifdef THERMAL
#include "materialT.h"
#endif // THERMAL
#include "output.h"
#include "version.h"

static CharString newMaterialName() {
  static int count = 0;
  return "_material_" + to_charstring(++count);
}

void AdaptiveMesh::find_material_groups(Vec<MeshGroup*> &mgrps,
					const Vec<PixelGroup*> &pgrps) const
{
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
    AMTriangle *tri = (*this)[i];
    Material *mat = tri->resolve_material();
    // Is this material already in a group?
    bool foundit = false;
    for(int j=0; j<mgrps.capacity() && !foundit; j++) {
      if(*(*mgrps[j])[0]->resolve_material() == *mat) {
	mgrps[j]->append(tri);
	foundit = true;
      }
    }
    if(!foundit) {		// Create a new group
      // Find the name of the pixel group for this Material.
      CharString name;
      for(int j=0; j<pgrps.capacity() && !name; j++) {
	Material *othermat = goof->material[(*pgrps[j])[0]];
	if(!othermat) othermat = defaultmaterial;
	if(*othermat == *mat)
	  name = pgrps[j]->query_name();
      }
      MeshGroup *newgroup = new MeshGroup(name);
      mgrps.grow(1, newgroup);
      newgroup->append(tri);
    }
  }
}


void Goof::find_material_groups(Vec<PixelGroup*> &pgrps) {
  pgrps.resize(0);
  Cell_coordinate pixel;
  ArrayIterator iter(material);
  while(iter(pixel)) {
    bool foundit = false;
    for(int i=0; i<pgrps.capacity() && !foundit; i++) {
      Material *mat = material[pixel];
      if(!mat) 
	mat = defaultmaterial;
      Cell_coordinate &otherpxl = (*pgrps[i])[0];
      Material *othermat = material[otherpxl];
      if(!othermat)
	othermat = defaultmaterial;
      if(*mat == *othermat) {
	pgrps[i]->append(pixel);
	foundit = true;
      }
    }
    if(!foundit) {
      PixelGroup *grp = add_group(newMaterialName());
      pgrps.grow(1, grp);
      grp->append(pixel);
    }
  }
}

static void cleanPgrps(Vec<PixelGroup*> *pgrps) {
  for(int i=0; i<pgrps->capacity(); i++) {
    current_goof->remove_group((*pgrps)[i]->query_name());
//     delete (*pgrps)[i];
  }
}

// ----------------

bool Goof::check_plane_strain(const Vec<PixelGroup*> &pgrps, bool &ok) const {
  PixelGroup &pgrp = *pgrps[0];
  Material *mat = material[pgrp[0]];
  if(!mat)
    mat = defaultmaterial;
  bool plane_strain = mat->query_planestrain();
  for(int i=1; i<pgrps.capacity(); i++) {
    PixelGroup &pgrp = *pgrps[i];
    const Material *mat = material[pgrp[0]];
    if(!mat)
      mat = defaultmaterial;
    if(mat->query_planestrain() != plane_strain) {
      ok = false;
      return plane_strain;
    }
  }
  ok = true;
  return plane_strain;
}

#ifdef THERMAL
bool Goof::check_Tplanarity(const Vec<PixelGroup*> &pgrps, bool &ok) const {
  bool started = false;
  bool Tplanarity;
  for(int i=0; i<pgrps.capacity(); i++) {
    PixelGroup &pgrp = *pgrps[i];
    const Material *mat = material[pgrp[0]];
    if(!mat)
      mat = defaultmaterial;
    const MaterialT *matT = dynamic_cast<const MaterialT*>(mat);
    if(matT) {
      if(!started) {
	Tplanarity = matT->query_Tplanarity();
	started = true;
      }
      else {
	if(matT->query_Tplanarity() != Tplanarity) {
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

// ----------------

void Goof::print_materials(ostream &os, const Vec<PixelGroup*> &pgrps) const {
  os << endl;
  os << "## The materials defined in this data file come from the following"
     << endl;
  os << "## OOF1 element definitions.  The data file also contains an element"
     << endl;
  os << "## group with the same name as the material." << endl;
  for(int i=0; i<pgrps.capacity(); i++) {
    PixelGroup &grp = *pgrps[i];
    Material *mat = material[grp[0]];
    if(!mat)
      mat = defaultmaterial;
    os << "## " << grp.query_name() << ": " << mat->parameters << endl;
  }
  os << endl;
  for(int i=0; i<pgrps.capacity(); i++) {
    PixelGroup &grp = *pgrps[i];
    Material *mat = material[grp[0]];
    if(!mat)
      mat = defaultmaterial;
    mat->write_oof2(os, grp.query_name());
  }
}

// ----------------

void Goof::print_pixelgroups(ostream &os, const CharString &msName) const {
  for(int i=0; i<grouplist.capacity(); i++) {
    os << "OOF.LoadData.Microstructure.PixelGroup(microstructure='"
       << msName 
       << "', group='" << grouplist[i]->query_name()
       << "', meshable=1)" << endl;
  }
}

// ------------

void Goof::print_images(ostream &os, const CharString &msName) const {
  for(int p=0; p<gallery.capacity(); p++) {
    const NamedImage &image = *gallery[p];
    std::vector<unsigned short> pxls;
    for(int j=0; j<image.query_height(); j++) {
      for(int i=0; i<image.query_width(); i++) {
	const Color &color = image[Cell_coordinate(i,j)];
	pxls.push_back((unsigned short)255*color.query_red());
	pxls.push_back((unsigned short)255*color.query_green());
	pxls.push_back((unsigned short)255*color.query_blue());
      }
    }
    os << "OOF.LoadData.Image.New(name='" << image.name()
       << "', microstructure='" << msName
       << "', pixels=RGBData8(rgbvalues=[" << pxls[0];
    for(int i=1; i<pxls.size(); i++)
      os << ", " << pxls[i];
    os << "]))" << endl;
  }
}

// ------------

void Goof::print_categories(ostream &os , const CharString &msName, 
			    const Vec<PixelGroup*> &pgrps)
{
  categorize_pixels();
  int w = query_width();
  int h = query_height();
  os << "OOF.LoadData.Microstructure.Categories(microstructure='" << msName
     << "', categories=[";
  for(int j=0; j<h; j++) {
    os << "[";
    for(int i=0; i<w; i++) {
      os << pixelcategory[Cell_coordinate(i,j)];
      if(i < w-1)
	os << ",";
    }
    os << "]";
    if(j < h-1)
      os << ",";
  }
  os << "])" << endl;
  // Assign materials and pixel groups to catgories.
  for(int cat=0; cat<Ncategories; cat++) {
    const Cell_coordinate &pxl = categorical_pixel(cat);
    Material *mat = material[pxl];
    if(!mat)
      mat = defaultmaterial;
    // Find which material group corresponds to this material.  This
    // is bass-ackward, but Material instances don't have names in
    // ppm2oof, and we need to know the name that was assigned to the
    // Material in Goof::find_material_groups.
    bool ok = false;
    for(int i=0; i<pgrps.capacity() && !ok; i++) {
      PixelGroup &grp = *pgrps[i];
      Material *grpmat = material[grp[0]];
      if(!grpmat)
	grpmat = defaultmaterial;
      if(*grpmat == *mat) {
	os << "OOF.LoadData.Microstructure.DefineCategory.Material("
	   << "microstructure='" << msName
	   << "', category=" << cat
	   << ", material='" << grp.query_name() << "')" << endl;
      }
    }
    LinkList<PixelGroup*> pglist = pixelgrouplist[pxl];
    if(pglist.size() > 0) {
      os << "OOF.LoadData.Microstructure.DefineCategory.PixelGroups("
	 << "microstructure='" << msName
	 << "', category=" << cat
	 << ", groups=[";
      for(LinkListIterator<PixelGroup*> i=pglist.begin(); !i.end(); ++i)
	os << "'" << pglist[i]->query_name() << "',";
      os << "])" << endl;
    }
  }

  os << "OOF.LoadData.Microstructure.EndCategories(microstructure='" << msName
     << "')" << endl;
  
}

// ------------

bool Goof::write_oof2(ostream &os, const CharString &msName,
		      Vec<PixelGroup*> *pgrps)
{
  // Look for materials in the Goof, and create a PixelGroup for each
  // Material.
  if(!prepare_output()) return false;
  bool ownpgrps = false;
  if(!pgrps) {
    pgrps = new Vec<PixelGroup*>;
    ownpgrps = true;
  }
  find_material_groups(*pgrps);

  bool ok;
  bool plane_strain = check_plane_strain(*pgrps, ok);
  if(!ok) {
    garcon()->msout << ms_error << 
      "Materials must all be plane stress or all be plane strain!"
		    << endl << ms_normal;
    return false;
  }
#ifdef THERMAL
  bool Tplanarity = check_Tplanarity(*pgrps, ok);
  if(!ok) {
    garcon()->msout << ms_error << 
      "Materials must all have T_planarity=true or all have T_planarity=false!"
		    << endl << ms_normal;
    return false;
  }
#endif // THERMAL

  // Header
  os << "# OOF version 2.0" << endl;
  os << "# OOF2 data file created by PPM2OOF" << ppm2oofversion() << endl;
  os << "OOF.LoadData.FileVersion(number=1.0, format='script')" << endl;

  print_materials(os, *pgrps);

  // Create a Microstructure
  double physical_height =
    physical_width*query_height()/query_width();
  os << "OOF.LoadData.Microstructure.New(name='" << msName
     << "', size=Point(" << physical_width << ", " << physical_height
     << "), isize=iPoint(" << query_width() << ", " << query_height() << "))"
     << endl;

  // Store Images
  print_images(os, msName);
  // Create pixel groups
  print_pixelgroups(os, msName);
  // Store pixel categories
  print_categories(os, msName, *pgrps);

  if(ownpgrps) {
    cleanPgrps(pgrps);
    delete pgrps;
  }

  return true;
}

// ------------

void AdaptiveMesh::print_nodes(ostream &os, const CharString &skeletonName)
  const
{
  double dx = physical_width/goof->query_width();
  os << "OOF.LoadData.Skeleton.Nodes(skeleton='" << skeletonName
     << "', points=[";
  for(int i=0; i<nodes.capacity(); i++) {
    MeshCoord where = dx*nodes[i]->coord();
    os << "(" << where.x << ", " << where.y << "),";
  }
  os << "])" << endl;
}

void AdaptiveMesh::print_elements(ostream &os, const CharString &skeletonName)
  const 
{
  set_triangle_indices();
  os << "OOF.LoadData.Skeleton.Elements(skeleton='" << skeletonName
     << "', nodes=[";
  for(AMIterator iter(this, AMI_ALL); !iter.end(); ++iter) {
    (*this)[iter]->writeoof2(os);
    os << ",";
  }
  os << "])" << endl;
}

void AMTriangle::writeoof2(ostream &os) const {
  os << "(" << node[0]->index << "," << node[1]->index << ","
     << node[2]->index << ")";
}

static void print_nodegroup(ostream &os, const CharString &skeletonName,
			    const CharString &grpname,
			    const Vec<int> &members)
{
  os << "OOF.LoadData.Skeleton.NodeGroup(skeleton='" << skeletonName
     << "', name='" << grpname
     << "', nodes=[";
  for(int i=0; i<members.capacity(); i++)
    os << members[i] << ",";
  os << "])" << endl;
  
}

void AdaptiveMesh::print_nodegroups(ostream &os, const CharString &skeletonName)
  const
{
  Vec<int> top;
  Vec<int> btm;
  Vec<int> lft;
  Vec<int> rgt;
  // The next four lists will have only one member each, but this is
  // the easy way to code it and there's no significant performance
  // penalty for doing it the sleazy way.
  Vec<int> toplft;
  Vec<int> btmlft;
  Vec<int> toprgt;
  Vec<int> btmrgt;

  // find which nodes are on the edges
  for(int i=0; i<nodes.capacity(); i++) {
    if(nodes[i]->lft()) lft.grow(1, i);
    if(nodes[i]->rgt()) rgt.grow(1, i);
    if(nodes[i]->btm()) btm.grow(1, i);
    if(nodes[i]->top()) top.grow(1, i);
    if(nodes[i]->top() && nodes[i]->lft()) toplft.grow(1, i);
    if(nodes[i]->top() && nodes[i]->rgt()) toprgt.grow(1, i);
    if(nodes[i]->btm() && nodes[i]->lft()) btmlft.grow(1, i);
    if(nodes[i]->btm() && nodes[i]->rgt()) btmrgt.grow(1, i);
  }
  print_nodegroup(os, skeletonName, "top", top);
  print_nodegroup(os, skeletonName, "bottom", btm);
  print_nodegroup(os, skeletonName, "left", lft);
  print_nodegroup(os, skeletonName, "right", rgt);
  print_nodegroup(os, skeletonName, "topleft", toplft);
  print_nodegroup(os, skeletonName, "topright", toprgt);
  print_nodegroup(os, skeletonName, "bottomleft", btmlft);
  print_nodegroup(os, skeletonName, "bottomright", btmrgt);
}

void AdaptiveMesh::print_elementgroups(ostream &os,
				       const CharString &skeletonName,
				       const Vec<MeshGroup*> &groups)
  const 
{
  for(int g=0; g<groups.capacity(); g++) {
    const MeshGroup &group = *groups[g];
    os << "OOF.LoadData.Skeleton.ElementGroup(skeleton='" << skeletonName
       << "', name='" << group.query_name()
       << "', elements=[";
    for(int i=0; i<group.size(); i++)
      os << group[i]->index << ",";
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
     << "', name='bottomleft', constructor=PointFromNodes(group='bottomleft'))"
     << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='bottomright', constructor=PointFromNodes(group='bottomright'))"
     << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='topleft', constructor=PointFromNodes(group='topleft'))"
     << endl;
  os << "OOF.Skeleton.Boundary.Construct(skeleton='" << skeletonName
     << "', name='topright', constructor=PointFromNodes(group='topright'))"
     << endl;
}

// ------------

void AdaptiveMesh::write_oof2(ostream &os, const CharString &msName) {
  
  Vec<PixelGroup*> pgrps;
  if(!goof->write_oof2(os, msName, &pgrps))
    return;

  // Catalog the distinct material types in the mesh.  Create a
  // MeshGroup for each Material.  Use the pixel groups to name the
  // mesh groups.
  Vec<MeshGroup*> mgrps;

  find_material_groups(mgrps, pgrps);

  // Element groups are created automatically for each pixel group.
  // Pixel groups have been created for each material, so there is
  // already an element group for each material.  BUT, depending on
  // the way that pixel groups are inherited by the mesh, the existing
  // element groups don't necessarily contain the correct elements
  // (those elements sharing a material).  SO, before printing the
  // element groups, we must destroy the pixel groups for the
  // materials.
  cleanPgrps(&pgrps);
  inherit_pixel_groups();


  if(!prepare_output()) return;


  // Create a Skeleton
  CharString skeletonName = msName + ":skeleton";
  os << "OOF.LoadData.Skeleton.New(name='skeleton', microstructure='"
     << msName << "')" << endl;
  print_nodes(os, skeletonName);
  print_elements(os, skeletonName);
  print_nodegroups(os, skeletonName);

  print_elementgroups(os, skeletonName, grouplist);
  print_elementgroups(os, skeletonName, mgrps);

  make_boundaries(os, skeletonName);
}

