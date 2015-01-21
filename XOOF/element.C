// -*- C++ -*-
// $RCSfile: element.C,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2005-02-15 22:19:50 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "element.h"
#include "eulerangle.h"
#include "grid.h"
#include "node.h"
#include "elementgroups.h"
#include "mvmult.h"
#include "mutableelement.h"
#include "parameters.h"
#include "readbinary.h"
#include "tocharstring.h"

// default values for element creation commands
float Element::gray_dflt = 0.5;
Vec<int> Element::nodenumber_dflt;
int Element::index_dflt;
unsigned char Element::inputformatflag(0);
int Element::cloneindex;

// Constructor for base element.

Element::Element(Grid *grid)
  : index(index_dflt),
    selected(0),
    intrinsic_gray(gray_dflt),
    current_stress(TF_FALSE),
    current_elastic_strain(TF_FALSE)
#ifdef THERMAL	
  , current_J(TF_FALSE),
    current_grad_thermal(TF_FALSE),
    J_field(3),
    Grad_T_field(3)
#endif
  
{
    int order = nodenumber_dflt.capacity();
    corner.resize(order);
    for(int j=0; j<order; j++) {
	corner[j] = grid->getnode(nodenumber_dflt[j]);
    }
    ++grid->elements_changed;	// update timestamp
}

Element::~Element() {
  /* this has to be done in reverse order, since Remove removes the group
   * from the element as well as removing the element from the group.
   */
  for(int i=groups.capacity()-1; i>=0; i--)
    groups[i]->Remove(this);
}

Element *Element::binaryread(FILE *file, TrueFalse &ok) {
    ok = TF_TRUE;
    if(!readbinary(file, gray_dflt)) ok = TF_FALSE;
    return 0;
}

void Element::reset_defaults() {
  // reset static default values used when creating elements
  MutableElement::reset_defaults();
}

void Element::binarywrite(FILE *file, char) const {
  writebinary(file, intrinsic_gray);
}

void Element::oof2write(ostream &os) const {
  os << "(" << corner[0]->index
     << ", " << corner[1]->index
     << ", " << corner[2]->index << ")";
}

ostream& operator<<(ostream& os, const Element& elem) {
    os << elem.tag() << " i=" << elem.index;
    for(int i=0; i<elem.corner.capacity(); i++) {
	os << " n" + to_charstring(i+1) + "=" << elem.corner[i]->index;
    }
    os << " gray=" << elem.intrinsic_gray;
    os << elem.parameters();
    return os;
}


int Element::hashfunc(const MeshCoord &where, double &dist) {
    if(contains(where)) {
	dist = 0.0;
	return 1;
    }
    else {
	dist = 1.0;
	return 0;
    }
}

void Element::not_current() {
  current_stress = TF_FALSE;
  current_elastic_strain = TF_FALSE;
#ifdef THERMAL
  current_J = TF_FALSE;
  current_grad_thermal = TF_FALSE;
#endif // THERMAL
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

double Element::elastic_energy() {
  elastic_straintensor();
  stresstensor();
  double e = 0;
  for(int i=0; i<3; i++) {
    e += stress(i,i)*elastic_strain(i,i);
    int j = (i+1)%3;
    e += 2*stress(i,j)*elastic_strain(i,j);
  }
  return 0.5*e;
}

#ifdef THERMAL
double Element::thermal_energy() {
  gradient_temperature_field(); //gradT has to have the right sign!!
  heat_flux_field();
  double e = 0;
  for(int i=0; i<3; i++) {
    e += J_field(i)*Grad_T_field(i); 
  }
  
  return 0.5*e;
  
}
#endif

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

EulerAngle Element::query_orientation() const
{
  return EulerAngle();
}

//---------------

CharString newPropertyName() {
  static int nprops = 0;
  return "property" + to_charstring(++nprops);
}

std::vector<CharString> *Element::print_properties(ostream &os) const {
  std::vector<CharString> *names = new std::vector<CharString>;
  CharString colorname = newPropertyName();
  names->push_back("Color:"+colorname);
  os << "OOF.LoadData.Property.Color(name='" << colorname
     << "', color=Gray(value=" << intrinsic_gray << "))" << endl;
  return names;
}

void Element::print_material(ostream &os, const CharString &matname) const
{
  std::vector<CharString> *propnames = print_properties(os);
  os << "OOF.LoadData.Material(name='" << matname << "', properties=[";
  for(int i=0; i<propnames->size(); i++) {
    os << "'" << (*propnames)[i] << "'";
    if(i < propnames->size()-1)
      os << ", ";
  }
  os << "])" << endl;
  delete propnames;
}

bool Element::same_type(const Element *other) const {
  return intrinsic_gray == other->intrinsic_gray;
}

bool operator==(const Element &el1, const Element &el2) {
  return el1.tag() == el2.tag() && el1.same_type(&el2);
}
