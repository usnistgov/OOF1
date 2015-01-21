// -*- C++ -*-
// $RCSfile: modifygrid.C,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2000-11-20 21:11:08 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// make modifications to grid list

#include "grid.h"
#include "element.h"
#include "freedom.h"
#include "node.h"
#include "vec.h"
#include "parameters.h"
#include "timer.h"
#include "stdlib.h"

// Reassign the various interior and boundary lists, according to
// whether or not the degrees of freedom of the nodes are fixed or
// free. It is assumed that the DoF flags are already set.


void Grid::shufflelists_elastic() {
  // check for disconnected nodes
  int discount = 0;
  Timer timer;
  for(int i=0; i<node.capacity(); i++) {
    if(!connected(i)) {
      discount++;
      node[i]->x->setflag(DISCONNECTED);
      node[i]->y->setflag(DISCONNECTED);
    }
    else {
      // node may have been disconnected previously, so make sure the
      // flag is set properly.
      node[i]->x->clearflag(DISCONNECTED);
      node[i]->y->clearflag(DISCONNECTED);
    }
  }
  if(verbose)
    garcon()->msout << ms_info << discount << " disconnected node"
		    << (discount == 1?"":"s") << " found (" << timer << ")"
		    << endl << ms_normal;

  // make copies of element arrays
  Vec<Element*> old_iE = interiorE;
  Vec<Element*> old_bE = boundaryE;

  // clear the original arrays
  interiorE.resize(0);
  boundaryE.resize(0);
  // put degrees of freedom in the correct lists
  fix_masters_elastic();
  elastic_dof.shufflelists();
  // put elements in new lists
  move_elem(old_iE);
  move_elem(old_bE);
}

    
#ifdef THERMAL
void Grid::shufflelists_thermal() {
  // check for disconnected nodes
  int discount = 0;
  Timer timer;
  for(int i=0; i<node.capacity(); i++) {
    if(!connected_phi(i)) {
      discount++;
      node[i]->phi->setflag(DISCONNECTED);
    }
    else
      node[i]->phi->clearflag(DISCONNECTED);
  }
  if(verbose)
    garcon()->msout << ms_info << discount << " disconnected node"
		    << (discount == 1?"":"s") << " found (" << timer << ")"
		    << endl << ms_normal;

  // make copies of element arrays
  Vec<Element*> old_iE = interiorE;
  Vec<Element*> old_bE = boundaryE;

  // clear the original arrays
  interiorE.resize(0);
  boundaryE.resize(0);
  // put degrees of freedom in the correct lists
  fix_masters_thermal();
  thermal_dof.shufflelists();
  // put elements in new lists
  move_elem(old_iE);
  move_elem(old_bE);
}
#endif // THERMAL

void DoFset::shufflelists() {
  Vec<DegreeOfFreedom*> old_free = free_dof;
  Vec<DegreeOfFreedom*> old_fixed = fixed_dof;
  Vec<DegreeOfFreedom*> old_inactive = inactive_dof;
    
  free_dof.resize(0);
  fixed_dof.resize(0);
  inactive_dof.resize(0);
    
  // put degrees of freedom in the correct lists
  shuffle_dof(old_free);
  shuffle_dof(old_fixed);
  shuffle_dof(old_inactive);

  // assign dof indexes
  for(int i=0; i<free_dof.capacity(); i++)
    free_dof[i]->dofindex() = i;
  for(int i=0; i<fixed_dof.capacity(); i++)
    fixed_dof[i]->dofindex() = i;
}

void DoFset::shuffle_dof(const Vec<DegreeOfFreedom*> &doflist) {
  int n = doflist.capacity();
  for(int i=0; i<n; i++) {
    if(doflist[i]->defunct())
      delete doflist[i];
    else if(doflist[i]->isslave() || doflist[i]->disconnected())
      inactive_dof.grow(1, doflist[i]); // slaved or marooned
    else if(doflist[i]->fixed())
      fixed_dof.grow(1, doflist[i]); // fixed 
    else
      free_dof.grow(1, doflist[i]); // free
  }
}

void Grid::fix_masters() {
  fix_masters_elastic();
#ifdef THERMAL
  fix_masters_thermal();
#endif
}

void Grid::fix_masters_needed() {
  fix_masters_elastic_needed();
#ifdef THERMAL
  fix_masters_thermal_needed();
#endif // THERMAL
}

void Grid::fix_masters_elastic() {
  if(fix_masters_elastic_flag) {
    emancipate_all_elastic_dofs(); // emancipate all groups
    enslave_marked_elastic_dofs(); // create masters for enslaved groups
    elastic_dof.fix_masters();	// f	ix the masters of fixed dofs
    apply_absolute_elastic_distortion(); // cuz emancipate&enslave mess up bcs
    fix_masters_elastic_flag = false;
  }
}

#ifdef THERMAL
void Grid::fix_masters_thermal() {
  if(fix_masters_thermal_flag) {
    emancipate_all_thermal_dofs(); // emancipate all groups
    enslave_marked_thermal_dofs(); // create masters for enslaved groups
    thermal_dof.fix_masters();	// fix the masters of fixed dofs
    apply_absolute_thermal_distortion(); // cuz emancipate&enslave mess up bcs
    fix_masters_thermal_flag = false;
  }
}
#endif // THERMAL

void DoFset::fix_masters() {
  // propagate the "fixed" flags to the master degrees of freedom
  clear_fixed_flags(free_dof);	// clear flag in masters
  clear_fixed_flags(fixed_dof);
  clear_fixed_flags(inactive_dof);
  set_fixed_flags(free_dof);	// set flags in masters if slaves are fixed
  set_fixed_flags(fixed_dof);
  set_fixed_flags(inactive_dof);
}

void DoFset::clear_fixed_flags(const Vec<DegreeOfFreedom*> &doflist) {
  // clear the fixed flags in all master degrees of freedom
  for(int i=0; i<doflist.capacity(); i++)
    doflist[i]->clear_master_fix();
}

void DoFset::set_fixed_flags(const Vec<DegreeOfFreedom*> &doflist) {
  // set the fixed flags in all masters which have fixed slaves
  for(int i=0; i<doflist.capacity(); i++)
    if(doflist[i]->isslave() && doflist[i]->DegreeOfFreedom::fixed())
      doflist[i]->controller()->fix();
}

void Grid::move_elem(const Vec<Element*> &elist) {
  int n = elist.capacity();
  for(int i=0; i<n; i++) {
    if(elist[i]->isbdy()) {	    // move to boundary
      int index = boundaryE.grow(1);
      boundaryE[index] = elist[i];
    }
    else {			    // move to interior
      int index = interiorE.grow(1);
      interiorE[index] = elist[i];
    }
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

int Element::compare(const void *a, const void *b) {
  const Element *ela = (const Element*) * (Element **) a;
  const Element *elb = (const Element*) * (Element **) b;
  if(ela->index < elb->index) return -1;
  if(ela->index > elb->index) return 1;
  return 0;
}

// Put all elements into the interior list, and put their indices in order

void Grid::unshufflelists() {
  int Ninterior = interiorE.capacity();
  interiorE.grow(boundaryE.capacity());
  for(int i=0; i<boundaryE.capacity(); i++)
    interiorE[Ninterior+i] = boundaryE[i];
  boundaryE.resize(0);

  qsort(&interiorE[0], interiorE.capacity(), sizeof(Element*),
	Element::compare);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//
