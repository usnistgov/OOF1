// -*- C++ -*-
// $RCSfile: grid.C,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2000-12-26 20:22:23 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// general grid maintenance and repair

#include "element.h"
#include "elementgroups.h"
#include "grid.h"
#include "griditerator.h"
#include "main.h"
#include "node.h"
#include "nodegroups.h"

// ------------------------------------------------------------ //

Grid::Grid()
  : node(0, BlockSize(100)),
    interiorE(0, BlockSize(100)),
    nodegroup(0, BlockSize(10)),
    elementgroup(0, BlockSize(10)),
    materialgroup(0, BlockSize(10))
{}

void Grid::setNelements(int Nel) {
  interiorE.setphysicalsize(Nel);
}

void Grid::setNnodes(int Nnd) {
  node.setphysicalsize(Nnd);
}

Grid::~Grid() {
  // do these in reverse order, since the groups remove themselves
  // from the list as they're deleted.
  for(int i=nodegroup.capacity()-1; i>=0; i--)
    delete nodegroup[i];
  for(int i=elementgroup.capacity()-1; i>=0; i--)
    delete elementgroup[i];
  for(int i=materialgroup.capacity()-1; i>0; i--)
    delete materialgroup[i];

  for(int i=0; i<interiorE.capacity(); i++)
    delete interiorE[i];
  for(int i=0; i<node.capacity(); i++)
    delete node[i];
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
  if(index >= node.capacity())
    node.resize(index+1, (Node*)0);
  node[index] = newnode;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void Grid::make_connectivity() {
  ElementIterator iter(EI_NONEMPTY, this);
  Element *el;
  while(el = iter()) {
    for(int i=1; i<el->corner.capacity(); i++) {
      for(int j=0; j<i; j++) {
        connectivity(el->corner[i]->index, el->corner[j]->index) = true;
        connectivity(el->corner[j]->index, el->corner[i]->index) = true;
      }
      for(int j=0; j<el->edgenode.capacity(); j++) {
	connectivity(el->corner[i]->index, el->edgenode[j]->index) = true;
	connectivity(el->edgenode[j]->index, el->corner[i]->index) = true;
      }
    }
    for(int i=0; i<el->edgenode.capacity(); i++) {
      for(int j=0; j<i; j++) {
	connectivity(el->edgenode[i]->index, el->edgenode[j]->index) = true;
        connectivity(el->edgenode[j]->index, el->edgenode[i]->index) = true;
      }
    }
  }
}

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

Node *Grid::get_edgenode(const Node* n0, const Node *n1) {
  // see if node has been created yet
  // the edgenode matrix assumes that n0->index < n1->index.
  int nn0 = n0->index;
  int nn1 = n1->index;
  if(nn0 > nn1) {
    int tmp = nn1;
    nn1 = nn0;
    nn0 = tmp;
  }
  Node *enode = edgenodes(nn0, nn1);
  if(!enode) {			// create it
    enode = new XYNode(node.capacity(), 
		       0.5*(n0->position.x + n1->position.x),
		       0.5*(n0->position.y + n1->position.y));
    node.grow(1, enode);	// store in node vector
    edgenodes(nn0, nn1) = enode; // store in edge matrix
    // add the new node to any groups containing both neighbors
    for(int i=0; i<n0->groups.capacity(); i++) {
      for(int j=0; j<n1->groups.capacity(); j++) {
	if(n0->groups[i] == n1->groups[j]) {
	  n0->groups[i]->add(enode);
	}
      }
    }
  }
  return enode;
}

void Grid::make_edgenodes() {
  ElementIterator *iter;
  if(empties)
    iter = new ElementIterator(EI_ALL, this);
  else
    iter = new ElementIterator(EI_NONEMPTY, this);
  Element *el;
  while(el = (*iter)()) {
    el->make_edgenodes(this);
  }
  delete iter;
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

ElementGroup *Grid::getEgroup(const CharString &label, const TrueFalse &create)
{
  // look for group with given label
  for(int i=0; i<elementgroup.capacity(); i++)
    if(label == elementgroup[i]->name())
      return elementgroup[i];
  // construct new group
  if(create) {
    ElementGroup *newgroup = new ElementGroup(label);
    elementgroup.grow(1, newgroup);
    return newgroup;
  }
  return 0;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// generate an elementset name for the i^th material group
static CharString elsetname(int i) {
  return "m" + to_charstring(i+1);
}

// generate a material name for the i^th material group
static CharString materialname(int i) {
  return "material-" + to_charstring(i+1);
}

// Find the Material group to which this element belongs
ElementGroup *Grid::getMgroup(const Element *el) const {
  // loop over groups
  for(int i=0; i<materialgroup.capacity(); i++)
    // compare to first element in the group
    if(*((*materialgroup[i])[0]) == *el)
      return materialgroup[i];
  return 0;
}

ElementGroup *Grid::makeMgroup(const CharString &label) {
  ElementGroup *newgroup = new ElementGroup(label);
  materialgroup.grow(1, newgroup);
  return newgroup;
}

void Grid::find_material_groups() {
  for(int i=0; i<interiorE.capacity(); i++) {
    Element *el = interiorE[i];
    ElementGroup *grp = getMgroup(el);
    if(grp)
      grp->Add(el);
    else {
      ElementGroup *newgroup =
	makeMgroup(elsetname(materialgroup.capacity()));
      newgroup->Add(el);
    }
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void Grid::print_abaqus(ostream &os) {
  list_materials(os);
  if(sixnodes)
    make_edgenodes();
  print_nodes(os);
  print_elements(os);
  print_nodegroups(os);
  print_elementgroups(os);
  print_materials(os);
}

void Grid::print_nodes(ostream &os) {
  os << "**\n** nodes\n**" << endl;
  os << "*Node" << endl;
  for(int i=0; i<node.capacity(); i++) {
    if(empties || connected(i)) {
      os << *node[i] << endl;
    }
  }
}

void Grid::print_nodegroups(ostream &os) const {
  if(nodegroup.capacity() > 0) {
    os << "**\n** node groups\n**" << endl;
    for(int i=0; i<nodegroup.capacity(); i++)
      os << *nodegroup[i];	// endl not needed
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void Grid::print_elements(ostream &os) const {
  // print elements by material type, in case planestrain differs from
  // element to element
  os << "**\n** elements\n**" << endl;
  for(int i=0; i<materialgroup.capacity(); i++) {
    ElementGroup &grp = *materialgroup[i];
    if(empties || !grp[0]->isempty()) {
      os << "*Element, type=";
      if(grp[0]->planestrain) {
	os << (sixnodes? "CPE6" : "CPE3") << endl;
      }
      else {
	os << (sixnodes? "CPS6" : "CPS3") << endl;
      }
      for(int j=0; j<grp.size(); j++) {
	os << *grp[j] << endl;
      }
    }
  }
}

void Grid::print_elementgroups(ostream &os) const {
  if(elementgroup.capacity() > 0) {
    os << "**\n** element groups\n**" << endl;
    for(int i=0; i<elementgroup.capacity(); i++)
      os << *elementgroup[i];	// endl not needed
  }
}

void Grid::print_materials(ostream &os) const {
  os << "**\n** materials\n**" << endl;
  for(int i=0; i<materialgroup.capacity(); i++) {
    ElementGroup &grp = *materialgroup[i];
    if(empties || !grp[0]->isempty()) {
      os << grp;
      os << "*Solid Section, elset=" << grp.name()
	 << ", material=" << materialname(i) << endl;
    }
  }
}

void Grid::list_materials(ostream &os) const {
  os << endl;
  os << "Materials defined in the goof file and names assigned to them here:"
     << endl;
  for(int i=0; i<materialgroup.capacity(); i++) {
    ElementGroup &grp = *materialgroup[i];
    Element *el = grp[0];
    if(empties || !el->isempty()) {
      os << "  " << materialname(i) << ": " << el->tag()
	 << " gray=" << el->intrinsic_gray << " " << el->parameters() << endl;
    }
  }
}

void Grid::get_dimensions(MeshCoord &ll, MeshCoord &ur) const {
  ll.x = 1.e10;
  ur.x = -1.e10;
  ll.y = 1.e10;
  ur.y = -1.e10;
  for(int i=0; i<node.capacity(); i++) {
    double x = node[i]->position.x;
    double y = node[i]->position.y;
    if(x < ll.x) ll.x = x;
    if(x > ur.x) ur.x = x;
    if(y < ll.y) ll.y = y;
    if(y > ur.y) ur.y = y;
  }
}
