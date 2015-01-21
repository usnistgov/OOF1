// -*- C++ -*-
// $RCSfile: equilibrate.C,v $
// $Revision: 1.14 $
// $Author: langer $
// $Date: 2000-11-15 19:23:37 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Construct the stiffness matrices and equilibrate the displacements.
 
#include "cg.h"
#include "comprow_double.h"
#include "coord_double.h"
#include "diagpre_double.h"
#include "displacements.h"
#include "element.h"
#include "grid.h"
#include "griditerator.h"
#include "mvvd.h"
#include "node.h"
#include "nodegroups.h"
#include "preconditioner.h"
#include "random.h"
#include "stiffness.h"
#include "timer.h"
#ifdef DEBUG
#include <fstream.h>
#endif

#include "parameters.h"

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//


int Grid::equilibrate(int use_global) {	// elastic degrees of freedom only
  int status;

  // put nodes and elements in proper lists
  // don't do this more than necessary!
  // this really doesn't belong here, but it has to be done before this
  // point.
  needs_recomputed();
  shufflelists_elastic();

  apply_absolute_elastic_distortion();
 
  int n = elastic_dof.free_dof.capacity(); // degrees of freedom
  if(n == 0) {
    garcon()->msout << ms_error << "*** No degrees of freedom!"
		    << endl << ms_normal;
    return 1;
  }
  else if(verbose) {
    garcon()->msout << ms_info << "*** Solving " << n << "x" << n << " system" 
		    << endl << ms_normal;
  }
    
  Timer timer;

  // initialize vector of unknowns, using current displacements
  MV_Vector_double ui(displacements(elastic_dof.free_dof));
    
  // initialize vector of applied forces
  MV_Vector_double Fext(appliedforces(elastic_dof.free_dof));
    
  // initialize vector of boundary displacements
  MV_Vector_double ub(displacements(elastic_dof.fixed_dof));
	
  int mxitr = maxiter;	// will be modified by CG
  double tlrnce = tol;	// ditto
    
  
  if(use_global) {
    // find stiffness matrices
    make_stiffness();

    // compute right hand side
    MV_Vector_double rhs(Li() + Fext);
    if(elastic_dof.fixed_dof.capacity() > 0)  // avoid null multiplication
      rhs += -1*(Kib*ub);
    
    // solve for interior displacements
    Preconditioner *precond = preconditioner(Kii);
    status = CG(Kii, ui, rhs, *precond, mxitr, tlrnce);
    delete precond;
  }
  else {  // no global stiffness matrix
    // compute right hand side
    Stiffness kib(this, F_FREE, F_FIXED);
    MV_Vector_double rhs = -1*(kib*ub) + Li() + Fext;
    
    // solve for interior displacements
    Preconditioner *precond = preconditioner(this);
    Stiffness s(this, F_FREE, F_FREE);
    status = CG(s, ui, rhs, *precond, mxitr, tlrnce);
    delete precond;
  }
    
  // copy displacements back to degrees of freedom
  displacements(elastic_dof.free_dof, ui);
  hash_needed = 1;

  if(status) {
    garcon()->msout << ms_error << "*** CG failed to converge in " << mxitr
		    << " steps" << endl;
    garcon()->msout << "*** Residual = " << tlrnce << endl << ms_normal;
  }
  if(verbose) {
    if(!status) {
      garcon()->msout << ms_info << "*** CG converged in " << mxitr
		      << " iterations" << endl;
      garcon()->msout << "*** Residual = " << tlrnce << endl << ms_normal;
    }
    garcon()->msout << ms_info << "*** " << timer << endl << ms_normal;
  }

  // move disconnected nodes to sensible positions
  fix_disconnected();

  // check for negative areas
  ElementIterator iter(EI_ALL, this);
  Element *el;
  while(el = iter()) {
    if(el->current_area() <= 0.0) {
      garcon()->msout << ms_error
	    << "!!! Warning! Some elements have zero or negative area! !!!"
		      << endl << ms_normal;
      break;
    }
  }
    
  return (status==0? 1 : 0);	// I like 0=error, 1=ok
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// construct global stiffness matrices

void Grid::make_stiffness() {
    int i;
    SparseLinkMat kii(elastic_dof.free_dof.capacity(),
		      elastic_dof.free_dof.capacity());
    SparseLinkMat kib(elastic_dof.free_dof.capacity(),
		      elastic_dof.fixed_dof.capacity());
    SparseLinkMat kbb(elastic_dof.fixed_dof.capacity(),
		      elastic_dof.fixed_dof.capacity());
    
    for(i=0; i<interiorE.capacity(); i++)
	interiorE[i]->loadinterior(kii);
    for(i=0; i<boundaryE.capacity(); i++)
	boundaryE[i]->loadboundary(kii, kib, kbb);

    Kii = CRMat(kii);	// convert from SparseLinkMat to CompRow_Mat_double
    Kib = CRMat(kib);
    Kbb = CRMat(kbb);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef THERMAL
int Grid::thermal_equilibrate() {
  int status;

  // put nodes and elements in proper lists
  // don't do this more than necessary!
  // this really doesn't belong here, but it has to be done before this
  // point.

  needs_recomputed();
  shufflelists_thermal();
  apply_absolute_thermal_distortion();
 
  int n = thermal_dof.free_dof.capacity(); // degrees of freedom
  if(n == 0) {
    garcon()->msout << ms_error << "*** No degrees of freedom!"
		    << endl << ms_normal;
    return 1;
  }
  else if(verbose) {
    garcon()->msout << ms_info << "*** Solving " << n << "x" << n << " system" 
		    << endl << ms_normal;
  }
    
  Timer timer;

  // initialize vector of unknowns, using current displacements
  MV_Vector_double ui(displacements(thermal_dof.free_dof));
    
  // initialize vector of applied forces
  MV_Vector_double Fext(appliedforces(thermal_dof.free_dof));
    
  // initialize vector of boundary displacements
  MV_Vector_double ub(displacements(thermal_dof.fixed_dof));
	
  int mxitr = maxiter;	// will be modified by CG
  double tlrnce = tol;	// ditto
    
  // find stiffness matrices
  make_stiffnessT();
  
  // compute right hand side
  MV_Vector_double rhs(Fext);
  if(thermal_dof.fixed_dof.capacity() > 0)  // avoid null multiplication
    rhs += -1*(Kib*ub);
  
  // solve for interior displacements
  Preconditioner *precond = preconditioner(Kii);
  status = CG(Kii, ui, rhs, *precond, mxitr, tlrnce);
  delete precond;
      
  // copy displacements back to degrees of freedom
  displacements(thermal_dof.free_dof, ui);
  hash_needed = 1;

  if(status) {
    garcon()->msout << ms_error << "*** CG failed to converge in " << mxitr
		    << " steps" << endl;
    garcon()->msout << "*** Residual = " << tlrnce << endl << ms_normal;
  }
  if(verbose) {
    if(!status) {
      garcon()->msout << ms_info << "*** CG converged in " << mxitr
		      << " iterations" << endl;
      garcon()->msout << "*** Residual = " << tlrnce << endl << ms_normal;
    }
    garcon()->msout << ms_info << "*** " << timer << endl << ms_normal;
  }

  return (status==0? 1 : 0);	// I like 0=error, 1=ok
}

int Grid::equilibrate_all(int use_global)
{
  if(thermal_equilibrate())
    return equilibrate(use_global);
  return 0; 
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void Grid::make_stiffnessT() {
    int i;
    SparseLinkMat kii(thermal_dof.free_dof.capacity(), thermal_dof.free_dof.capacity());
    SparseLinkMat kib(thermal_dof.free_dof.capacity(), thermal_dof.fixed_dof.capacity());
    SparseLinkMat kbb(thermal_dof.fixed_dof.capacity(), thermal_dof.fixed_dof.capacity());
    
    for(i=0; i<interiorE.capacity(); i++)
	interiorE[i]->loadinteriorT(kii);
    for(i=0; i<boundaryE.capacity(); i++)
	boundaryE[i]->loadboundaryT(kii, kib, kbb);

    Kii = CRMat(kii);	// convert from SparseLinkMat to CompRow_Mat_double
    Kib = CRMat(kib);
    Kbb = CRMat(kbb);
}

#endif // THERMAL

// ---------------------------------------------------------------- //

// construct vectors of internal forces

MV_Vector_double Grid::Li() {
  int i;
  MV_Vector_double ell(elastic_dof.free_dof.capacity(), 0.0);
  for(i=0; i<interiorE.capacity(); i++)
    interiorE[i]->loadinterior_rhs(ell);
  for(i=0; i<boundaryE.capacity(); i++)
    boundaryE[i]->loadinterior_rhs(ell);
  return ell;
}

MV_Vector_double Grid::Lb() {
    MV_Vector_double ell(elastic_dof.fixed_dof.capacity(), 0.0);
    for(int i=0; i<boundaryE.capacity(); i++)
	boundaryE[i]->loadboundary_rhs(ell);
    return ell;
}

// ------------------------------------------------------------------ //



MeshCoord Grid::force(const NodeGroup *n) {
  Node *knowed;
  MeshCoord f(0.0, 0.0);	// total force
  GroupIterator<Node> iter(*n);
  while(knowed = iter())
    f += knowed->force();
  return f;
}

MeshCoord Grid::avgdisp(const NodeGroup *n) {
    int npts = n->size();
    if(npts == 0) return MeshCoord(0,0);
    GroupIterator<Node> iter(*n);
    Node *knowed;
    MeshCoord ad;
    while(knowed = iter())
	ad += knowed->displacement();
    return (1./npts)*ad;
}

MeshCoord Grid::avgposition(const NodeGroup *n) {
  int npts = n->size();
  if(npts == 0) return MeshCoord(0,0);
  GroupIterator<Node> iter(*n);
  MeshCoord ap;
  Node *knowed;
  while(knowed = iter())
    ap += knowed->position();
  return (1./npts)*ap;
}

#ifdef THERMAL
double Grid::avgtemp(const NodeGroup *n) const {
  Node *knowed;
  double Tsum = 0;
  int npts = n->size();
  if(npts == 0) return 0.0;
  GroupIterator<Node> iter(*n);
  while(knowed = iter())
    Tsum += knowed->voltage();
  return Tsum/npts;
}

double Grid::heatrate(const NodeGroup *n) const {
  Node *knowed;
  double qdot = 0;
  GroupIterator<Node> iter(*n);
  while(knowed = iter())
    qdot += knowed->surf_charge();
  return qdot;
}
#endif // THERMAL

double Grid::energy() {
#ifdef BADMETHOD
  // what's wrong with this method?
  double eng;
  shufflelists(); 		// get dof indices in order
  MV_Vector_double ui(displacements(free_dof));
  MV_Vector_double ub(displacements(fixed_dof));
  
  if(use_globalK) {
    make_stiffness();
    eng = 0.5*(dot(ui, Kii*ui) + dot(ub, Kbb*ub) + 2*dot(ui, Kib*ub)
		- dot(ub, Lb()) - dot(ui, Li()));
    garcon()->msout << endl;
//     garcon()->msout << "Kii: " << 0.5*dot(ui, Kii*ui) << endl;
//     garcon()->msout << "Kbb: " << 0.5*dot(ub, Kbb*ub) << endl;
//     garcon()->msout << "Kib: " << dot(ui, Kib*ub) << endl;
//     garcon()->msout << "Lb : " << -dot(ub, Lb()) << endl;
//     garcon()->msout << "Li : " << -dot(ui, Li()) << endl;
    }
  else {
    Stiffness kbb(this, F_FIXED, F_FIXED);
    Stiffness kbi(this, F_FIXED, F_FREE);
    Stiffness kii(this, F_FREE, F_FREE);
    eng = 0.5*(dot(ui, kii*ui) + dot(ub, kbb*ub) + 2*dot(ub, kbi*ui)
      - dot(ub, Lb()) - dot(ui, Li()));
  }
  garcon()->msout << " (" << eng << ") ";
#endif

  ElementIterator iter(EI_ALL, this);
  Element *el;
  double ee = 0;
  while(el = iter())
    ee += el->elastic_energy()*el->original_area();
  return ee;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Move disconnected nodes to the average position of their neighbors
#define FIX_ITERATIONS 10

void Grid::fix_disconnected() {
  // make list of disconnected nodes
  Vec<Node*> discon;
  discon.setphysicalsize(elastic_dof.inactive_dof.capacity()/2);
                                      // maybe bigger than it needs to be...
  for(int i=0; i<node.capacity(); i++)
    if(node[i]->disconnected())
      discon.grow(1, node[i]);
  if(discon.capacity() == 0) return;

  int N = discon.capacity();

  // tabulate neighbors of disconnected nodes
  Vec<Vec<Node*> > nbrlist(N);
  for(int i=0; i<N; i++) {
    int j, k;
    bool linked;
    SparseLinkIterator<bool> iter(fullconnectivity, discon[i]->index);
    // loop over row of connectivity matrix corresponding to node discon[i]
    while(iter(j, k, linked))
      if(linked)		// j and k are neighbors
	nbrlist[i].grow(1, node[k]);
  }

  // create a vector of ints to shuffle, so we can loop over the
  // disconnected nodes in a random order
  Vec<int> order(N);
  for(int k=0; k<N; k++)
    order[k] = k;

  // iterate the procedure
  for(int iteration=0; iteration<FIX_ITERATIONS; iteration++) {
    // shuffle the order
    for(int k=0; k<order.capacity(); k++) {
      int x = int(k + rndm()*(N-k)); // pick a remaining node at random
      if(x != k) {		// swap random node with node k
	int temp = order[k];
	order[k] = order[x];
	order[x] = temp;
      }
    }
    for(int k=0; k<N; k++) {	// loop over disconnected nodes
      int kk = order[k];
      MeshCoord avg;
      for(int n=0; n<nbrlist[kk].capacity(); n++)
	avg += nbrlist[kk][n]->position();
      avg /= nbrlist[kk].capacity();
      discon[kk]->move(avg - discon[kk]->position());
    }
  }
}
