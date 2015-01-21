// -*- C++ -*-
// $RCSfile: fiddlemesh.C,v $
// $Revision: 1.15 $
// $Author: langer $
// $Date: 2004-10-23 00:48:47 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

// Stuff for adjusting the node positions.

#include "adaptmesh.h"
#include "amtriangle.h"
#include "amtriangleiterator.h"
#include "fiddlemesh.h"
#include "goof.h"
#include "material.h"
#include "menuinterrupt.h"
#include "meshcmds.h"
#include "random.h"
#include "timer.h"
#include "stdlib.h"
#include <math.h>

static const double minarea = 0.01; // minimum allowed area of a triangle

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// find all the different types of pixels

void Goof::categorize_pixels() {
  // Compare time stamps. Don't do anything unless groups and
  // materials have changed since the last time the pixels were
  // categorized.
  if(pixels_categorized > groups_changed &&
     pixels_categorized > materials_changed)
    return;
  ++pixels_categorized;

  weed_all_groups();

  // keep a list of pixels with unique properties
  catalog.resize(0);

  // loop over pixels 
  Cell_coordinate pixel;
  ArrayIterator iter(material);
  while(iter(pixel)) {
    // does this pixel fit an existing category?
    int fits = 0;
    for(int i=0; i<catalog.capacity() && !fits; i++) {
      if(material[pixel] == material[catalog[i]] &&
	 pixelgrouplist[pixel] == pixelgrouplist[catalog[i]]) { // yes
	pixelcategory[pixel] = i;
	fits = 1;
      }
    }
    if(!fits) {			// a new category of pixel
      pixelcategory[pixel] = catalog.capacity();
      catalog.grow(1, pixel);
    }
  }
  Ncategories = catalog.capacity();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// The sum of E over all triangles is the function to be minimized by
// fiddling with the node positions.  E has two parts: a homogeneity
// energy that prefers homogenous triangles, and an angle energy that
// prefers fat triangles with no sharp angles.

// The area of all pixels of type i within the triangle is A*a[i],
// where A is the area of the triangle. The homogeneiety energy
// function is

// E_h = \product_i (1-a[i])*normalization

// The energy is minimized (E_h=0) if the triangle is homogeneous, so
// that one a[i] is one and the rest are zero. It it normalized so
// that its maximum value (obtained when all a[i]=1/N) is one.

double AdaptiveMesh::alpha = 0.33;

double AMTriangle::E() {
  if(AdaptiveMesh::alpha == 0.0)
    return Eangle();
  return AdaptiveMesh::alpha*Ehomogeneity() + (1-AdaptiveMesh::alpha)*Eangle();
}

double AMTriangle::Ehomogeneity() {
  // is the saved E current?
  if(mesh->goof->materials_changed < Ecalculated
     && node[0]->nodemoved < Ecalculated
     && node[1]->nodemoved < Ecalculated
     && node[2]->nodemoved < Ecalculated)
    return currentE;

  mesh->goof->categorize_pixels(); // find all pixel types
  int N = mesh->goof->Ncategories;
  if(N <= 1) return 0;
  Vec<double> ac(N, 0.0); // area of each type
  const Array<int> &pixelcategory = mesh->goof->pixelcategory;
  for(AMTriangleIterator ti(*this); !ti.end(); ++ti) {
    const Cell_coordinate pixel = (*this)[ti];
    double ai = intersection(pixel);
    ac[pixelcategory[pixel]] += ai;
  }
  
  double e = 1.0;
  double a = area();
  double invarea = 1./a;
  double norm = N/(N-1.0);	// 1/(1-1/N)
  int j;
  for(j=0; j<N; j++) {
    ac[j] *= invarea;		// convert to fractional areas
    e *= (1-ac[j])*norm;
  }
  
  lastE = currentE;
  currentE = e;
  lastEcalculated = Ecalculated;
  ++Ecalculated;
  return e;
}

void AMTriangle::revertE() {
  Ecalculated = lastEcalculated;
  currentE = lastE;
}

void AMNode::revert() {
  move_back();
  for(int i=0; i<triangle.capacity(); i++)
    triangle[i]->revertE();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Energy function that is a minimum when the triangle has no small angles.

double AMTriangle::Eangle() const {
  static const double norm = 36./sqrt(3.); // L^2/A for an equilateral triangle
  double L = 0;
  for(int i=0; i<3; i++) {
    MeshCoord edge = node[i]->coord() - node[(i+1)%3]->coord();
    L += edge.norm();
  }
  return 1.0 - area()/(L*L)*norm;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

double AMNode::E() const {
  double e = 0;
  for(int i=0; i<triangle.capacity(); i++)
    e += triangle[i]->E();
  return e;
}

double AMNode::Eangle() const {
  double e = 0;
  for(int i=0; i<triangle.capacity(); i++)
    e += triangle[i]->Eangle();
  return e;
}

double AMNode::Ehomogeneity() const {
  double e = 0;
  for(int i=0; i<triangle.capacity(); i++)
    e += triangle[i]->Ehomogeneity();
  return e;
}

double AdaptiveMesh::E() {
  double e = 0;
  for(AMIterator it(this, AMI_ALL); !it.end(); ++it)
    e += (*this)[it]->E();
  return e;
}

void AdaptiveMesh::Erange(double &min, double &max) {
  min = 2;
  max = -1;
  for(AMIterator it(this, AMI_ALL); !it.end(); ++it) {
    if((*this)[it]->active()) {
      double e = (*this)[it]->E();
      if(e > max) max = e;
      if(e < min) min = e;
    }
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AMNode::MCmove(double T, double delta, double *deltaE) {
  // Choose a random displacement with a Gaussian distribution, but
  // don't move node off of the boundaries.
  double dx, dy;
  if(lft() || rgt())
    dx = 0;
  else
    dx = gasdev()*delta;
  if(top() || btm())
    dy = 0;
  else
    dy = gasdev()*delta;
  MeshCoord disp(dx, dy);
  double estart = E();		// original energy
  move_by(disp);		// move to new position

  // Make sure that topology hasn't changed!
  for(int j=0; j<triangle.capacity(); j++)
    if(triangle[j]->area() < 0.0) {
      revert();			// oops. Move back to original position
      return 0;			// reject move
    }

  ++nodemoved;
  double efinal = E();
  *deltaE = efinal - estart;
  if(*deltaE < 0 || (T > 0 && exp(-*deltaE/T) > rndm()))  // accept the move
    return 1;
  else {			// don't accept the move
    revert();			// move node back to where it was
    return 0;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AMNode::Laplacemove(double *deltaE) {
  // find average position of neighboring nodes
  MeshCoord avg;
  int nnbr = 0;
  for(int i=0; i<triangle.capacity(); i++) {
    for(int j=0; j<3; j++)
      if(triangle[i]->node[j] != this) {
	avg += triangle[i]->node[j]->coord();
	nnbr++;
      }
  }
  avg /= nnbr;

  // don't move off of boundaries
  if(lft() || rgt())
    avg.x = coord().x;
  if(top() || btm())
    avg.y = coord().y;

  double estart = E();
  move_to(avg);
  double efinal = E();
  *deltaE = efinal - estart;
  if(*deltaE <= 0)
    return 1;
  // reject moves that increase E
  revert();
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Vec<AMNode*> AdaptiveMesh::activenodes() const {
  Vec<AMNode*> anodes;
  anodes.setphysicalsize(nodes.capacity());
  for(int i=0; i<nodes.capacity(); i++)
    if(nodes[i]->active())
      anodes.grow(1, nodes[i]);
  return anodes;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// create a list of active nodes in random order

static Vec<AMNode*> shufflenodes(const Vec<AMNode*> &nodelist) {
  Vec<AMNode*> randomnode(nodelist);
  int N = randomnode.capacity();
  for(int i=0; i<N; i++) {
    int x = int(i + rndm()*(N-i)); // pick a remaining node at random
    if(x != i) {		// swap random node with node i
      AMNode *temp = randomnode[i];
      randomnode[i] = randomnode[x];
      randomnode[x] = temp;
    }
  }
  return randomnode;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::fiddleMC(double T, double delta,
			    const Vec<AMNode*> &nodelist)
{
  Vec<AMNode*> randomnode(shufflenodes(nodelist));
  double deltaE = 0;		// cumulative change in energy

  // try to move each node
  int accepted = 0;
  int rejected = 0;
  for(int i=0; i<randomnode.capacity(); i++) { // loop over shuffled nodes
    double dE;
    if(randomnode[i]->MCmove(T, delta, &dE)) {
      deltaE += dE;
      accepted++;
      //      current_goof->redraw();
    }
    else
      rejected++;
  }
  materials_need_recomputing();
  pixels_listed = 0;		// force update of pixel lists in each triangle
  
  garcon()->msout << ms_info << "dE = " << deltaE << "  acceptance rate = "
		  << float(accepted)/(accepted + rejected) << endl
		  << ms_normal;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void AdaptiveMesh::fiddleLaplace(const Vec<AMNode*> &nodelist) {
  Vec<AMNode*> randomnode(shufflenodes(nodelist));
  double deltaE = 0;
  int accepted = 0;
  int rejected = 0;
  for(int i=0; i<randomnode.capacity(); i++) {
    double dE;
    if(randomnode[i]->Laplacemove(&dE)) {
      deltaE += dE;
      accepted++;
    }
    else
      rejected++;
  }
  materials_need_recomputing();
  pixels_listed = 0;
  garcon()->msout << ms_info << "dE = "  << deltaE << "  acceptance rate = "
		  << float(accepted)/(accepted + rejected)
		  << endl << ms_normal;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AdaptiveMesh::fiddleDownhill(double delta) {
  static const double epsilon = 1.e-5;
  MeshCoord dx(epsilon, 0);
  MeshCoord dy(0, epsilon);
  Vec<MeshCoord> gradE(nodes.capacity());
  Interrupter stop;
  double e0 = E();
  double gradmax = 0;		// maximum gradient found
  for(int i=0; i<nodes.capacity() && !stop(); i++) {
    double e0 = nodes[i]->E();
    double f=1;
    // compute dE/dx
    if(!(nodes[i]->rgt() || nodes[i]->lft())) {
      nodes[i]->move_by(dx);	// move the node
      while(!nodes[i]->areas_ok(minarea)) { // check for negative area
	nodes[i]->revert();	// move back
	f *= -0.5;		// try again with half the motion in
	nodes[i]->move_by(f*dx); // the opposite direction
      }
      gradE[i].x = (nodes[i]->E() - e0)/(f*epsilon);
      nodes[i]->revert();	// move back
    }
    // compute dE/dy
    if(!(nodes[i]->top() || nodes[i]->btm())) {
      f=1;
      nodes[i]->move_by(dy);	// move the node
      while(!nodes[i]->areas_ok(minarea)) { // check for negative area
	nodes[i]->revert();	// move back
	f *= -0.5;		// try again with half the motion in
	nodes[i]->move_by(f*dy); // the opposite direction
      }
      gradE[i].y = (nodes[i]->E() - e0)/(f*epsilon);
      nodes[i]->revert();	// move back
    }

    double g = dot(gradE[i], gradE[i]);
    if(g > gradmax) gradmax = g;
  }
  //  if(stop()) return;

  gradmax = sqrt(gradmax);
  // move all nodes
  double move = delta/gradmax;
  for(int j=0; j<nodes.capacity(); j++)
    nodes[j]->move_by(-move*gradE[j]);
  if(!areas_ok(minarea) || E() > e0) {	// reject move!
    // move all nodes back
    for(int k=0; k<nodes.capacity(); k++)
      nodes[k]->revert();
    return 0;
  }
  garcon()->msout << ms_info << "dE = " << E() - e0
		  << "  max grad = " << gradmax << endl << ms_normal;
  materials_need_recomputing();
  pixels_listed = 0;
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

//         node3                     2 1
//            /\                     /|\       
//   Change  /  \         to        / | \      
//          /    \                 /  |  \     
//         /  A   \               /   |   \    
//  node0 /________\ node2      0/    |    \0  
//        \        /             \ A' | B' /   
//         \  B   /               \   |   /    
//          \    /                 \  |  /     
//           \  /                   \ | /      
//            \/node1               1\|/2 <--- nodenumbers of new triangles
//                                     

int AdaptiveMesh::swap_edges(AMTriangle *A, AMTriangle *B,
			      AMTriangle *&Anew, AMTriangle *&Bnew)
{
  int i;
  // identify the nodes in common
  int nbr_a = 0;		// which neighbor of A is B?
  int nbr_b = 0;		// which neighbor of B is A?
  while(nbr_a < 3 && A->neighbor[nbr_a] != B) ++nbr_a;
  while(nbr_b < 3 && B->neighbor[nbr_b] != A) ++nbr_b;
  if(nbr_a == 3 || nbr_b == 3) {
    cerr << "AdaptiveMesh::swap_edges: bad pair!" << endl;
    return 0;
  }
  AMNode *node0 = A->node[(nbr_a+1)%3];
  AMNode *node1 = B->node[nbr_b];
  AMNode *node2 = B->node[(nbr_b+1)%3];
  AMNode *node3 = A->node[nbr_a];

  // check that resulting triangles will have positive areas!
  if((trianglearea(node0->coord(), node1->coord(), node3->coord()) <= minarea)||
     (trianglearea(node2->coord(), node3->coord(), node1->coord()) <= minarea))
    return 0;

  // remove old triangles from lists in the nodes
  for(i=0; i<3; i++) {
    A->node[i]->remove_triangle(A);
    B->node[i]->remove_triangle(B);
  }
  
  // create new triangles
  Anew = new AMTriangle(A->parent, node0, node1, node3);
  Bnew = new AMTriangle(B->parent, node2, node3, node1);
  Anew->mesh = this;
  Bnew->mesh = this;

  // update neighbor lists
  int nbr_a1 = (nbr_a+1)%3;
  int nbr_a2 = (nbr_a+2)%3;
  int nbr_b1 = (nbr_b+1)%3;
  int nbr_b2 = (nbr_b+2)%3;
  Anew->neighbor[0] = Bnew;
  Anew->neighbor[1] = A->neighbor[nbr_a2];
  Anew->neighbor[2] = B->neighbor[nbr_b1];
  Bnew->neighbor[0] = Anew;
  Bnew->neighbor[1] = B->neighbor[nbr_b2];
  Bnew->neighbor[2] = A->neighbor[nbr_a1];
  if(A->neighbor[nbr_a1])
    A->neighbor[nbr_a1]->replace_neighbor(A, Bnew);
  if(A->neighbor[nbr_a2])
    A->neighbor[nbr_a2]->replace_neighbor(A, Anew);
  if(B->neighbor[nbr_b1])
    B->neighbor[nbr_b1]->replace_neighbor(B, Anew);
  if(B->neighbor[nbr_b2])
    B->neighbor[nbr_b2]->replace_neighbor(B, Bnew);

  // update parents or root list
  if(A->parent) {
    if(A->parent->child[0] == A)
      A->parent->child[0] = Anew;
    else
      A->parent->child[1] = Anew;
  }
  else {			// A is a root triangle
    for(int k=0; k<nroots; k++)
      if(root[k] == A) {
	root[k] = Anew;
	break;
      }
  }

  if(B->parent) {
    if(B->parent->child[0] == B)
      B->parent->child[0] = Bnew;
    else
      B->parent->child[1] = Bnew;
  }
  else {			// B is a root triangle
    for(int k=0; k<nroots; k++)
      if(root[k] == B) {
	root[k] = Bnew;
	break;
      }
  }

  // make sure all derivative quantities will be recalculated
  Anew->representative_material_time.backdate();
  Bnew->representative_material_time.backdate(); 
  Anew->group_cell_time.backdate();
  Bnew->group_cell_time.backdate();
  Anew->meshgroups_time.backdate();
  Bnew->meshgroups_time.backdate();
  
  // Set the new triangles' inhibit_inheritance flags.  If the
  // triangles have the same material and both flags are set, then it
  // makes sense to set the flags in the new triangles.  Otherwise,
  // both new triangles should have the flags unset.
  if(A->material() && B->material() && *A->material() == *B->material() &&
     A->inhibit_inheritance && B->inhibit_inheritance)
    {
      Anew->set_material(A->material(), true/* inhibit automatic assignment*/);
      Bnew->set_material(B->material(), true/* inhibit automatic assignment*/);
    }
  else {
    Anew->inhibit_inheritance = false;
    Bnew->inhibit_inheritance = false;
  }

  return 1;
}

void AdaptiveMesh::unswap_edges(AMTriangle *old1, AMTriangle *old2,
				AMTriangle *new1, AMTriangle *new2)
{
  // Replace new1 and new2 with old1 and old2. All links in the old
  // triangles are still ok, but the links from other triangles have
  // to be restored.
  int i;
  for(i=0; i<3; i++) {
    // restore node indices
    new1->node[i]->remove_triangle(new1);
    new2->node[i]->remove_triangle(new2);
    old1->node[i]->add_triangle(old1);
    old2->node[i]->add_triangle(old2);
  }

  if(!new1->parent) root.remove(new1);
  if(!new2->parent) root.remove(new2);

  // restore links in parents
  if(old1->parent) {
    if(old1->parent->child[0] == new1 || old1->parent->child[0] == new2)
      old1->parent->child[0] = old1;
    else
      old1->parent->child[1] = old1;
  }
  else				// old1 is a root triangle
    root.grow(1, old1);

  if(old2->parent) {
    if(old2->parent->child[0] == new1 || old2->parent->child[0] == new2)
      old2->parent->child[0] = old2;
    else
      old2->parent->child[1] = old2;
  }
  else				// old2 is a root triangle
    root.grow(1, old2);

  // restore links in neighbors
  for(i=0; i<3; i++) {
    if(old1->neighbor[i] && old1->neighbor[i] != old2)
      if(!old1->neighbor[i]->replace_neighbor(new1, old1))
	old1->neighbor[i]->replace_neighbor(new2, old1);
    if(old2->neighbor[i] && old2->neighbor[i] != old1)
      if(!old2->neighbor[i]->replace_neighbor(new1, old2))
	old2->neighbor[i]->replace_neighbor(new2, old2);
  }

  // force derivative quantities to be updated
  old1->representative_material_time.backdate();
  old2->representative_material_time.backdate();
  old1->group_cell_time.backdate();
  old2->group_cell_time.backdate();
  old1->meshgroups_time.backdate();
  old2->meshgroups_time.backdate();
}

void AdaptiveMesh::test_swap() {
  // find selected triangles
  AMTriangle *t1 = 0;
  AMTriangle *t2 = 0;
  for(AMIterator i(this, AMI_SELECTED); !i.end(); ++i)
    if(!t1)
      t1 = (*this)[i];
    else if(!t2)
      t2 = (*this)[i];
    else {
      garcon()->msout << ms_error << "Please select only two triangles!"
		      << endl << ms_normal;
      return;
    }
  if(!t1 || !t2) {
    garcon()->msout << ms_error << "Please select two triangles!"
		    << endl << ms_normal;
    return;
  }

  double e0 = t1->E() + t2->E();
  
  AMTriangle *new1, *new2;
  if(swap_edges(t1, t2, new1, new2)) {
    goof->triangle_destroyed(t1);
    goof->triangle_destroyed(t2);
    delete t1;
    delete t2;
    new1->select();
    new2->select();
    materials_need_recomputing();
    inherit_pixel_materials();
    groups_need_recomputing();
    inherit_pixel_groups();
    double e1 = new1->E() + new2->E();
    garcon()->msout << ms_info << "swap succeeded!   dE=" << (e1 - e0)
		    << endl << ms_normal;
    current_goof->redraw();
  }
  else
    garcon()->msout << ms_info << "swap failed!" << endl << ms_normal;
}

static int compareT(const void *a, const void *b) {
  double ea = (*(AMTriangle**) a)->E();
  double eb = (*(AMTriangle**) b)->E();
  if(ea > eb) return -1;
  if(ea < eb) return 1;
  return 0;
}

void AdaptiveMesh::swap_worst() {
  // make a list of all triangles, sorted by decreasing energy
  Vec<AMTriangle*> tangleangle; // Naomi's word for "triangle"
  tangleangle.setphysicalsize(ntriangles());
  for(AMIterator it(this, AMI_ALL); !it.end(); ++it) {
    if((*this)[it]->active()) {
      tangleangle.grow(1, (*this)[it]); // unsorted list of active triangles
      (*this)[it]->swapped = 0; // reset flag
    }
  }
  ::qsort(&tangleangle[0], tangleangle.capacity(), sizeof(AMTriangle*),
	compareT); // sort it

  // Triangles that have been replaced can't be deleted immediately,
  // since the tangleangle list still points to them. So keep a list
  // of defunct triangles and delete them at the end.
  Vec<AMTriangle*> defunct(0, BlockSize(10));

  // look for pairs to swap
  int nswapped = 0;
  double dE = 0;
  for(int i=0; i<tangleangle.capacity(); i++) {
    if(!tangleangle[i]->swapped) {
      AMTriangle *tri0 = tangleangle[i]; // current triangle is tri0

      // Mark this triangle as swapped, even if nothing is done. If no
      // swap is performed now, it won't be done when a neighbor is
      // examined either, so marking it now saves time later.
      tri0->swapped = 1;

      // find neighbor (tri1) with highest energy
      AMTriangle *tri1 = 0;
      double en = -1.e30;	// smaller than any possible E
      for(int k=0; k<3; k++) {
	AMTriangle *nbr = tri0->neighbor[k];
	if(nbr && !nbr->swapped) {
	  if(nbr->E() > en) {
	    tri1 = nbr;
	    en = nbr->E();
	  }
	}
      }
      if(tri1) {
	// try to swap tri0 and tri1. keep the swap if the energy decreases
	double e0 = tri0->E() + tri1->E();
	AMTriangle *new0, *new1;
	if(swap_edges(tri0, tri1, new0, new1)) {
	  double e1 = new0->E() + new1->E();
	  if(e1 < e0) {		// swap succeeded in lowering energy
	    tri0->swapped = 1;
	    tri1->swapped = 1;
	    new0->swapped = 1;
	    new1->swapped = 1;
	    defunct.grow(1, tri0);
	    defunct.grow(1, tri1);
	    nswapped++;
	    dE += e1 - e0;
	  }
	  else {		// swap failed to lower the energy
	    unswap_edges(tri0, tri1, new0, new1);
	    delete new0;
	    delete new1;
	  }
	}
      }
    }
  }

  // finally delete old (swapped) triangles
  for(int j=0; j<defunct.capacity(); j++) {
    goof->triangle_destroyed(defunct[j]);
    delete defunct[j];
  }

  garcon()->msout << ms_info << "nswapped=" << nswapped
		  << " dE=" << dE << endl << ms_normal;

  materials_need_recomputing();
  inherit_pixel_materials();
  groups_need_recomputing();
  inherit_pixel_groups();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int AdaptiveMesh::areas_ok(double min) const {
  for(AMIterator it(this, AMI_ALL); !it.end(); ++it)
    if((*this)[it]->area() <= min)
      return 0;
  return 1;
}

int AMNode::areas_ok(double min) const {
  for(int i=0; i<triangle.capacity(); i++)
    if(triangle[i]->area() <= min)
      return 0;
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

FiddleCmd::FiddleCmd()
  : CommandM("anneal", "optimize mesh by simulated annealing"),
    T(0),
    delta(1.0),
    iterations(1)
{
  AddArgument(this, "T", T);
  AddArgument(this, "delta", delta);
  AddArgument(this, "iterations", iterations);
}

CommandFn FiddleCmd::func() {
  if(meshexists()) {
    Timer timer;
    Interrupter stop;
    current_goof->dup_mesh();
    Vec<AMNode*> nodelist(current_goof->mesh()->activenodes());
    for(int i=0; i<iterations && !stop(); i++) {
      current_goof->mesh()->fiddleMC(T, delta, nodelist);
      if(AdaptiveMesh::continuous_redraw)
	current_goof->redraw();
    }
    if(!AdaptiveMesh::continuous_redraw)
      current_goof->redraw();
    garcon()->msout << ms_info << timer << endl << ms_normal;
  }
}

CommandM *FiddleCmd::clone() const {
  FiddleCmd *fc = new FiddleCmd;
  fc->T = T;
  fc->delta = delta;
  fc->iterations = iterations;
  return fc;
}


FiddleDownHillCmd::FiddleDownHillCmd() 
  : CommandM("relax", "optimize mesh by relaxation"),
    delta(1),
    iterations(1)
{
  AddArgument(this, "delta", delta);
  AddArgument(this, "iterations", iterations);
}

CommandFn FiddleDownHillCmd::func() {
  if(meshexists()) {
    Interrupter stop;
    current_goof->dup_mesh();
    for(int i=0; i<iterations && !stop(); i++) {
      if(current_goof->mesh()->fiddleDownhill(delta)) {
	if(AdaptiveMesh::continuous_redraw)
	  current_goof->redraw();
      }
      else {
	garcon()->msout << ms_error
			<< "Failed to move nodes. Try reducing delta."
			<< endl << ms_normal;
	break;
      }
    }
    if(!AdaptiveMesh::continuous_redraw)
      current_goof->redraw();
  }
}

CommandM *FiddleDownHillCmd::clone() const {
  FiddleDownHillCmd *fdhc = new FiddleDownHillCmd;
  fdhc->delta = delta;
  fdhc->iterations = iterations;
  return fdhc;
}

FiddleLaplaceCmd::FiddleLaplaceCmd()
  : CommandM("smooth", "optimize mesh by Laplacian smoothing"),
    iterations(3)
{
  AddArgument(this, "iterations", iterations);
}

CommandFn FiddleLaplaceCmd::func() {
  if(meshexists()) {
    Timer timer;
    Interrupter stop;
    current_goof->dup_mesh();
    Vec<AMNode*> nodelist(current_goof->mesh()->activenodes());
    for(int i=0; i<iterations && !stop(); i++) {
      current_goof->mesh()->fiddleLaplace(nodelist);
      if(AdaptiveMesh::continuous_redraw)
	current_goof->redraw();
    }
    if(!AdaptiveMesh::continuous_redraw)
      current_goof->redraw();
    garcon()->msout << ms_info << timer << endl << ms_normal;
  }
}

CommandM *FiddleLaplaceCmd::clone() const {
  FiddleLaplaceCmd *flc = new FiddleLaplaceCmd;
  flc->iterations = iterations;
  return flc;
}

CommandFn swapedges() {
  if(meshexists()) {
    current_goof->dup_mesh();
    current_goof->mesh()->test_swap();
    current_goof->redraw();
  }
}

CommandFn swapworst() {
  if(meshexists()) {
    current_goof->dup_mesh();
    current_goof->mesh()->swap_worst();
    current_goof->redraw();
  }
}
