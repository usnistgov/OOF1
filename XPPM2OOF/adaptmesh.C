// -*- C++ -*-
// $RCSfile: adaptmesh.C,v $
// $Revision: 1.38 $
// $Author: langer $
// $Date: 2006-01-12 16:59:53 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "adaptmesh.h"
#include "amtriangle.h"
#include "array.h"
#include "cell_coordinate.h"
#include "colorutils.h"
#include "elector.h"
#include "filename.h"
#include "goof.h"
#include "imagecanvas.h"
#include "material.h"
#include "meshcmds.h"
#include "meshgroups.h"
#include "output.h"
#include "readbinary.h"
#include "sparselink.h"
#include "version.h"
#include <stdio.h>
#include "stdlib.h"

TrueFalse meshvisible = 1;
TrueFalse meshselectvisible = 1;
TrueFalse AdaptiveMesh::continuous_redraw = 1;
double AdaptiveMesh::min_area = 0.5;
int AdaptiveMesh::max_divisions = 5;

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

AdaptiveMesh::AdaptiveMesh() :
  goof(0),
  nroots(0),
  pixels_listed(0),
  depth(0),
  periodic_x(0), periodic_y(0)
{
}

AdaptiveMesh::AdaptiveMesh(Goof *g, int nx, int ny)
  : goof(g),
    nodes(0, BlockSize(10)),
    nroots(2*nx*ny),
    root(2*nx*ny),
    pixels_listed(0),
    depth(0),
    periodic_x(g->periodic_x), periodic_y(g->periodic_y)
{
  int i, j;
  Array<AMNode*> ndarr(ny+1, nx+1); // array of nodes
  Array<AMTriangle*> upper(ny, nx); // upper left/right triangles
  Array<AMTriangle*> lower(ny, nx); // lower right/left triangles

  double width = goof->query_width()/(double) nx;   // triangle size
  double height = goof->query_height()/(double) ny; // triangle size

  // create nodes
  for(i=0; i<=nx; i++) {
    unsigned char erl = 0;  // left or right edge?
    if(i == 0)  erl = AMNode::lft_;
    if(i == nx) erl = AMNode::rgt_;
    for(j=0; j<=ny; j++) {
      unsigned char etb = 0;  // top or bottom edge?
      if(j == 0) etb = AMNode::btm_;
      if(j == ny)  etb = AMNode::top_;
	
      Cell_coordinate here(i,j);
      ndarr[here] = new AMNode(this, i*width, j*height, etb | erl);
    }
  }

  // set up root triangles. Each of the nx*ny blocks looks like this:
  //         ul ---------- ur    ul ---------- ur
  //            |       /|          |\       |   
  //            |      / |          | \      |   
  //            |upper/  |          |  \upper|   
  //            |    /   |          |   \    |   
  //            |   /    |    or    |    \   |   
  //            |  /lower|          |lower\  |   
  //            | /      |          |      \ |   
  //            |/       |          |       \|   
  //         ll ---------- lr    ll ---------- lr
  //             LEANRIGHT           LEANLEFT

  // offsets in the node and triangle arrays
  Cell_coordinate yhat(0, 1);
  Cell_coordinate xhat(1, 0);
  Cell_coordinate di(1, 1);
  const int LEANRIGHT = 1;
  const int LEANLEFT = -1;
  int nr = 0; // number of root triangles created
  int leaning;
  for(j=0; j<ny; j++) {
    leaning = (j&1? LEANLEFT : LEANRIGHT);
    for(i=0; i<nx; i++) {
      Cell_coordinate xy(i, j); // lowerleft corner of block
      if(leaning == LEANRIGHT) {
	upper[xy] = new AMTriangle(0, ndarr[xy+yhat], ndarr[xy], ndarr[xy+di]);
	lower[xy] = new AMTriangle(0, ndarr[xy+xhat], ndarr[xy+di], ndarr[xy]);
      }
      else { // leaning == LEANLEFT
	upper[xy] = new AMTriangle(0, ndarr[xy+di], ndarr[xy+yhat],
				   ndarr[xy+xhat]);
	lower[xy] = new AMTriangle(0, ndarr[xy], ndarr[xy+xhat],
				   ndarr[xy+yhat]);
      }
      upper[xy]->parent = 0;
      upper[xy]->mesh = this;
      lower[xy]->parent = 0;
      lower[xy]->mesh = this;
      root[nr++] = upper[xy];
      root[nr++] = lower[xy];
      leaning *= -1;
    }
  }
  for(j=0; j<ny; j++) {
    leaning = (j&1? LEANLEFT : LEANRIGHT);
    for(i=0; i<nx; i++) {
      Cell_coordinate xy(i, j); // lowerleft corner of block
      upper[xy]->neighbor[0] = lower[xy];
      lower[xy]->neighbor[0] = upper[xy];
      if(leaning == LEANRIGHT) {
	if(j < ny-1) upper[xy]->neighbor[1] = lower[xy+yhat];
	if(i > 0)    upper[xy]->neighbor[2] = upper[xy-xhat];
	if(j > 0)    lower[xy]->neighbor[1] = upper[xy-yhat];
	if(i < nx-1) lower[xy]->neighbor[2] = lower[xy+xhat];
      }
      else {  // leaning == LEANLEFT
	if(i < nx-1)  upper[xy]->neighbor[1] = upper[xy+xhat];
	if(j < ny-1)  upper[xy]->neighbor[2] = lower[xy+yhat];
	if(i > 0)     lower[xy]->neighbor[1] = lower[xy-xhat];
	if(j > 0)     lower[xy]->neighbor[2] = upper[xy-yhat];
      }
      leaning *= -1;
    }
  }  

  materials_need_recomputing();
  groups_need_recomputing();
  inherit_pixel_materials();
  inherit_pixel_groups();
}

AdaptiveMesh::~AdaptiveMesh() {
  int i;
  for(i=0; i<nodes.capacity(); i++)
    delete nodes[i];
  for(i=0; i<nroots; i++)
    delete root[i];
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Cell_coordinate AdaptiveMesh::mesh2cell(const MeshCoord &pt) const {
//   return Cell_coordinate(pt.x, goof->query_height() - pt.y);
// }

MeshCoord AdaptiveMesh::cellcenter(const Cell_coordinate &pt) const {
  // add (0.5, 0.5) to put the mesh point at the center of the pixel
  return MeshCoord(pt.x + 0.5, pt.y + 0.5);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

AMTriangle *
AdaptiveMesh::smallest_triangle_containing(const Cell_coordinate &pixel,
					   AMTriangle *guess)
  const 
{
  MeshCoord pt(cellcenter(pixel));
  return smallest_triangle_containing(pt, guess);
}

AMTriangle*
AdaptiveMesh::smallest_triangle_containing(const MeshCoord &pt,
					   AMTriangle *guess) const
{
  // This scheme doesn't work if the original mesh has been distorted,
  // because the child triangles don't have to lie completely within
  // the parent.
//   for(int which = 0; which<nroots; which++) {
//     if(root[which]->contains(pt)) {
//       return root[which]->child_containing(pt);
//     }
//   }
//   return 0;

  // Find a bottom-level (childless) triangle if no initial guess was provided
  AMTriangle *triangle = guess;
  if(!triangle) {
    triangle = root[0];
    while(triangle->child[0])
      triangle = triangle->child[0];
  }

  // Move from this triangle into its neighbor in the direction of pt,
  // until the triangle containing pt is found.  Roundoff error in
  // AMTriangle::contains can make it look like a point on the
  // boundary of two triangles is in neither triangle, so make sure
  // that the algorithm isn't jumping back and forth between two
  // triangles.  If it is, return one of the two.
  AMTriangle *penultimate = 0;
  AMTriangle *antepenultimate = 0;
  while(triangle && triangle != antepenultimate && !triangle->contains(pt)) {
    antepenultimate = penultimate;
    penultimate = triangle;
    triangle = triangle->neighbor_towards(pt);
  }
  if(!triangle) return 0;	// click outside mesh!
  if(!triangle->contains(pt)) {
    cerr << "Error in AdaptiveMesh::smallest_triangle_containing!" << endl;
  }
  return triangle;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Find the node closest to the given point. This should probably be
// done more efficiently.

AMNode *AdaptiveMesh::closest_node(const MeshCoord &pt) const {
  int closest = 0;
  double ddmin = sq_distance(pt, nodes[0]->coord());
  for(int i=1; i<nodes.capacity(); i++) {
    double dd = sq_distance(pt, nodes[i]->coord());
    if(dd < ddmin) {
      ddmin = dd;
      closest = i;
    }
  }
  return nodes[closest];
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AdaptiveMesh::move_node(const MeshCoord &from, const MeshCoord &to) {
  MeshCoord destination(to);
  AMNode *n = closest_node(from);
  if(n->top() || n->btm())
    destination.y = n->coord().y;
  if(n->rgt() || n->lft())
    destination.x = n->coord().x;
  n->move_to(destination);
  if(!n->areas_ok()) {		// reject moves that invert triangles
    n->move_back();
    return 0;			// unsuccessful move
  }
  return 1;			// successful move
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::draw(ImageCanvas &canvas, const Color &color, int width)
  const
{
  XSetForeground(gfxinfo.display(), canvas.gc(), color.pixel);
  XSetLineAttributes(gfxinfo.display(), canvas.gc(),
		     width, LineSolid, CapButt, JoinBevel);

  for(AMIterator i(this, AMI_ALL); !i.end(); ++i)
    (*this)[i]->draw(canvas);
}

void AdaptiveMesh::draw_selected(ImageCanvas &canvas,
				 const Color &activecolor,
				 const Color &inactivecolor) const
{
  for(AMIterator i(this, AMI_SELECTED); !i.end(); ++i) {
    AMTriangle *tri = (*this)[i];
    if(tri->active())
      tri->color = activecolor;
    else
      tri->color = inactivecolor;
    tri->fill(canvas);
  }
}

void AdaptiveMesh::draw_material(ImageCanvas &canvas) const
{
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i)
    (*this)[i]->fill(canvas);
}

void AdaptiveMesh::setcolors(const Color &dflt_color) {
 for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
    AMTriangle *tri = (*this)[i];
    Color color;
    if(tri->material()) {
      // Triangle has a material assigned. Get color from the
      // representative pixel for the triangle.
//       Cell_coordinate rep = tri->representative_material_cell();
//       color = goof->materialimage[rep];
      color = tri->material()->query_gray();
    }
    else			// Triangle has no material assigned.
      color = dflt_color;
    if(tri->active())
      tri->color = color;
    else
      tri->color = color.fade(ActiveArea::fade);
 }
}

void AdaptiveMesh::setselectedcolors(const Color &selected_color) {
  for(AMIterator i(this, AMI_SELECTED); !i.end(); ++i) {
    AMTriangle *tri = (*this)[i];
    if(tri->active())
      tri->color = selected_color;
    else
      tri->color = selected_color.fade(ActiveArea::fade);
  }
}

void AdaptiveMesh::getpixels(int nreserved, Colormap colormap) const {

  // Add each triangle's color to the colortree
  ColorTree<AMTriangle*> colortree;
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
    AMTriangle *tri = (*this)[i];
    colortree.add(tri, tri->color);
  }

  // get X pixel values for each color in the colortree
  if(gfxinfo.c_class() == PseudoColor) {
    if(restricted_colors != 0)
      colortree.reduce(restricted_colors);
    else
      colortree.reduce(gfxinfo.colormap_size() - nreserved);
    // Mkae a list of XColors
    XColor *xcolor = new XColor[colortree.Nreducedcolors()];
    int ncolors = 0;		// number of colors being set
    for(int i=0; i<colortree.Nreducedcolors(); i++) {
      ColorTreeNode<AMTriangle*> &node = colortree[i];
      if(node.elected) {
	xcolor[ncolors] = node.color.xcolor();
	xcolor[ncolors].pixel = nreserved + ncolors;
	xcolor[ncolors].flags = DoRed | DoGreen | DoBlue;
	node.color.pixel = xcolor[ncolors].pixel;
	ncolors++;
      }
    }
    XStoreColors(gfxinfo.display(), colormap, xcolor, ncolors);
    delete [] xcolor;
  }
  else if(gfxinfo.c_class() == TrueColor) {
    if(restricted_colors != 0)
      colortree.reduce(restricted_colors);
    else
      colortree.electall();
    for(int i=0; i<colortree.Nreducedcolors(); i++) {
      if(colortree[i].elected)
	colortree[i].color.RO_pixel();
    }
  }
  else {
    cerr << "AdaptiveMesh::getpixels --- bad X visual!\n" << endl;
    exit(1);
  }

  // copy pixel values from colortree to mesh triangles
  for(int ii=0; ii<colortree.Ncolors(); ii++) {
    ColorTreeNode<AMTriangle*> &node = colortree[ii];
    unsigned long pixel;
    if(node.elected)
      pixel = node.color.pixel;
    else
      pixel = colortree[node.nearest].color.pixel;
    for(LinkListIterator<AMTriangle*> j=node.list.begin(); !j.end(); ++j)
      node.list[j]->color.pixel = pixel;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::select_all() {
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i)
    if((*this)[i]->active())
      (*this)[i]->select();
}

void AdaptiveMesh::unselect_all() {
  for(AMIterator i(this, AMI_SELECTED); !i.end(); ++i)
    if((*this)[i]->active())
      (*this)[i]->unselect();
}

void AdaptiveMesh::select(const MeshGroup &grp) {
  unselect_all();
  inherit_pixel_groups();
  for(int i=0; i<grp.size(); i++)
    grp[i]->select();
}

void AdaptiveMesh::select_too(const MeshGroup &grp) {
  inherit_pixel_groups();
  for(int i=0; i<grp.size(); i++)
    grp[i]->select();
}

// unselect all triangles with fewer than 2 selected neighbors
int AdaptiveMesh::unselect_sorethumbs() {
  int total = 0;
  Vec<AMTriangle*> selectees(0, BlockSize(100));
  do {
    selectees.resize(0);
    for(AMIterator i(this, AMI_SELECTED); !i.end(); ++i)
      if((*this)[i]->active() && (*this)[i]->nnbrs_selected() < 2)
	selectees.grow(1, (*this)[i]);
    for(int j=0; j<selectees.capacity(); j++)
      selectees[j]->unselect();
    total += selectees.capacity();
  } while(selectees.capacity() > 0);
  return total;
}

// select all triangles with at least 2 selected neighbors
int AdaptiveMesh::select_dimples() {
  int total = 0;
  Vec<AMTriangle*> selectees(0, BlockSize(100));
  do {
    selectees.resize(0);
    for(AMIterator i(this, AMI_UNSELECTED); !i.end(); ++i)
      if((*this)[i]->active() && (*this)[i]->nnbrs_selected() > 1)
	selectees.grow(1, (*this)[i]);
    for(int j=0; j<selectees.capacity(); j++)
      selectees[j]->select();
    total += selectees.capacity();
  } while(selectees.capacity() > 0);
  return total;
}

int AdaptiveMesh::select_interface() {
  int n=0;
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i)
    if(!(*this)[i]->selected() &&
       (*this)[i]->active() && (*this)[i]->is_interface()) {
      n++;
      (*this)[i]->select();
    }
  return n;
}

int AdaptiveMesh::select_neighbors() {
  Group<AMTriangle*> draftees;
  for(AMIterator i(this, AMI_SELECTED); !i.end(); ++i) {
    AMTriangle *tri = (*this)[i];
    for(int j=0; j<3; j++)
      if(tri->neighbor[j] && !tri->neighbor[j]->selected())
	draftees.append(tri->neighbor[j]);
  }
  draftees.weed();
  for(int k=0; k<draftees.size(); k++)
    draftees[k]->select();
  return draftees.size();
}

// select triangles with a range of E values
int AdaptiveMesh::selectE(double min, double max) {
  unselect_all();
  int n = 0;
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
    if((*this)[i]->active()) {
      double e = (*this)[i]->E();
      if(e >= min && e <= max) {
	(*this)[i]->select();
	n++;
      }
    }
  }
  return n;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::assign_material_selected(Material *mat) {
  for(AMIterator i(this, AMI_SELECTED); !i.end(); ++i) {
    if((*this)[i]->active())
      (*this)[i]->set_material(mat, true/* inhibit automatic assignment*/);
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AdaptiveMesh::ntriangles() const {
  int count = 0;
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i)
    count++;
  return count;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::refine(int iterations, const RefinementCondition &rc) {
  int startnnodes = nodes.capacity();
  int startnelements = ntriangles();
  int lastnnodes;
  do {
    dividelist.resize(0);
    lastnnodes = nodes.capacity();
    for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
      AMTriangle *tri = (*this)[i];
      if(tri->active() && tri->area() >= min_area && rc(tri))
	tri->mark_for_division();
      tri->start_division();
    }
//      for(AMIterator j(this, AMI_ALL); !j.end(); ++j)
//        (*this)[j]->conditional_divide();
    refine_dividelist();
  } while(nodes.capacity() != lastnnodes && --iterations > 0);
  materials_need_recomputing();
  inherit_pixel_materials();
  groups_need_recomputing();
  inherit_pixel_groups();
  int endnelements = ntriangles();
  garcon()->msout << ms_info << "Mesh contains " << nodes.capacity()
		  << " nodes (" << nodes.capacity() - startnnodes
		  << " more) and " << endnelements << " elements ("
		  << endnelements - startnelements << " more)."
		  << endl << ms_normal;
}

// refine all the triangles in dividelist

void AdaptiveMesh::refine_dividelist() {
  while(dividelist.capacity() > 0) {
    int n = dividelist.capacity() - 1;
    // remove last triangle from the list ...
    AMTriangle *divideme = dividelist[n];
    dividelist.resize(n);
    // ... and divide it.
    divideme->divide();
  }
}

// refine by E function

int RefineE::operator()(AMTriangle *triangle) const {
  return triangle->E() > threshold;
}

RefinementCondition *RefineE::clone() const {
  RefineE *re = new RefineE;
  re->threshold = threshold;
  return re;
}

// refine selected triangles

int RefineSelected::operator()(AMTriangle *triangle) const {
  return triangle->selected();
}

// refine triangles on interfaces

int RefineInterface::operator()(AMTriangle *triangle) const {
  return triangle->is_interface();
}

int RefineDoubleInterface::operator()(AMTriangle *triangle) const {
  return triangle->is_double_interface();
}

// refine by depth

int RefineDepth::operator()(AMTriangle *triangle) const {
  int depth = triangle->depth();
  return (depth >= mindepth && depth <= maxdepth);
}

RefinementCondition *RefineDepth::clone() const {
  RefineDepth *rd = new RefineDepth;
  rd->mindepth = mindepth;
  rd->maxdepth = maxdepth;
  return rd;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::refine_group(int iterations, MeshGroup *grp) {
  int startnnodes = nodes.capacity();
  int startnelements = ntriangles();
  int lastnodes;
  inherit_pixel_groups();
  do {
    lastnodes = nodes.capacity();
    int i;
    for(i=0; i<grp->size(); i++)
      (*grp)[i]->mark_for_division();
    for(AMIterator it(this, AMI_ALL); !it.end(); ++it)
      (*this)[it]->start_division();
//      for(i=0; i<grp->size(); i++)
//        (*grp)[i]->conditional_divide();
    refine_dividelist();
  } while(nodes.capacity() != lastnodes && --iterations > 0);
  materials_need_recomputing();
  inherit_pixel_materials();
  groups_need_recomputing();
  inherit_pixel_groups();
  int endnelements = ntriangles();
  garcon()->msout << ms_info << "Mesh contains " << nodes.capacity()
		  << " nodes (" << nodes.capacity() - startnnodes
		  << " more) and " << endnelements << " elements ("
		  << endnelements - startnelements << " more)."
		  << endl << ms_normal;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::inherit_pixel_groups(bool forced) {
  if(forced)			// recompute even if not out of date
    ++groups_recompute_requested;
  if(groups_recompute_performed < groups_recompute_requested) {
    if(grouptransferreplace) remove_all_groups(forced);
    for(AMIterator i(this, AMI_ALL); !i.end(); ++i)
      (*this)[i]->inherit_groups(forced);
    ++groups_recompute_performed;
  }
}

void AdaptiveMesh::inherit_pixel_materials(bool forced) {
  if(forced)			// recompute even if not out of date
    ++material_recompute_requested;

  if(material_recompute_performed < material_recompute_requested) {
    for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
      (*this)[i]->inherit_material(forced);
    }
    ++material_recompute_performed;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

MeshGroup *AdaptiveMesh::add_group(const CharString &name) {
  // check for existing group with this name
  for(int i=0; i<grouplist.capacity(); i++)
    if(grouplist[i]->query_name() == name) {
      garcon()->msout << ms_error << "There is already a group named \""
		      << name << "\"!" << endl << ms_normal;
      return 0;
    }
  MeshGroup *newgroup = new MeshGroup(name);
  grouplist.grow(1, newgroup);
  return newgroup;
}

MeshGroup *AdaptiveMesh::get_group(const CharString &name) const {
  for(int i=0; i<grouplist.capacity(); i++)
    if(name == grouplist[i]->query_name())
      return grouplist[i];
  return 0;
}

MeshGroup *AdaptiveMesh::find_group(const CharString &name) {
  // check for existing group, and return it...
  for(int i=0; i<grouplist.capacity(); i++)
    if(grouplist[i]->query_name() == name)
      return grouplist[i];
  // ... otherwise, create a new group
  MeshGroup *newgroup = new MeshGroup(name);
  grouplist.grow(1, newgroup);
  return newgroup;
}

void AdaptiveMesh::remove_group(MeshGroup *deadgroup, bool forced) {
  // remove this group from each triangle's list of groups
  bool removedall = true;
  for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
    AMTriangle *tri = (*this)[i];
    if(forced || !tri->inhibit_groupinheritance)
      (*this)[i]->meshgroups.remove(deadgroup);
    else
      removedall = false;
  }

  if(removedall) {			// able to remove all triangles from the group
    // remove the group from the mesh's list of groups
    grouplist.remove(deadgroup);
    delete deadgroup;
  }
  else {			// some triangles remain in the group
    // have to actually remove triangles from the group, since the
    // group can't be removed.
    deadgroup->remove_conditional(AMTriangle::inhibittest);
  }
}

void AdaptiveMesh::remove_all_groups(bool forced) {
  for(int i=grouplist.capacity()-1; i>=0; i--) {
    remove_group(grouplist[i], forced);
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

AMIterator::AMIterator(const AdaptiveMesh *m, AMIteratorType t)
  : mesh(m),
    type(t),
    rootnumber(0),
    finished(0),
    triangle(0)
{
  if(mesh->nroots == 0)
    finished = 1;
  operator++();
}

void AMIterator::operator++() {
  if(type == AMI_ALL)
    next();
  else if(type == AMI_SELECTED) {
    do
      next();
    while(!finished && !triangle->selected());
  }
  else if(type == AMI_UNSELECTED) {
    do
      next();
    while(!finished && triangle->selected());
  }
}

void AMIterator::next() {
  if(!triangle) // just starting
    triangle = mesh->root[0];
  else {
    // do child[0] before child[1] recursively
    // move up the tree until there's a child[1] to do.
    while(triangle->parent && triangle->parent->child[1] == triangle)
      triangle = triangle->parent;
    if(triangle->parent) // didn't get to the top of the tree
      triangle = triangle->parent->child[1];
    else {
      rootnumber++;
      if(rootnumber == mesh->nroots) {
	finished = 1;
	triangle = 0;
	return;
      }
      triangle = mesh->root[rootnumber];
    }
  }
  // move down the tree to the lowest child[0]
  while(triangle->child[0])
    triangle = triangle->child[0];
}

AMTriangle *AdaptiveMesh::operator[](const AMIterator &iter) const {
  return iter.triangle;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

bool AdaptiveMesh::prepare_output() {
  if(!goof->prepare_output())
    return false;

  if(groups_recompute_performed < groups_recompute_requested ||
     groups_recompute_performed < groups_rules_changed) {
    if(garcon()->question("Pixel groups may have changed.\nTransfer to mesh?", 
			  1)) 
      inherit_pixel_groups();
  }

  if(material_recompute_performed < material_recompute_requested ||
     material_recompute_performed < material_rules_changed) {
    if(garcon()->question("Materials may have changed.\nTransfer to mesh?", 1))
      inherit_pixel_materials(1);
  }
  return 1;
}

void AdaptiveMesh::writegoof(const FileName &filename) {
  Cell_coordinate pixel;
  int i;

  if(!prepare_output()) return;

  FILE *file = fopen(filename, "w");
  if(!file) {
    garcon()->msout << ms_error << "Could not open file: " << filename << "!"
		    << endl << ms_normal;
    return;
  }

  printgoofheader(file);

  // nodes

  float dx = physical_width/goof->query_width();
  for(i=0; i<nodes.capacity(); i++) {
    writebinary(file, OutputFlags::xy_node);
    writebinary(file, OutputFlags::no_displacement);
    writebinary(file, nodes[i]->index); // node number
    writebinary(file, (float)(dx*nodes[i]->coord().x));
    writebinary(file, (float)(dx*nodes[i]->coord().y));
  }
  writebinary(file, OutputFlags::end_marker);
  
  // elements
  set_triangle_indices();
  for(AMIterator iter(this, AMI_ALL); !iter.end(); ++iter)
    (*this)[iter]->writegoof(file);
  writebinary(file, OutputFlags::end_marker);

  // node groups
  Vec<int> top;
  Vec<int> btm;
  Vec<int> lft;
  Vec<int> rgt;
  int toplft;
  int btmlft;
  int toprgt;
  int btmrgt;

  // find which nodes are on the edges
  for(i=0; i<nodes.capacity(); i++) {
    if(nodes[i]->lft()) lft.grow(1, i);
    if(nodes[i]->rgt()) rgt.grow(1, i);
    if(nodes[i]->btm()) btm.grow(1, i);
    if(nodes[i]->top()) top.grow(1, i);
    if(nodes[i]->top() && nodes[i]->lft()) toplft = i;
    if(nodes[i]->top() && nodes[i]->rgt()) toprgt = i;
    if(nodes[i]->btm() && nodes[i]->lft()) btmlft = i;
    if(nodes[i]->btm() && nodes[i]->rgt()) btmrgt = i;
  }

  // the order in which the groups are written must be the order in
  // which they are listed in printgoofheader()
  for(i=0; i<rgt.capacity(); i++)
    writebinary(file, rgt[i]);
  writebinary(file, OutputFlags::end_marker);

  for(i=0; i<lft.capacity(); i++)
    writebinary(file,lft[i]);
  writebinary(file, OutputFlags::end_marker);

  for(i=0; i<top.capacity(); i++)
    writebinary(file,top[i]);
  writebinary(file, OutputFlags::end_marker);

  for(i=0; i<btm.capacity(); i++)
    writebinary(file,btm[i]);
  writebinary(file, OutputFlags::end_marker);

  writebinary(file, toplft);
  writebinary(file, OutputFlags::end_marker);
  writebinary(file, btmlft);
  writebinary(file, OutputFlags::end_marker);
  writebinary(file, toprgt);
  writebinary(file, OutputFlags::end_marker);
  writebinary(file, btmrgt);
  writebinary(file, OutputFlags::end_marker);

  // element groups
  for(i=0; i<grouplist.capacity(); i++) {
    MeshGroup *mg = grouplist[i];
    mg->weed();
    for(int j=0; j<mg->size(); j++)
      writebinary(file, (*mg)[j]->index);
    writebinary(file, OutputFlags::end_marker);
    garcon()->msout << ms_info << "Wrote " << mg->size()
		    << " elements in group \"" << mg->query_name() << "\"."
		    << endl << ms_normal;
  }

  fclose(file);
}

void AdaptiveMesh::printgoofheader(FILE *file) {
  int i;
#ifdef THERMAL
  fputs("program = thermal_oof\n", file);
#else // !THERMAL
  fputs("program = oof\n", file);
#endif // THERMAL
  fprintf(file,"version number = 5\n");
  fprintf(file, "Nelements = %d\n", ntriangles());
  fprintf(file, "Nnodes = %d\n", nodes.capacity());
  fprintf(file,"type = b\n");
  fprintf(file,"elements\n");
  Vec<MaterialTypeRegistration*> &registry = material_registry();
  for(i=0; i<registry.capacity(); i++) {
    fprintf(file,"%s\n",registry[i]->name().charstar());
  }
  endlist(file);
  fprintf(file,"nodes\n");
  fprintf(file,"xy\n");
  endlist(file);
  fprintf(file,"nodegroups\n");
  fprintf(file,"right\n");
  fprintf(file,"left\n"); 
  fprintf(file,"top\n");
  fprintf(file,"bottom\n");
  fprintf(file,"upperleft\n");
  fprintf(file,"lowerleft\n");
  fprintf(file,"upperright\n");
  fprintf(file,"lowerright\n");
  endlist(file);
  if(goof->grouplist.capacity() > 0) {
    fprintf(file, "elementgroups\n");
    for(i = 0; i < grouplist.capacity();i++)
      fprintf(file,"%s\n", grouplist[i]->query_name().charstar());
    endlist(file);
  }
  
  endlist(file);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::set_triangle_indices() const {
  int count = 0;
  for(AMIterator iter(this, AMI_ALL); !iter.end(); ++iter)
    (*this)[iter]->index = count++;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

AdaptiveMesh *AdaptiveMesh::copy() const {
  int i;
#ifdef DEBUG
  cerr << " ---- Copying mesh ---- " << endl;
#endif
  AdaptiveMesh *newmesh = new AdaptiveMesh();
  newmesh->goof = goof;
  newmesh->nroots = nroots;
  newmesh->nodes.resize(nodes.capacity());
  newmesh->root.resize(root.capacity());
  newmesh->depth = depth;
  newmesh->material_rules_changed = material_rules_changed;
  newmesh->material_recompute_requested = material_recompute_requested;
  newmesh->material_recompute_performed = material_recompute_performed;
  newmesh->groups_rules_changed = groups_rules_changed;
  newmesh->groups_recompute_requested = groups_recompute_requested;
  newmesh->groups_recompute_performed = groups_recompute_performed;
  newmesh->periodic_x = periodic_x;
  newmesh->periodic_y = periodic_y;

  // create groups
  for(i=0; i<grouplist.capacity(); i++)
    newmesh->add_group(grouplist[i]->query_name());

  // copy nodes
  for(i=0; i<nodes.capacity(); i++)
    newmesh->nodes[i] = nodes[i]->copy();

  // copy triangles
  for(i=0; i<nroots; i++)
    newmesh->root[i] = root[i]->copy(newmesh); // recursively copies children

  newmesh->set_neighbor_pointers();

  return newmesh;
}

void AdaptiveMesh::set_neighbor_pointers() {
  // Set neighbor pointers in triangles.
  // nbr(i,j) is nonzero if nodes i and j are in the same triangle, in
  // which case it is a pointer to the first such triangle found.
  // Triangles are neighbors if they have two nodes in common.
  // Make sure that neighbor[i] is opposite node[i] within each triangle

  SparseLinkMatrix<AMTriangle*> nbr(nodes.capacity(), nodes.capacity());
  for(AMIterator iter(this, AMI_ALL); !iter.end(); ++iter) {
    AMTriangle *tri = (*this)[iter];
    for(int j=0; j<3; j++) {
      int n0 = tri->node[j]->index;
      int n1 = tri->node[(j+1)%3]->index;
      if(n0 > n1) {
	int tmp = n0;
	n0 = n1;
	n1 = tmp;
      }
      AMTriangle *neighbor = nbr(n0, n1);
      if(neighbor == 0)	// first triangle found with this node pair
	nbr(n0, n1) = tri;
      else {			// second triangle found... make neighbors
	neighbor->add_neighbor(n0, n1, tri);
	tri->add_neighbor(n0, n1, neighbor);
      }
    }
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::save(ostream &file) const {
  // write nodes
  file << nodes.capacity() << endl;
  for(int i=0; i<nodes.capacity(); i++)
    nodes[i]->save(file);
  
  // write triangles
  file << nroots << endl;
  for(int i=0; i<nroots; i++)
    root[i]->save(file);	// recursively saves subtriangles
}

// call load() after a mesh has been created with the null constructor
void AdaptiveMesh::load(istream &file) {
  int nn;
  file >> nn;
  if(nn <= 0)
    file.clear(ios::badbit | file.rdstate());
  if(!file) return;
  goof = current_goof;
  nodes.setphysicalsize(nn);
  for(int i=0; i<nn && file; i++)
    AMNode::read(this, file);	// puts node in AdaptiveMesh::nodes
  file >> nroots;
  root.resize(nroots);
  for(int i=0; i<nroots && file; i++)
    root[i] = AMTriangle::read(file, this, 0); // recursively read subtriangles
  
  set_neighbor_pointers();	// fix neighbors
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::remove_periodic_x() {

}

void AdaptiveMesh::remove_periodic_y() {

}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Sanity checking

void AdaptiveMesh::sanity() const {
  if(getenv("USER") == CharString("mloco")) {
    garcon()->msout << ms_error << "What? Are you nuts?" << endl << ms_normal;
    return;
  }
#ifdef DEBUG
  garcon()->msout.tee("sanity_check", "w");
  garcon()->msout.tee_normal(1);
  garcon()->msout.tee_error(1);
  garcon()->msout.tee_info(1);
#endif // DEBUG

  garcon()->msout << "--- Sanity Check --- " << endl;
  int ntri = ntriangles();
  garcon()->msout << "  Mesh has " << ntri << " triangles and "
		  << nodes.capacity() << " nodes. " << endl;
  set_triangle_indices();

  // check that no triangle appears twice in the list
  {
    bool ok = true;
    Vec<int> found(ntri, 0);
    for(AMIterator i(this, AMI_ALL); !i.end(); ++i)
      ++found[(*this)[i]->index];
    for(int i=0; i<found.capacity(); i++) {
      if(found[i] > 1) {
	garcon()->msout << ms_error << "ERROR   Triangle " << i
			<< " is listed " << found[i] << " times!"
			<< endl << ms_normal;
	ok = false;
      }
    }
    if(ok)
      garcon()->msout << "   No triangle is listed twice." << endl;
  }

  // check that all areas are positive
  {
    bool ok = true;
    for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
      AMTriangle *tri = (*this)[i];
      if(tri->area() <= 0.0) {
	garcon()->msout << ms_error << "ERROR   Area of triangle "
			<< tri->index << " is "
			<< tri->area() << endl << ms_normal;
	ok = false;
      }
    }
    if(ok)
      garcon()->msout << "   Areas are all positive." << endl;
  }

  // check that neighborness is reflexive
  {
    bool ok = true;
    for(AMIterator i(this, AMI_ALL); !i.end(); ++i) {
      AMTriangle *tri = (*this)[i];
      for(int j=0; j<3; j++) {	// loop over edges of tri
	AMTriangle *nbr = tri->neighbor[j]; // neighbor across edge j
	if(nbr) {
	  // is tri a neighbor of nbr?
	  bool mutual = false;
	  for(int k=0; k<3 && !mutual; k++) // loop over edges of nbr
	    if(nbr->neighbor[k] == tri) { // triangles are mutual neighbors.
	      mutual = true;
	      // Check that they share the same nodes.
	      AMNode *n1 = tri->node[(j+1)%3];
	      AMNode *n2 = tri->node[(j+2)%3];
	      AMNode *nb1 = nbr->node[(k+1)%3];
	      AMNode *nb2 = nbr->node[(k+2)%3];
	      if(n1 != nb2 || n2 != nb1) {
		ok = false;
		garcon()->msout << ms_error
				<< "ERROR   Neighboring triangles "
				<< tri->index << " and " << nbr->index
				<< " don't share the correct nodes!" << endl
				<< ms_normal;
	      }
	    }
	  if(!mutual) {
	    ok = false;
	    garcon()->msout << ms_error << "ERROR   Triangle "
			    << tri->index << " lists triangle " << nbr->index
			    << " as a neighbor, but not vice versa." << endl
			    << ms_normal;
	  }
	  if(nbr->divided()) {
	    ok = false;
	    garcon()->msout << ms_error << "ERROR   Triangle " << tri->index
			    << " has a divided neighbor " << j << endl
			    << ms_normal;
	  }
	}
      }
    }
    if(ok)
      garcon()->msout << "   Neighbors are mutual." << endl;
  }
  // check that AMTriangle::node and AMNode::triangle are consistent
  {
    bool ok = true;
    for(AMIterator i(this, AMI_ALL); !i.end(); ++i) { // loop over triangles
      // Check that each node of this triangle lists this triangle once
      AMTriangle *tri = (*this)[i];
      for(int k=0; k<3; k++) {	// loop over corners
	AMNode *corner = tri->node[k];
	int count = 0;		// how many times triangle is found in node
	for(int tt=0; tt<corner->triangle.capacity(); tt++) {
	  if(corner->triangle[tt] == tri)
	    ++count;
	}
	if(count != 1) {
	  ok = false;
	  garcon()->msout << ms_error << "ERROR   Corner " << k
			  << " of triangle " << tri->index
			  << " lists the triangle " << count << " times!"
			  << endl << ms_normal;
	}
      }
    }
    if(ok)
      garcon()->msout << "   Triangles are listed exactly once in their nodes."
		      << endl;
    ok = true;
    for(int n=0; n<nodes.capacity(); n++) { // loop over nodes
      AMNode *corner = nodes[n];
      // loop over triangles stored in a node
      for(int tt=0; tt<corner->triangle.capacity(); tt++) {
	AMTriangle *tri = corner->triangle[tt];
	if(tri->divided())
	  garcon()->msout << ms_error << "ERROR   Divided triangle "
			  << tri->index << " is listed in node " << n << "!"
			  << endl << ms_normal;
	// Make sure that this node is used in the triangle.
        int count = 0;
	for(int k=0; k<3; k++)
	  if(tri->node[k] == corner)
	    ++count;
	if(count != 1) {
	  ok = false;
	  garcon()->msout << ms_error << "ERROR   Triangle " << tri->index
			  << " uses node " << n << " " << count << " times!"
			  << endl << ms_normal;
	}
      }
    }
    if(ok)
      garcon()->msout << "   Nodes are used in every triangle they list. "
		      << endl;
  }
#ifdef DEBUG
  garcon()->msout.teestop();
#endif // DEBUG
}
