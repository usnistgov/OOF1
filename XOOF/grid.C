// -*- C++ -*-
// $RCSfile: grid.C,v $
// $Revision: 1.21 $
// $Author: langer $
// $Date: 2005-07-30 20:16:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// general grid maintenance and repair

//#include "cmdlist.h"
#include "elementgroups.h"
#include "formdrawer.h"
#include "grid.h"
#include "griditerator.h"
#include "hashmesh.h"
#include "mvmult.h"
#include "node.h"
#include "nodegroups.h"
#include "parameters.h"
#include "statistics.h"
#include "random.h"
#include "mvmd.h"
#include "mvvd.h"
#include <math.h>
#include "timer.h"

// ------------------------------------------------------------ //

Grid::Grid()
  : node(0, BlockSize(100)),
    interiorE(0, BlockSize(100)),
    boundaryE(0, BlockSize(100)),
    nodegroup(0), 
    nodehash(&Node::position),
    elemhash(&Element::center),
    hash_needed(1),
    enhancement_(enhancement_dflt),
#ifdef THERMAL
    fix_masters_thermal_flag(false),
#endif // THERMAL
    fix_masters_elastic_flag(false)
    
{}

void Grid::setNelements(int Nel) {
  interiorE.setphysicalsize(Nel);
}

void Grid::setNnodes(int Nnd) {
  node.setphysicalsize(Nnd);
  elastic_dof.free_dof.setphysicalsize(2*Nnd);
#ifdef THERMAL
  thermal_dof.free_dof.setphysicalsize(Nnd);
#endif
}

Grid::~Grid() {
  // do these in reverse order, since the groups remove themselves
  // from the list as they're deleted.
  for(int i=nodegroup.capacity()-1; i>=0; i--)
    delete nodegroup[i];
  for(int i=elementgroup.capacity()-1; i>=0; i--)
    delete elementgroup[i];

  for(int i=0; i<interiorE.capacity(); i++)
    delete interiorE[i];
  for(int i=0; i<boundaryE.capacity(); i++)
    delete boundaryE[i];
  for(int i=0; i<node.capacity(); i++)
    delete node[i];
}

// ------------------------------------------------------------ //

void Grid::redraw() {
  FormDrawer::GridUpdated(this);
  FormDrawer::redraw(this);
}

// ------------------------------------------------------------ //

int Grid::AddElement(Element *elem) {    // add to interior element list
  if(elem->index >= interiorE.capacity())
    interiorE.resize(elem->index+1, (Element*) 0);
  if(interiorE[elem->index] != 0) {
    garcon()->msout << ms_error << "Duplicate element index " << elem->index
		    << "!" << endl << ms_normal;
    return 0;
  }
  interiorE[elem->index] = elem;
  return 1;
}

// ------------------------------------------------------------ //

void Grid::AddNode(int index, Node *newnode) {    // add to node list
  if(index >= node.capacity()) node.resize(index+1, (Node*)0);
  node[index] = newnode;
}

// ------------------------------------------------------------ //

/* Create node connectivity matrix.  Two nodes are connected if they
 * are in a non-empty element.
 */

void Grid::make_connectivity() {
  ElementIterator iter(EI_NONEMPTY, this);
  Element *el;
  while(el = iter()) {
    for(int i=1; i<el->corner.capacity(); i++)
      for(int j=0; j<i; j++) {
	connectivity(el->corner[i]->index, el->corner[j]->index) = true;
	connectivity(el->corner[j]->index, el->corner[i]->index) = true;
      }
  }
  ElementIterator iter2(EI_ALL, this);
  while(el = iter2()) {
    for(int i=1; i<el->corner.capacity(); i++)
      for(int j=0; j<i; j++) {
	fullconnectivity(el->corner[i]->index, el->corner[j]->index) = true;
	fullconnectivity(el->corner[j]->index, el->corner[i]->index) = true;
      }
  }
}

#ifdef THERMAL
void Grid::make_connectivity_phi() {
  ElementIterator iter(EI_NONEMPTY_PHI, this);
  Element *el;
  while(el = iter()) {
    for(int i=1; i<el->corner.capacity(); i++)
      for(int j=0; j<i; j++) {
	connectivity_phi(el->corner[i]->index, el->corner[j]->index) = true;
	connectivity_phi(el->corner[j]->index, el->corner[i]->index) = true;
      }
  }
}
#endif // THERMAL

void Grid::reset_connectivity() {
  connectivity.clear();
  fullconnectivity.clear();
#ifdef THERMAL
  connectivity_phi.clear();
#endif
}

// is node i connected physically to any other nodes?

bool Grid::connected(int i) {
    if(connectivity.rows() == 0)
	make_connectivity();

    SparseLinkIterator<bool> iter(connectivity, i);  // loop over row i
    int j, k;
    bool link;
    while(iter(j, k, link))
	if(link) return true;
    return false;
}

#ifdef THERMAL
bool Grid::connected_phi(int i) {
  if(connectivity_phi.rows() == 0)
    make_connectivity_phi();
  
  SparseLinkIterator<bool> iter(connectivity_phi, i);
  int j, k;
  bool link;
  while(iter(j, k, link))
    if(link) return true;
  return false;
}
#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void Grid::update_node_elists() {
  if(elements_changed > node_elists_updated) {
    ++node_elists_updated;
    for(int j=0; j<node.capacity(); j++)
      node[j]->element.resize(0);
    ElementIterator iter(EI_ALL, this);
    Element *el;
    while(el = iter()) {
    for(int i=0; i<el->corner.capacity(); i++)
      el->corner[i]->element.grow(1, el);
    }
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//


NodeGroup *Grid::getgroup(const CharString &label, const TrueFalse &create) {
    // look for group with given label
    for(int i=0; i<nodegroup.capacity(); i++)
	if(label == nodegroup[i]->name())
	    return nodegroup[i];
    // construct new group
    if(create) {
	NodeGroup *newgroup = new NodeGroup(label);
	nodegroup.grow(1, newgroup);
	return newgroup;
    }
    return 0;
}

void Grid::showgroups() const {
  if(nodegroup.capacity() == 0)
    garcon()->msout << ms_error << "  No groups!" << endl << ms_normal;
  else
    for(int i=0; i<nodegroup.capacity(); i++)
      garcon()->msout << "  " << *nodegroup[i] << endl;
}

ElementGroup *Grid::getEgroup(const CharString &label, const TrueFalse &create)
{
  // look for group with given label
  for(int i=0; i<elementgroup.capacity(); i++)
    if(label == elementgroup[i]->name())
      return elementgroup[i];
  // construct new group
  if(create) {
    // garcon()->msout << "creating group " << label << endl;
    ElementGroup *newgroup = new ElementGroup(label);
    elementgroup.grow(1, newgroup);
    return newgroup;
  }
  return 0;
}

void Grid::showEgroups() const {
  if(elementgroup.capacity() == 0)
    garcon()->msout << ms_error << "  No groups!" << endl << ms_normal;
  else
    for(int i=0; i<elementgroup.capacity(); i++)
      garcon()->msout << "  " << *nodegroup[i] << endl;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// free (unfix) all nodes in all node groups

void Grid::freeall_nodes_x() const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->free_x();
}

void Grid::freeall_nodes_y() const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->free_y();
}

#ifdef THERMAL
void Grid::freeall_nodes_phi() const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->free_phi();
}
#endif

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Emancipate all dof's, but leave the nodegroups marked for enslavement

void Grid::emancipate_all_elastic_dofs() const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->do_emancipatenodes_elastic_all();
}

void Grid::emancipate_all_groups() const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->emancipatenodes_all();
}

// Enslave all node groups that are marked for enslavement

void Grid::enslave_marked_elastic_dofs() const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->do_enslavenodes_elastic_all();
}

#ifdef THERMAL

void Grid::emancipate_all_thermal_dofs() const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->do_emancipatenodes_thermal_all();
}

void Grid::enslave_marked_thermal_dofs() const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->do_enslavenodes_thermal_all();
}

#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Apply distortions and forces. 


// apply absolute distortions (stored in nodegroups) to the nodes themselves

void Grid::apply_absolute_elastic_distortion() {
  // The force on a node is the total force of all of that nodes
  // nodegroups.  So the force has to be reset here, before applying
  // any of the groups' forces.  The displacements don't accumulate
  // like this, so they are reset just before being applied by each
  // nodegroup.
  for(int i=0; i<node.capacity(); i++)
    node[i]->reset_force();
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->apply_absolute_elastic_distortion();
  newcoords();
  hash_needed = 1;
}

// apply all groups' incremental distortions and forces to their nodes

void Grid::apply_incremental_elastic_distortion() {
  // increment the absolute distortions stored in the groups
  for(int i=0; i<nodegroup.capacity(); i++) {
    NodeGroup *n = nodegroup[i];
    n->absolute_force += n->incremental_force;
    n->absolute_distortion += n->incremental_distortion;
    n->absolute_shift += n->incremental_shift;
  }
  // apply the new absolute distortion
  apply_absolute_elastic_distortion();
}

void Grid::clear_elastic_distortion() {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->clear_elastic_distortion();
  apply_absolute_elastic_distortion();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef THERMAL

// See the analogous elastic distortion routines above for a few more
// relevant comments...

void Grid::apply_absolute_thermal_distortion() {
  for(int i=0; i<node.capacity(); i++)
    node[i]->reset_surfcharge();
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->apply_absolute_thermal_distortion();
}

void Grid::apply_incremental_thermal_distortion() {
  for(int i=0; i<nodegroup.capacity(); i++) {
    NodeGroup *n = nodegroup[i];
    n->absolute_thermaldistortion += n->incremental_thermaldistortion;
  }
  apply_absolute_thermal_distortion();
}

void Grid::clear_thermal_distortion() {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->clear_thermal_distortion();
  apply_absolute_thermal_distortion();
}

#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// tell elements that the coordinates have changed. they may want to
// update their stiffness matrices.

void Grid::newcoords() {
  for(int i=0; i<interiorE.capacity(); i++)
    interiorE[i]->newcoords();
  for(int i=0; i<boundaryE.capacity(); i++)
    boundaryE[i]->newcoords();
}

// ------------------------------------------------------------ //

static const MeshCoord zeroforce(0., 0.);
static const MeshCoord zeroshift(0., 0.);
static const TwoTwo zeromatrix;
#ifdef THERMAL
static const ThermalDistortion zerothermaldistortion;
#endif // THERMAL

void Grid::reset() {
  for(int i=0; i<node.capacity(); i++) {
    node[i]->reset_displacement(1, 1);
    node[i]->reset_force();
#ifdef THERMAL
    node[i]->reset_surfcharge();
    node[i]->reset_voltage();
#endif // THERMAL
  }
  for(int i=0; i<nodegroup.capacity(); i++) {
    nodegroup[i]->absolute_force = zeroforce;
    nodegroup[i]->absolute_distortion = zeromatrix;
    nodegroup[i]->absolute_shift = zeroshift;
#ifdef THERMAL
    nodegroup[i]->absolute_thermaldistortion = zerothermaldistortion;
#endif // THERMAL
  }
  needs_recomputed(); // so that stress and strain are recomputed

  ElementIterator iter(EI_ALL, this);
  Element *el;
  while(el = iter())
    el->revert();
  hash_needed = 1;
  newcoords();
}

// ------------------------------------------------------------ //

double Grid::enhancement_dflt = 1.0;

void Grid::set_enhancement(double enh) {
  enhancement_ = enh;
  require_rehash();
}

void Grid::set_enhancement_cmd(const CharString&) { // static
  if(grid) {
    grid->set_enhancement(enhancement_dflt);
    grid->redraw();
  }
}

// ------------------------------------------------------------ //

#ifdef DEBUG
void Grid::printKlocal(ostream &os) {
    int ni = interiorE.capacity();
    for(int i=0; i<ni; i++) {
	os << "interior element " << i << endl;
	interiorE[i]->printstiffness(os);
    }
    int nb = boundaryE.capacity();
    for(int i=0; i<nb; i++) {
	os << "boundary element " << i << endl;
	boundaryE[i]->printstiffness(os);
    }
}

void Grid::printKii(ostream &os) {
  if(!Kii.dim(0)) {
    shufflelists_elastic();
    make_stiffness();
  }
  os << Kii.dim(0) << " " << Kii.dim(1) << endl;
  os << Kii;
}

void Grid::printKib(ostream &os) {
  if(!Kib.dim(0)) {
    shufflelists_elastic();
    make_stiffness();
  }
  os << Kib.dim(0) << " " << Kib.dim(1) << endl;
  os << Kib;
}

void Grid::printKbb(ostream &os) {
  if(!Kbb.dim(0)) {
    shufflelists_elastic();
    make_stiffness();
  }
  os << Kbb.dim(0) << " " << Kbb.dim(1) << endl;
  os << Kbb;
}
#endif // DEBUG

void Grid::postscript(FILE *psfile) {
    fprintf(psfile,"%%!Postscript from oof\n");
    fprintf(psfile,"/el {newpath moveto lineto lineto stroke} def\n");
    int ni = interiorE.capacity();
    for(int i=0; i<ni; i++)
      {
      for (int ic = 2; ic >= 0 ; ic--)
        fprintf(psfile,"%f %f   ",
		interiorE[i]->corner[ic]->position().x,
		interiorE[i]->corner[ic]->position().y);
        fprintf(psfile,"el\n");
      }
    int nb = boundaryE.capacity();
    for(int i=0; i<nb; i++)
      {
      for (int ic = 2; ic >= 0 ; ic--)
        fprintf(psfile,"%f %f   ",
		boundaryE[i]->corner[ic]->position().x,
		boundaryE[i]->corner[ic]->position().y);
        fprintf(psfile,"el\n");
      }
}

Rectangle Grid::query_extreme_pos() const {
  if(node.capacity() < 2)
    return Rectangle(0, 1, 0, 1);
  Rectangle range(node[0]->position(enhancement()),
		  node[1]->position(enhancement()));
  for (int i=0; i<node.capacity(); i++)
    range.swallow(node[i]->position(enhancement()));
  return range;
}

// ------------------------------------------------------------- //

Node *Grid::findNode(const MeshCoord &where) {
    rehash();
    return nodehash.find(where);
}

Element *Grid::findElement(const MeshCoord &where) {
    rehash();
    return elemhash.find(where);
}

void Grid::rehash() {
    if(!hash_needed) return;
    Rectangle range(query_extreme_pos());
    double aspect = (range.xmax-range.xmin)/(range.ymax-range.ymin);
    int nx = int(0.5*sqrt((double) node.capacity())*aspect);
    int ny = int(0.5*sqrt((double) node.capacity())/aspect);
    nodehash.setsize(nx, ny, range);
    elemhash.setsize(nx, ny, range);
    for(int i=0; i<node.capacity(); i++)
	nodehash.add(node[i]);
    for(int i=0; i<interiorE.capacity(); i++)
	elemhash.add(interiorE[i]);
    for(int i=0; i<boundaryE.capacity(); i++)
	elemhash.add(boundaryE[i]);
    hash_needed = 0;
}

// --------------------------------------------------------------- //

void Grid::unselectallelements() const {
    for(int i=0; i<interiorE.capacity(); i++)
	interiorE[i]->unselect();
    for(int i=0; i<boundaryE.capacity(); i++)
	boundaryE[i]->unselect();
}

void Grid::selectallelements() const {
    for(int i=0; i<interiorE.capacity(); i++)
	interiorE[i]->select();
    for(int i=0; i<boundaryE.capacity(); i++)
	boundaryE[i]->select();
}

void Grid::toggleselect() const {
  for(int i=0; i<interiorE.capacity(); i++)
    interiorE[i]->toggleselect(); 
  for(int i=0; i<boundaryE.capacity(); i++)
    boundaryE[i]->toggleselect();
}

void Grid::selectelement(int which) const {
  for(int i=0; i<interiorE.capacity(); i++)
    if(interiorE[i]->index == which) {
      interiorE[i]->select();
      return;
    }
  for(int i=0; i<boundaryE.capacity(); i++)
    if(boundaryE[i]->index == which) {
      boundaryE[i]->select();
      return;
    }
}

void Grid::selectrandomelements(double prob) const {
    for(int i=0; i<interiorE.capacity(); i++)
	if(rndm() < prob)
	    interiorE[i]->select();
	else
	    interiorE[i]->unselect();
    for(int i=0; i<boundaryE.capacity(); i++)
	if(rndm() < prob)
	    boundaryE[i]->select();
	else
	    boundaryE[i]->unselect();
}

void Grid::selectshape(const Shape &shape) const {
    for(int i=0; i<interiorE.capacity(); i++)
	if(shape.contains(interiorE[i]->center()))
	   interiorE[i]->select();
    for(int i=0; i<boundaryE.capacity(); i++)
	if(shape.contains(boundaryE[i]->center()))
	   boundaryE[i]->select();
}

// ---------------------------------------------------------------- //

void Grid::selectallnodes() const {
  for(int i=0; i<node.capacity(); i++)
    node[i]->select();
}

void Grid::selectshape_nodes(const Shape &shape) const {
  for(int i=0; i<node.capacity(); i++)
    if(shape.contains(node[i]->position()))
      node[i]->select();
}

void Grid::unselectallnodes() const {
  for(int i=0; i<node.capacity(); i++)
    node[i]->unselect();
}

// ---------------------------------------------------------------- //

int Grid::mutate() {
  int count = 0;   // number of elements mutated
  Element *el;
  ElementIterator iter(EI_ALL, this);
  Interrupter stop;
  while((el = iter()) && !stop()) {
    if(el->mutate()) {
      el->setstiffness();
      count++;
    }
  }
  //  redraw();  // redraw is done in higher level routine
  return count;
}

void Grid::needs_recomputed() {
  Element *el;
  ElementIterator iter(EI_ALL, this);
  while(el = iter()) {
    el->not_current();
  }
}

void Grid::complete_equilibrate(int use_global) {
  int mutants = 1;
  Interrupter stopnow;
  while(mutants && !stopnow()) {
    int equilibrated = equilibrate(use_global);
    redraw();
    if(garcon()->graphics()) XFlush(fl_get_display());
    if(!equilibrated) return;	// bail out if doesn't converge
    mutants = mutate();
    garcon()->msout << ms_info << mutants << " element" << (mutants==1?"":"s")
		    << " mutated." << endl << ms_normal;
  }
}

StressStrainStat Grid::stress_statistics(Iterator<Element> &itel) const {
  Element *el;
  StressStrainStat stats;
  while(el = itel())
    {
      stats.include(el->stresstensor(), el->original_area());
    }
  return stats;
}
    
StressStrainStat Grid::strain_statistics(Iterator<Element> &itel) const {
  Element *el;
  StressStrainStat stats;
  while(el = itel())
    {
      stats.include(el->total_straintensor(), el->original_area());
    }
  return stats;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

DoFset::DoFset()
  : fixed_dof(0, BlockSize(100)),
    free_dof(0, BlockSize(100)),
    inactive_dof(0, BlockSize(10))
{}

DoFset::~DoFset() {
  for(int i=0; i<free_dof.capacity(); i++)
    delete free_dof[i];
  for(int i=0; i<fixed_dof.capacity(); i++)
    delete fixed_dof[i];
  for(int i=0; i<inactive_dof.capacity(); i++)
    delete inactive_dof[i];
}

void DoFset::initialize(int n) {
  free_dof.setphysicalsize(n);
  free_dof.resize(0);
  fixed_dof.resize(0);
  inactive_dof.resize(0);
}

DegreeOfFreedom *DoFset::AddDoF(DegreeOfFreedom *dof) {
  free_dof.grow(1, dof);
  return dof;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef DEBUG
void Grid::nodegroupdump(ostream &os) const {
  for(int i=0; i<nodegroup.capacity(); i++)
    nodegroup[i]->dump(os);
}

void Grid::nodedump(ostream &os) const {
  os << "*** Nodes ***" << endl;
  for(int i=0; i<node.capacity(); i++)
    node[i]->dump(os);
}

#ifdef THERMAL
void Grid::dofdump(ostream &os) const {
  os << "*** thermal dofs  ***" << endl;
  thermal_dof.dump(os);
}
#else  // !THERMAL
void Grid::dofdump(ostream&) const {}
#endif // THERMAL

void DoFset::dump(ostream &os) const {
  os << "------ free" << endl;
  for(int i=0; i<free_dof.capacity(); i++)
    free_dof[i]->dump(os, "");
  os << "------ fixed" << endl;
  for(int i=0; i<fixed_dof.capacity(); i++)
    fixed_dof[i]->dump(os, "");
  os << "------ inactive" << endl;
  for(int i=0; i<inactive_dof.capacity(); i++)
    inactive_dof[i]->dump(os, "");
}
#endif // DEBUG

#include "hashmesh.C"
