// -*- C++ -*-
// $RCSfile: trielementT.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2004-10-19 02:24:35 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "freedom.h"
#include "node.h"
#include "parameters.h"
#include "readbinary.h"
#include "trielementT.h"

#ifdef THERMAL
double TriElementT::temperature0_dflt(0.0);
#endif // THERMAL

#ifdef THERMAL
TriElementT::TriElementT(Grid *g)
  : TriElement(g),
    temperature0(temperature0_dflt)
{}
#else // !THERMAL
TriElementT::TriElementT(Grid *g)
  : TriElement(g)
{}
#endif // THERMAL

Element *TriElementT::binaryread(FILE *file, TrueFalse &ok) {
  TriElement::binaryread(file, ok);
#ifdef THERMAL
  if(ok) {
    float t0;
    if(!readbinary(file, t0)) ok = TF_FALSE;
    temperature0_dflt = t0;
  }
#endif // THERMAL
  return 0;
}

void TriElementT::binarywrite(FILE* file, char formatflag) const {
  TriElement::binarywrite(file, formatflag);
#ifdef THERMAL
  float t0 = temperature0;
  writebinary(file, t0);
#endif // THERMAL
}

CharString TriElementT::parameters() const {
  return TriElement::parameters()
#ifdef THERMAL
    + " T0=" + to_charstring(temperature0)
#endif // THERMAL
    ;
}

#ifdef THERMAL
double TriElementT::average_T() const {
  double tsum = 0;
  for(int i=0; i<3; i++)
    tsum += corner[i]->phi->disp();
  return tsum/3.0;
}
#endif // THERMAL

#ifdef THERMAL
double TriElementT::deltaT() const {
  return average_T() - temperature0;
}
#else // !THERMAL
double TriElementT::deltaT() const {
  return delta_T;
}
#endif

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Compute the force on a node

MeshCoord TriElementT::force(const Node *n) const {
  MeshCoord f;
  int whichnode;
  for(whichnode=0; whichnode<3; whichnode++)
    if(n == corner[whichnode])
      break;
  if(whichnode == 3) {
    cerr << "Error in TriElement::force!" << endl;
    return f;
  }
  for(int i=0; i<3; i++) {
    double dx = corner[i]->x->disp();
    double dy = corner[i]->y->disp();
    f.x += K(0, 0, whichnode, i)*dx + K(0, 1, whichnode, i)*dy;
    f.y += K(1, 0, whichnode, i)*dx + K(1, 1, whichnode, i)*dy; 
  }
  // include thermal forces
  f.x -= deltaT()*thermal_forces[2*whichnode];
  f.y -= deltaT()*thermal_forces[2*whichnode+1];
  return f;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void TriElementT::loadinterior_rhs(MV_Vector_double &ell) const {
  for(int mu=0; mu<3; mu++) {
    if(!corner[mu]->x->fixed())
      ell[corner[mu]->x->dofindex()] += deltaT()*thermal_forces[2*mu];
    if(!corner[mu]->y->fixed())
      ell[corner[mu]->y->dofindex()] += deltaT()*thermal_forces[2*mu+1];
  }
}

void TriElementT::loadboundary_rhs(MV_Vector_double &ell) const {
  for(int mu=0; mu<3; mu++) {
    if(corner[mu]->x->fixed())
      ell[corner[mu]->x->dofindex()] += deltaT()*thermal_forces[2*mu];
    if(corner[mu]->y->fixed())
      ell[corner[mu]->y->dofindex()] += deltaT()*thermal_forces[2*mu+1];
  }
}

//-----------------

bool TriElementT::same_type(const Element *el) const {
  const TriElementT *other = dynamic_cast<const TriElementT*>(el);
  return other && TriElement::same_type(el)
#ifdef THERMAL
    && temperature0 == other->temperature0
#endif // THERMAL
    ;
}

std::vector<CharString> *TriElementT::print_properties(ostream &os) const {
  // The temperature0 parameter contained in TriElementT is really
  // part of the thermalexpansion property, which has to be printed by
  // the derived class, which knows the symmetry.
  return TriElement::print_properties(os);
}
