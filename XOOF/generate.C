// -*- C++ -*-
// $RCSfile: generate.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:49 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

/* templated version of grid generation routines */

#ifndef GENERATE_C
#define GENERATE_C

#include "distort.h"
#include "grid.h"
#include "inputflags.h"
#include "generate.h"
#include "freedom.h"
#include "node.h"
#include "nodegroups.h"
#include "random.h"

/* Generate a uniform mesh of triangular elements.
 * ELEMENT is the type of element to use.
 * INTERIORNODE is the type of node for the interior.
 * BOUNDARYNODE is the type of node for the boundary.
 */

// make an element
template <class ELEMENT>
void newelement(const ELEMENT*, int &ECount, Grid *g, 
		int corner1, int corner2, int corner3)
{
  Element::index_dflt = ECount;
  Element::nodenumber_dflt.resize(3);
  Element::nodenumber_dflt[0] = corner1;
  Element::nodenumber_dflt[1] = corner2;
  Element::nodenumber_dflt[2] = corner3;
  Element::inputformatflag &= ~DUPLICATE;
  g->interiorE[ECount] = new ELEMENT(g);
  ECount++;
}

template <class ELEMENT>
void newelement(const ELEMENT*, int &ECount, Grid *g,
		  int corner1, int corner2, int corner3, int clone)
{
  Element::index_dflt = ECount;
  Element::nodenumber_dflt.resize(3);
  Element::nodenumber_dflt[0] = corner1;
  Element::nodenumber_dflt[1] = corner2;
  Element::nodenumber_dflt[2] = corner3;
  Element::inputformatflag |= DUPLICATE;
  Element::cloneindex = clone;
  g->interiorE[ECount] = new ELEMENT(g);
  ECount++;
}

template <class ELEMENT, class INTERIORNODE, class BOUNDARYNODE>
Grid *generate_uniform(const ELEMENT *elem,
		       const INTERIORNODE *, const BOUNDARYNODE *, 
		       int nx, int ny, double width, double height,
		       DiagStyle style)
{
  if(nx < 1 || ny < 1) return NULL;
    
  int Nel = 2*nx*ny;	    // number of elements to generate
  int NN = (nx+1)*(ny+1); // number of nodes

  Grid *grid = new Grid;
  grid->interiorE.resize(Nel);
  grid->node.resize(NN);
  grid->elastic_dof.initialize(2*NN);
#ifdef THERMAL
  grid->thermal_dof.initialize(NN);
#endif // THERMAL

//    /* get current values of variables in the distort menu, so that
//     * when the grid is saved it can be reconstructed properly.
//     */
//    record_distort_vars(grid);
    
  /* create default groups of nodes */
  NodeGroup *bottom = grid->getgroup("bottom", TF_TRUE);
  NodeGroup *top = grid->getgroup("top", TF_TRUE);
  NodeGroup *left = grid->getgroup("left", TF_TRUE);
  NodeGroup *right = grid->getgroup("right", TF_TRUE);
  NodeGroup *lowerleft = grid->getgroup("lowerleft", TF_TRUE);
  NodeGroup *upperleft = grid->getgroup("upperleft", TF_TRUE);
  NodeGroup *lowerright = grid->getgroup("lowerright", TF_TRUE);
  NodeGroup *upperright = grid->getgroup("upperright", TF_TRUE);
    
  /* create nodes and their degrees of freedom */
#ifdef DEBUG
  breakpoint();
#endif
  int i, j;
  int NCount = 0;
  double x, y;
  double dx = width/nx;
  double dy = height/ny;
  for(j=0, y=0; j<=ny; j++, y+=dy) {
    for(i=0, x=0; i<=nx; i++, x+=dx) {
      DegreeOfFreedom *xdof = grid->AddDoFelastic(new DegreeOfFreedom(x));
      DegreeOfFreedom *ydof = grid->AddDoFelastic(new DegreeOfFreedom(y));
#ifdef THERMAL
      //initial voltage in grid is initialized to zero
      DegreeOfFreedom *phidof = grid->AddDoFthermal(new DegreeOfFreedom(0.0));
      if(i == 0 || i == nx || j == 0 || j == ny)
	grid->node[NCount] = new BOUNDARYNODE(NCount, xdof, ydof, phidof);
      else
	grid->node[NCount] = new INTERIORNODE(NCount, xdof, ydof, phidof);
#else // !THERMAL
      if(i == 0 || i == nx || j == 0 || j == ny)
	grid->node[NCount] = new BOUNDARYNODE(NCount, xdof, ydof);
      else
	grid->node[NCount] = new INTERIORNODE(NCount, xdof, ydof);
#endif // THERMAL
	    
      // fill in node groups
      Node *newnode = grid->node[NCount];
      if(i==0) left->Add(newnode);
      if(i==nx) right->Add(newnode);
      if(j==0) bottom->Add(newnode);
      if(j==ny) top->Add(newnode);
      if(i==0 && j==0) lowerleft->Add(newnode);
      if(i==0 && j==ny) upperleft->Add(newnode);
      if(i==nx && j==0) lowerright->Add(newnode);
      if(i==nx && j==ny) upperright->Add(newnode);
	    
      NCount++;
    }
  }

  //  grid->nodes_done();
    
  // Create elements
  int ECount = 0;
  int isign, jsign;
  // elements that serve as prototypes
  int ul_proto = -1;
  int ll_proto = -1;
  int ur_proto = -1;
  int lr_proto = -1;
    
  for(j=0, jsign=-1; j<ny; j++, jsign*=-1) {
    for(i=0, isign=-1; i<nx; i++, isign*=-1) {
      int corner1 = i + (nx+1)*j;
      int corner2 = corner1 + 1;
      int corner3 = corner1 + nx + 1;
      int corner4 = corner3 + 1;

      if((style == LEANLEFT) || (style == ALTERNATE && (isign*jsign==1))
	 || (style == RANDOM && rndm() < 0.5)) {
	/*   3----4
	 *   |\   |   
	 *   | \  |
	 *   |  \ |
	 *   |   \|
	 *   1----2
	 */
	// lower left triangle
	if(ll_proto == -1) {     // create prototype
	  ll_proto = ECount;
	  newelement(elem, ECount, grid, corner1, corner2, corner3);
	}
	else                    // use prototype
	  newelement(elem, ECount, grid, corner1, corner2, corner3, ll_proto);

	// upper right triangle
	if(ur_proto == -1) {
	  ur_proto = ECount;
	  newelement(elem, ECount, grid, corner2, corner4, corner3);
	}
	else
	  newelement(elem, ECount, grid, corner2, corner4, corner3, ur_proto);
      }
      else {
	/*   3----4
	 *   |   /|   
	 *   |  / |
	 *   | /  |
	 *   |/   |
	 *   1----2
	 */
	// lower right triangle
	if(lr_proto == -1) {
	  lr_proto = ECount;
	  newelement(elem, ECount, grid, corner1, corner2, corner4);
	}
	else
	  newelement(elem, ECount, grid, corner1, corner2, corner4, lr_proto);
	// upper left triangle
	if(ul_proto == -1) {
	  ul_proto = ECount;
	  newelement(elem, ECount, grid, corner1, corner4, corner3);
	}
	else
	  newelement(elem, ECount, grid, corner1, corner4, corner3, ul_proto);
      }
    }
  }

  return grid;
}

#endif
