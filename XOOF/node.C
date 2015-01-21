// -*- C++ -*-
// $RCSfile: node.C,v $
// $Revision: 1.10 $
// $Author: langer $
// $Date: 2000-12-15 21:57:53 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "distort.h"
#include "freedom.h"
#include "grid.h"
#include "meshcoord.h"
#include "mvmd.h"
#include "mvmult.h"
#include "mvvd.h"
#include "node.h"
#include "parameters.h"
#include "preconditioner.h"
#include "readbinary.h"
#include <iostream.h>

Node::Node()
  : element(0, BlockSize(3))
{}

Node:: Node(int n, DegreeOfFreedom *xx, DegreeOfFreedom *yy
#ifdef THERMAL
	    , DegreeOfFreedom *phiphi
#endif // THERMAL
	    )
    : index(n),
      x(xx),
      y(yy),
#ifdef THERMAL
      phi(phiphi),
#endif // THERMAL
      selected(0),
      element(0, BlockSize(3))
{}

void Node::reset_displacement(const int do_x, const int do_y) {
  if(do_x) x->disp() = 0.0;
  if(do_y) y->disp() = 0.0;
}

void Node::reset_force() {
  x->force() = 0.0;
  y->force() = 0.0;
}

#ifdef THERMAL
void Node::reset_voltage() {
  phi->disp() = 0.0;		//displacement is analogous to voltage
}

void Node::reset_surfcharge() {
  phi->force() = 0.0;		//force is analogous to surface charge
}
#endif // THERMAL

int Node::fixed() const {
  return ((x->fixed() || y->fixed())
#ifdef THERMAL
	  || (phi->fixed())
#endif // THERMAL
	  );
}

// set boundary condition flags
void Node::fix_x() {
  x->fix();
}

void Node::fix_y() {
  y->fix();
}

void Node::free_x() {
  x->unfix();
}

void Node::free_y() {
  y->unfix();
}

MeshCoord Node::force() const {
  grid->update_node_elists();	// make sure we know who our elements are
  MeshCoord f;
  for(int i=0; i<element.capacity(); i++)
    f += element[i]->force(this);
  return f;
}

#ifdef THERMAL
void Node::fix_phi() {
  phi->fix();
}

void Node::free_phi() {
  phi->unfix();
}

double Node::voltage() const {
  return phi->disp();
}

double Node::surf_charge() const {
  grid->update_node_elists();	// make sure we know who our elements are
  double rho = 0.0;
  
  for(int i=0; i<element.capacity(); i++)
    rho += element[i]->heat(this);
  return rho;
}

void Node::v_move(const double &d) {
  phi->disp()+=d;
}

void Node::v_distort(const ThermalDistortion &td) {
  v_move(td.T0 + original().x*td.dTdx + original().y*td.dTdy);
}

void Node::apply_surfcharge(const double &f) {
  phi->force() +=f;		// positive heatrate (f) is like negative force
}

#endif // THERMAL

bool Node::disconnected() const {
  return (x->disconnected() && y->disconnected());
}

Element *Node::neighboring(const Node *nbr) const {
  // does this node have the given node as a neighbor?
  for(int i=0; i<element.capacity(); i++) {
    Element *elem = element[i];
    for(int j=0; j<elem->corner.capacity(); j++) {
      if(elem->corner[j] == nbr) {
	return elem;
      }
    }
  }
  return false;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

ostream &operator<<(ostream &os, const Node &n) {
    MeshCoord p(n.original());
    MeshCoord d(n.displacement());
    os << n.tag() << " i=" << n.index << " x=" << p.x << " y=" << p.y
       << " dx=" << d.x << " dy=" << d.y;
#ifdef THERMAL
    os << " dT=" << n.phi->disp();
#endif // THERMAL
    os << " fx=" << n.x->DegreeOfFreedom::force()
       << " fy=" << n.y->DegreeOfFreedom::force();
#ifdef THERMAL
    os << " fT=" << n.phi->DegreeOfFreedom::force();
#endif // THERMAL
    os << n.parameters();
    return os;
}

CharString Node::flags() const {
  return "x=" + x->flagstr() + " y=" + y->flagstr()
#ifdef THERMAL
    + " T=" + phi->flagstr()
#endif // THERMAL
    ;
}

#ifdef DEBUG
#ifdef THERMAL
void Node::dump(ostream &os) const {
  os << *this << "\tdT=" << phi << endl;
}
#else
void Node::dump(ostream&) const {}
#endif // THERMAL
#endif // DEBUG

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

CharString XYNode::nameplate("xy");
NodeTypeRegistration XYNode::reg(nameplate, binaryread);

int XYNode::binaryread(FILE *file) {
    int index;
    char flag;
    float x, y, dx, dy;
    float fx, fy;
#ifdef THERMAL
    float dphi, fphi;
#endif // THERMAL
    if(!readbinary(file, flag)) return 0;
    if(!readbinary(file, index)) return 0;
    if(!readbinary(file, x)) return 0;
    if(!readbinary(file, y)) return 0;
    if(flag & 0x1) {
	if(!readbinary(file, dx)) return 0;
	if(!readbinary(file, dy)) return 0;
#ifdef THERMAL
	if(!readbinary(file, dphi)) return 0;
#endif // THERMAL
    }
    else {
	dx = dy = 0.0;
#ifdef THERMAL
	dphi = 0;
#endif // THERMAL
    }
    if(flag & 0x2) {
      if(!readbinary(file, fx)) return 0;
      if(!readbinary(file, fy)) return 0;
#ifdef THERMAL
      if(!readbinary(file, fphi)) return 0;
#endif // THERMAL
    }
    Node *newnode = XYNode::create(index, x, y, dx, dy
#ifdef THERMAL
				   , dphi
#endif // THERMAL
				   );
    if(flag & 2) {
      newnode->applyforce(MeshCoord(fx, fy)); // may be wrong for slaves
#ifdef THERMAL
      newnode->apply_surfcharge(fphi);
#endif // THERMAL
    }
    return 1;
}

void XYNode::binarywrite(FILE *file) {
    float xx = x->orig;
    float yy = y->orig;
    float dx = x->disp();
    float dy = y->disp();
    float fx = x->DegreeOfFreedom::force();
    float fy = y->DegreeOfFreedom::force();
#ifdef THERMAL
    float dphi = phi->disp();
    float fphi = phi->DegreeOfFreedom::force();
#endif // THERMAL
    char flag = 0;
    if(dx != 0.0 || dy != 0.0
#ifdef THERMAL			//    boy, this is hard to read...
       || dphi != 0.0
#endif // THERMAL
       ) flag = 0x1;
    if(fx != 0.0 || fy != 0.0
#ifdef THERMAL
       || fphi != 0.0
#endif // THERMAL
       ) flag |= 0x2;
    writebinary(file, flag);
    writebinary(file, index);
    writebinary(file, xx);
    writebinary(file, yy);
    if(flag & 1) {
      writebinary(file, dx);
      writebinary(file, dy);
#ifdef THERMAL
      writebinary(file, dphi);
#endif // THERMAL
    }
    if(flag & 2) {
      writebinary(file, fx);
      writebinary(file, fy);
#ifdef THERMAL
      writebinary(file, fphi);
#endif // THERMAL
    }
}


XYNode *XYNode::create(const int index, double x, double y, 
		       double dx, double dy
#ifdef THERMAL
		       , double dphi
#endif // THERMAL
		       )
{
    // create degrees of freedom with bad indices
    DegreeOfFreedom *xdof = new DegreeOfFreedom(x, dx);
    DegreeOfFreedom *ydof = new DegreeOfFreedom(y, dy);
#ifdef THERMAL
    DegreeOfFreedom *phidof = new DegreeOfFreedom(0.0, dphi);
#endif // THERMAL
    // store them in the grid
    grid->AddDoFelastic(xdof);
    grid->AddDoFelastic(ydof);
#ifdef THERMAL
    grid->AddDoFthermal(phidof);
#endif // THERMAL
    // create a node
    XYNode *newnode =  new XYNode(index, xdof, ydof
#ifdef THERMAL
				  , phidof
#endif // THERMAL
				  );
    grid->AddNode(index,newnode);
    return newnode;
}

MeshCoord XYNode::original() const {	// original position
  return MeshCoord(x->orig, y->orig);
}

MeshCoord XYNode::displacement() const {
  return MeshCoord(x->disp(), y->disp());
}

void XYNode::distort(const TwoTwo &dm, const MeshCoord &shift,
		     const int do_x, const int do_y) {
    MeshCoord disp = dm*original() + shift; // new displacements
    if(!do_x) disp.x = 0.0;
    if(!do_y) disp.y = 0.0;
    move(disp);
}

void XYNode::applyforce(const MeshCoord &f) {
  x->force() += f.x;
  y->force() += f.y;
}

void XYNode::move(const MeshCoord &d) {
  x->disp() += d.x;
  y->disp() += d.y;
}
	
// ------------------------------------------------------------//

CharString LinearNode::nameplate("linear");
NodeTypeRegistration LinearNode::reg(nameplate, binaryread);

int LinearNode::binaryread(FILE *file) {
    int index;
    char flag;
    float x, y, dx, dy;
    float t00, t01, t10, t11;
    float fx, fy;
#ifdef THERMAL
    float dphi, fphi;
#endif // THERMAL
    if(!readbinary(file, flag)) return 0;
    if(!readbinary(file, index)) return 0;
    if(!readbinary(file, x)) return 0;
    if(!readbinary(file, y)) return 0;
    if(flag & 0x1) {
	if(!readbinary(file, dx)) return 0;
	if(!readbinary(file, dy)) return 0;
#ifdef THERMAL
	if(!readbinary(file, dphi)) return 0;
#endif // THERMAL
    }
    else
	dx = dy = 0.0;
    if(flag & 0x2) {
	if(!readbinary(file, t00)) return 0;
	if(!readbinary(file, t01)) return 0;
	if(!readbinary(file, t10)) return 0;
	if(!readbinary(file, t11)) return 0;
    }
    else {
	t00 = t11 = 1.0;
	t01 = t10 = 0.0;
    }
    if(flag & 0x4) {
      if(!readbinary(file, fx)) return 0;
      if(!readbinary(file, fy)) return 0;
#ifdef THERMAL
      if(!readbinary(file, fphi)) return 0;
#endif // THERMAL
    }
    LinearNode *newnode = 
      create(index, x, y, dx, dy,
#ifdef THERMAL
	     dphi,
#endif // THERMAL
	     TwoTwo(t00, t01, t10, t11));
    if(flag & 0x4) {
      newnode->applyforce(MeshCoord(fx, fy)); // may be wrong for slaves
#ifdef THERMAL
      newnode->apply_surfcharge(fphi);
#endif // THERMAL
    }
    return 1;
}

void LinearNode::binarywrite(FILE *file) {
    float xx = x->orig;
    float yy = y->orig;
    float dx = x->disp();
    float dy = y->disp();
    float t00 = T(0, 0);
    float t01 = T(0, 1);
    float t10 = T(1, 0);
    float t11 = T(1, 1);
    float fx = x->DegreeOfFreedom::force();
    float fy = y->DegreeOfFreedom::force();
#ifdef THERMAL
    float dphi = phi->disp();
    float fphi = phi->DegreeOfFreedom::force();
#endif // THERMAL
    char flag = 0;
    if(dx != 0.0 || dy != 0.0
#ifdef THERMAL
       || dphi != 0.0
#endif // THERMAL
       ) flag |= 0x1;
    if(t00 != 0.0 || t01 != 0.0 || t10 != 0.0 || t11 != 0.0) flag |= 0x2;
    if(fx != 0.0 || fy != 0.0
#ifdef THERMAL
       || fphi != 0.0
#endif // THERMAL
       ) flag |= 0x4;
    writebinary(file, flag);
    writebinary(file, index);
    writebinary(file, xx);
    writebinary(file, yy);
    if(flag & 0x1) {
	writebinary(file, dx);
	writebinary(file, dy);
#ifdef THERMAL
	writebinary(file, dphi);
#endif // THERMAL
    }
    if(flag & 0x2) {
	writebinary(file, t00);
	writebinary(file, t01);
	writebinary(file, t10);
	writebinary(file, t11);
    }
    if(flag & 0x4) {
      writebinary(file, fx);
      writebinary(file, fy);
#ifdef THERMAL
      writebinary(file, fphi);
#endif // THERMAL
    }
}

LinearNode *LinearNode::create(const int index,
			       double x, double y,
			       double dx, double dy,
#ifdef THERMAL
			       double dphi,
#endif // THERMAL
			       const TwoTwo &tm) {
    // create new degrees of freedom with bad indices
    DegreeOfFreedom *xdof = new DegreeOfFreedom(x, dx);
    DegreeOfFreedom *ydof = new DegreeOfFreedom(y, dy);
#ifdef THERMAL
    DegreeOfFreedom *phidof = new DegreeOfFreedom(0.0, dphi);
#endif // THERMAL
    // store them in the grid
    grid->AddDoFelastic(xdof);
    grid->AddDoFelastic(ydof);
#ifdef THERMAL
    grid->AddDoFthermal(phidof);
#endif // THERMAL
    // create a node
    LinearNode *newnode =  new LinearNode(index, xdof, ydof,
#ifdef THERMAL
					  phidof,
#endif // THERMAL
					  &tm);
    grid->AddNode(index, newnode);
    return newnode;
}

#ifndef THERMAL
LinearNode::LinearNode(const int n, DegreeOfFreedom *xx, DegreeOfFreedom *yy,
		       const TwoTwo *tr)
  : Node(n, xx, yy), T(tr ? *tr : I2x2)
{
}
#else // THERMAL
LinearNode::LinearNode(const int n, DegreeOfFreedom *xx, DegreeOfFreedom *yy,
		       DegreeOfFreedom *pp, const TwoTwo *tr)
  : Node(n, xx, yy, pp), T(tr ? *tr : I2x2)
{
}
#endif // THERMAL

MeshCoord LinearNode::original() const {
  return MeshCoord(x->orig, y->orig);
}

MeshCoord LinearNode::displacement() const {
  MeshCoord p(x->disp(), y->disp());
  return T*p;
}

void LinearNode::move(const MeshCoord &d) {
    MeshCoord dd(T.inverse()*d);
    x->disp() += dd.x;
    y->disp() += dd.y;
}

MeshCoord LinearNode::force() const {
  return T*Node::force();
}

void LinearNode::distort(const TwoTwo &dm, const MeshCoord &shift,
			 const int do_x, const int do_y) {
    // use shear components to set new direction
    MeshCoord olddisp, newdisp;	// old and new displacements
    TwoTwo Tnew( (I2x2 + dm) * T ); // new transformation matrix
    olddisp.x = x->disp();
    olddisp.y = y->disp();
    newdisp = Tnew.inverse() * (T*olddisp + dm*original() + shift);
    olddisp = Tnew.inverse() * (T*olddisp);
    T = Tnew;
    if(do_x)
	x->disp() = newdisp.x;
    else
	x->disp() = olddisp.x;

    if(do_y)
	y->disp() = newdisp.y;
    else
	y->disp() = olddisp.y;
}

void LinearNode::applyforce(const MeshCoord &f) {
    MeshCoord g = T.inverse()*f;
    x->force() += g.x;
    y->force() += g.y;
}

CharString LinearNode::parameters() const {
  return Node::parameters()
    + " T00=" + to_charstring(T(0, 0))
    + " T01=" + to_charstring(T(0, 1))
    + " T10=" + to_charstring(T(1, 0))
    + " T11=" + to_charstring(T(1, 1));
}

// --------------------------------------------------------- //

/* Block preconditioner set up
 * Dimension of block is number of free dofs at the node
 * This just allocates the block and stores the indices of the dofs.
 */

#ifndef THERMAL

BP_block *Node::bpblock() {
  if(x->fixed()) {
    if(y->fixed())		// both dof's fixed -- no block!
      return 0;
    else			// only y dof free
      return new BP_block(y->dofindex());
  }
  else {			// x free
    if(y->fixed())		// x fixed, y free
      return new BP_block(x->dofindex());
    else			// both free
      return new BP_block(x->dofindex(), y->dofindex());
  }
}
#endif // !THERMAL

// --------------------------------------------------------- //

int Node::hashfunc(const MeshCoord &where, double &dist) {
    MeshCoord r(where - position(grid->enhancement()));
    dist = r.x*r.x + r.y*r.y;
    return 1;
}
