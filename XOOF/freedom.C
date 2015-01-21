// -*- C++ -*-
// $RCSfile: freedom.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2002-02-05 20:22:18 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// In this discussion, "Node" is sometimes used when "x or y
// DegreeOfFreedom of Node" is meant. If that's confusing, complain.

// A DegreeOfFreedom (DoF for short) has a original position, a
// displacement, a force, and a bunch of flags dictating its
// behavior. A DoF is "fixed" if it does not move during
// equilibration. Since a DoF can belong to a node that is in more
// than one NodeGroup, and since boundary conditions are applied to
// NodeGroups, a DoF can be fixed more than once. It is "free" (ie not
// fixed) only if it is not in any fixed NodeGroups.

// There are two classes derived from the DegreeOfFreedom. A
// SlaveDegreeOfFreedom (aka Slave) is one whose displacement is tied
// to that of its MasterDegreeOfFreedom (aka Master). When the x or y
// DoFs in a NodeGroup are enslaved, each DoF in the group is replaced
// by a new Slave, each of which points to the same Master, which is
// stored in the group. The master is fixed if any of the original
// DoF's are fixed. If at a later time any Slave is fixed, the Master
// is also fixed. Therefore the routines that set flags in the DoFs
// are virtual, so that setting flags in a Slave also sets flags in a
// Master, and querying a Slave's flags returns the Master's flags.

// When displacements are applied to a Slave, the Master's
// displacement must be incremented only once, even if the Master has
// more than one Slave. The INCREMENTED flag is used to make sure that
// the increment isn't repeated. This also guarantees that Nodes that
// are in more than one NodeGroup don't get incremented multiple
// times. (See Grid::apply_incremental_distortion().) When a NodeGroup
// is enslaved, the Master's displacement is set to the average
// displacement of group. (This is just temporary -- presumably the
// system will soon be equilibrated.) When a NodeGroup is emancipated,
// the former Slaves displacement is set to the former Master's
// displacement.

// On the other hand, when forces are applied to a Slave, the Master's
// force is the sum of all the forces on all its Slaves. When a group
// is enslaved, the force on the Master is the total force on all the
// Slaves. (It's not just the force on the enslaved NodeGroup, since
// some of the Nodes may have forces from other groups. The Master
// must encompass all forces acting on all of its Slaves.) When a
// group is emancipated, the force on the freed Slaves is the force
// set in the NodeGroup. This may be the wrong thing to do for Nodes
// in more than one group.

// When a group is enslaved, the DoFs that are replaced by Slaves are
// marked as DEFUNCT, instead of being deleted immediately. This is
// because it would be costly to remove them one at a time from the
// DoF lists in the Grid. All DEFUNCT DoFs are removed from the list
// before equilibration by Grid::shufflelists().

// After a group is enslaved or emancipated, Grid::fix_masters() must
// be called to update the fixed flags in the master DoF's.  This
// can't be done at the time of enslavement or emancipation, because a
// DoF may be in more than one group, and it may be enslaved by one
// group and fixed by another. The rule is that if any DoF in an
// enslaved group is fixed, then its master is fixed.


#include "freedom.h"
#include "grid.h"
#include "charstring.h"
#include "parameters.h"
#include "tocharstring.h"
#include <iomanip.h>

DegreeOfFreedom::DegreeOfFreedom()
  : index_(-1), orig(0), disp_(0), force_(0), flags_(0), nfix(0)
{
//  #ifdef DEBUG
//    cerr << "Allocating dof " << this << endl;
//  #endif
}

DegreeOfFreedom::DegreeOfFreedom(double s, double d, int nf)
  : index_(-1), orig(s), disp_(d), force_(0.0), flags_(0), nfix(nf)
{
//  #ifdef DEBUG
//    cerr << "Allocating dof " << this << endl;
//  #endif
}

DegreeOfFreedom::~DegreeOfFreedom() {
//  #ifdef DEBUG
//    cerr << "Deleting dof " << this << endl;
//  #endif
}

void DegreeOfFreedom::fix() {
  ++nfix;
}

void DegreeOfFreedom::unfix() {
  if(nfix > 0)
    --nfix;
}

void DegreeOfFreedom::setflag(const unsigned char f) {
  flags_.set(f);
}

void DegreeOfFreedom::setflag(const FreedomFlags &fl) {
  flags_.set(fl);
}

void DegreeOfFreedom::clearflag(const unsigned char f) {
  flags_.clear(f);
}

void DegreeOfFreedom::clearflag(const FreedomFlags &fl) {
  flags_.clear(fl);
}

void DegreeOfFreedom::setflags(const DegreeOfFreedom &dof) {
  flags_ = dof.flags_;
  nfix = dof.nfix;
}

CharString DegreeOfFreedom::tag() const {
  return CharString("autonomous");
}

CharString DegreeOfFreedom::flagstr() const {
  CharString s;
  if(fixed())
    s = "fixed(" + to_charstring(nfix) + ")";
  else
    s = "free";
  s += "/" + tag();
  return s;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

DegreeOfFreedom *MasterDegreeOfFreedom::controller() {
  if(master) return master->controller();
  return this;
}

const DegreeOfFreedom *MasterDegreeOfFreedom::controller() const {
  if(master) return master->controller();
  return this;
}

CharString MasterDegreeOfFreedom::tag() const {
  return CharString("master");
}

void MasterDegreeOfFreedom::remove_slave() {
  nslaves--;
  if(nslaves == 0) {
    // Don't use virtual function, since we don't want to set the
    // DEFUNCT flag in this dofs master.
    DegreeOfFreedom::setflag(DEFUNCT);
    if(master) {
      master->remove_slave();
      master = 0;
    }
  }
}

void MasterDegreeOfFreedom::add_slave() {
  nslaves++;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void MasterSlaveDoF::setflag(const unsigned char f) {
  DegreeOfFreedom::setflag(f);
  DegreeOfFreedom *cntrllr = controller();
  if(cntrllr != this)
    cntrllr->setflag(f);
}

void MasterSlaveDoF::setflag(const FreedomFlags &fl) {
  DegreeOfFreedom::setflag(fl);
  DegreeOfFreedom *cntrllr = controller();
  if(cntrllr != this)
    cntrllr->setflag(fl);
}

void MasterSlaveDoF::clearflag(const unsigned char f) {
  DegreeOfFreedom::clearflag(f);
  DegreeOfFreedom *cntrllr = controller();
  if(cntrllr != this)
    cntrllr->clearflag(f);
}

void MasterSlaveDoF::clearflag(const FreedomFlags &fl) {
  DegreeOfFreedom::clearflag(fl);
  DegreeOfFreedom *cntrllr = controller();
  if(cntrllr != this)
    cntrllr->clearflag(fl);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

SlaveDegreeOfFreedom::SlaveDegreeOfFreedom(MasterDegreeOfFreedom *m,
					   double s, double d, int nf)
  : MasterSlaveDoF(s, d),	// s = original position
    master(1, m)
{
  disp_ = d;
  nfix = nf;
}

CharString SlaveDegreeOfFreedom::tag() const {
  return "slaved";
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// The second argument to enslave() is a Grid function to be applied
// to new dof's that may be created as a by-product of the
// enslavement.
 
DegreeOfFreedom *DegreeOfFreedom::enslave(MasterDegreeOfFreedom *mstr,
			  DegreeOfFreedom *(Grid::*f)(DegreeOfFreedom*))
{
  mstr->add_slave();
  mstr->force() += force();
  setflag(DEFUNCT);
  DegreeOfFreedom *newdof = new SlaveDegreeOfFreedom(mstr, orig, disp_, nfix);
  (grid->*f)(newdof);
  return newdof;
}

DegreeOfFreedom *MasterDegreeOfFreedom::enslave(MasterDegreeOfFreedom *mstr,
				DegreeOfFreedom *(Grid::*f)(DegreeOfFreedom*))
{
  if(master)
    master->enslave(mstr, f);
  else {
    master = mstr;
    master->add_slave();
    master->force() += force();
  }
  return this;
}

DegreeOfFreedom *SlaveDegreeOfFreedom::enslave(MasterDegreeOfFreedom *mstr,
			       DegreeOfFreedom *(Grid::*f)(DegreeOfFreedom*))
{
  // this dof is getting a second master
  master.grow(1, mstr);
  mstr->add_slave();
  if(controller() != mstr->controller()) {
    // have to create a common controlling master dof and slave both
    // the old master and the new one to it.
    MasterDegreeOfFreedom *topdog = new MasterDegreeOfFreedom;
    (grid->*f)(topdog);
    controller()->enslave(topdog, f);
    mstr->enslave(topdog, f);
  }
  return this;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// The emancipate functions return the DoF that should replace the one
// being emancipated, or zero if the DoF shouldn't be replaced.

DegreeOfFreedom *DegreeOfFreedom::emancipate(MasterDegreeOfFreedom*) {
  return 0;			// already emancipated!
}

DegreeOfFreedom *SlaveDegreeOfFreedom::emancipate(MasterDegreeOfFreedom *mstr) {
  if(master.capacity() == 1) {	// removing this dof's last master
    if(master[0] == mstr) {	// check that it's the right one
      // Replace this dof with a vanilla dof.
      DegreeOfFreedom *newdof;
      if(DegreeOfFreedom::fixed())
	// replace fixed dof's at their original displacement
	newdof = new DegreeOfFreedom(orig, disp_, nfix);
      else
	// replace free dof's at the displacement of the master
	newdof = new DegreeOfFreedom(orig, disp(), nfix);
      newdof->setflags(*this);
      // Mark this DoF as defunct. Don't use
      // SlaveDegreeOfFreedom::setflag(), because that would set flags
      // in the master as well, and the master may not be defunct yet.
      DegreeOfFreedom::setflag(DEFUNCT);
      master.remove(mstr);
      mstr->remove_slave();
      return newdof;		// return replacement for this DoF
    }
  }
  else if(master.remove(mstr)) { // remove one of many masters
    mstr->remove_slave();
    return 0;			// not replacing this DoF
  }
  cerr << "Error in SlaveDegreeOfFreedom::emancipate!" << endl;
  fatalerror();
  /*NOTREACHED*/
}

DegreeOfFreedom *MasterDegreeOfFreedom::emancipate(MasterDegreeOfFreedom*) {
  // this should never be called...
  cerr << "Error: MasterDegreeOfFreedom::emancipate() called" << endl;
  fatalerror();
  /*NOTREACHED*/
  return 0;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

ostream &operator<<(ostream &os, const DegreeOfFreedom &dof) {
  os << "[" << dof.dofindex();
  if(dof.fixed())
    os << " fixed";
  else
    os << " free";
  os << "/" << dof.tag();
  os << "]";
  return os;
}

ostream &operator<<(ostream &os, const FreedomFlags &ff) {
  if(ff.fixed())
    os << "fixed";
  else
    os << "free";
  return os;
}

#ifdef DEBUG
void DegreeOfFreedom::dump(ostream &os, CharString prefix) const {
  os << prefix
     << "\t vanilla\taddr=" << this
     << " index=" << setw(3) << dofindex()
     << " nfix=" << nfix;
  if(defunct()) os << " (defunct)";
  os << endl;
}

void SlaveDegreeOfFreedom::dump(ostream &os, CharString prefix) const {
  os << prefix
     << "\t slave  \taddr=" << this
     << " index=" << setw(3) << dofindex()
     << " nfix=" << nfix
     << " masters=";
  for(int i=0; i<master.capacity(); i++)
    os << master[i] << " ";
  if(defunct()) os << " (defunct)";
  os << endl;
  for(int i=0; i<master.capacity(); i++)
    master[i]->dump(os, prefix+" ");
}

void MasterDegreeOfFreedom::dump(ostream &os, CharString prefix) const {
  os << prefix
     << "\t master \taddr=" << this
     << " index=" << setw(3) << dofindex()
     << " nfix=" << nfix
     << " nslaves=" << nslaves
     << " master=" << master;
  if(defunct())
    os << " (defunct)";
  os << endl;
  if(master)
    master->dump(os, prefix+" ");
}
#endif // DEBUG
