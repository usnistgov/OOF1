// -*- C++ -*-
// $RCSfile: freedom.h,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-11-14 23:39:08 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// degree of freedom

// copious comments have been moved to freedom.C, so that the comments
// can be updated without recompiling almost everything. Almost all
// files depend on this one.

class DegreeOfFreedom;
class CharString;
class Grid;

#ifndef FREEDOM_H
#define FREEDOM_H

class DegreeOfFreedom;
class MasterDegreeOfFreedom;
class SlaveDegreeOfFreedom;
class FreedomFlags;

#include <iostream.h>
#include "vec.h"

const unsigned char FIX_DISP = 1;     // fixed displacement applied
const unsigned char INCREMENTED = 2;  // dof displacement has been incremented
const unsigned char DISCONNECTED = 4; // node is irrelevant
const unsigned char DEFUNCT = 8;      // delete this dof when shuffling lists
const unsigned char ENSLAVED = 16;

class FreedomFlags {
private:
  unsigned char flags;
public:
  FreedomFlags() : flags(0) {}
  FreedomFlags(const unsigned char f) : flags(f) {}
  bool isfree() const { return !(flags & FIX_DISP); }
  bool fixed() const { return flags & FIX_DISP; }
  bool defunct() const { return flags & DEFUNCT; }
  bool disconnected() const { return flags & DISCONNECTED; }
  bool incremented() const { return flags & INCREMENTED; }
  bool enslaved() const { return flags & ENSLAVED; }
  void set(const unsigned char f) { flags |= f; }
  void set(const FreedomFlags &fl) { set(fl.flags); }
  void clear(const unsigned char f) { flags &= ~f; }
  void clear(const FreedomFlags &fl) { clear(fl.flags); }
};

class DegreeOfFreedom {
protected:
  double disp_;
  double force_;
  int index_;
  FreedomFlags flags_;
  int nfix;			// number of times this dof has been fixed
public:
  DegreeOfFreedom();
  DegreeOfFreedom(double s, double d=0, int nf=0);
  virtual ~DegreeOfFreedom();

  virtual DegreeOfFreedom *controller() { return this; }
  virtual const DegreeOfFreedom *controller() const { return this; }
  virtual DegreeOfFreedom *enslave(MasterDegreeOfFreedom*,
			   DegreeOfFreedom* (Grid::*)(DegreeOfFreedom*));
  virtual DegreeOfFreedom *emancipate(MasterDegreeOfFreedom*);
  bool isslave() const {
    return (this != controller());
  }
  virtual void clear_master_fix() {} // ugly

  // dofindex is set by Grid::shuffle_dof before equilibrating
  virtual int dofindex() const { return index_;}
  virtual int &dofindex() { return index_; }

  double orig;	// original value
  virtual double disp() const { return disp_; }	// disp. from original
  virtual double &disp() { return disp_; }
  virtual double force() const { return force_; } // force applied
  virtual double &force() { return force_; }

  // For slaved dof's, flags() returns the master's flags, and
  // setflag() and clearflag() set flags in both the master and slave.
  virtual FreedomFlags flags() const { return flags_;}
  virtual FreedomFlags &flags() { return flags_; }
  virtual void setflag(const unsigned char f);
  virtual void setflag(const FreedomFlags &fl);
  void setflags(const DegreeOfFreedom&);
  virtual void clearflag(const unsigned char f);
  virtual void clearflag(const FreedomFlags &fl);

  void fix();
  void unfix();
  virtual bool isfree() const { return nfix == 0; }
  virtual bool fixed() const { return nfix != 0; }
  bool disconnected() const { return flags_.disconnected(); }
  bool defunct() const { return flags_.defunct(); }
  bool incremented() const { return flags().incremented(); }
  bool enslaved() const { return flags_.enslaved(); }

  CharString flagstr() const;
  virtual CharString tag() const;

#ifdef DEBUG
  virtual void dump(ostream&, CharString) const;
#endif

  friend class MasterSlaveDoF;
};

// Masters and Slaves are both derived from MasterSlaveDoF.  Both
// Masters and Slaves can have a master. All MasterSlaveDoF does is to
// pass requests on to the relevant master.

class MasterSlaveDoF : public DegreeOfFreedom {
public:
  MasterSlaveDoF(double s, double d) : DegreeOfFreedom(s, d) {}
  virtual ~MasterSlaveDoF() {}

  virtual CharString tag() const = 0;
  // The second argument to enslave is a Grid function to be applied
  // to new dof's that are created as a by-product of the enslavement.
  virtual DegreeOfFreedom *enslave(MasterDegreeOfFreedom*,
				   DegreeOfFreedom* (Grid::*)(DegreeOfFreedom*))
    = 0;
  virtual DegreeOfFreedom *emancipate(MasterDegreeOfFreedom*) = 0;
  virtual DegreeOfFreedom *controller() = 0;
  virtual const DegreeOfFreedom *controller() const = 0;

  virtual double disp() const { return controller()->disp_; }
  virtual double &disp() { return controller()->disp_; }
  virtual double force() const { return controller()->force_; }
  virtual double &force() { return controller()->force_; }
  virtual bool isfree() const { return controller()->nfix == 0; }
  virtual bool fixed() const { return controller()->nfix != 0; }
  virtual FreedomFlags flags() const { return controller()->flags_; }
  virtual FreedomFlags &flags() { return controller()->flags_; }
  virtual int dofindex() const { return controller()->index_;}
  virtual int &dofindex() { return controller()->index_; }
  virtual void setflag(const unsigned char f);
  virtual void setflag(const FreedomFlags &fl);
  virtual void clearflag(const unsigned char f);
  virtual void clearflag(const FreedomFlags &fl);
};

class MasterDegreeOfFreedom : public MasterSlaveDoF {
private:
  MasterDegreeOfFreedom *master; // masters can be enslaved too!
  int nslaves;
  void add_slave();
  void remove_slave();
public:
  MasterDegreeOfFreedom(double d=0) // displacement is optional arg
    : MasterSlaveDoF(0, d),
      master(0),
      nslaves(0)
  {}
  virtual ~MasterDegreeOfFreedom() {}
  virtual DegreeOfFreedom *controller();
  virtual const DegreeOfFreedom *controller() const;
  virtual DegreeOfFreedom *enslave(MasterDegreeOfFreedom*,
			   DegreeOfFreedom *(Grid::*)(DegreeOfFreedom*));
  virtual DegreeOfFreedom *emancipate(MasterDegreeOfFreedom*);
  virtual void clear_master_fix() { nfix = 0; } // ugly
  virtual CharString tag() const;

#ifdef DEBUG
  virtual void dump(ostream &, CharString) const;
#endif
  friend class SlaveDegreeOfFreedom;
  friend class DegreeOfFreedom;
  friend class Grid;
};

class SlaveDegreeOfFreedom : public MasterSlaveDoF {
private:
  Vec<MasterDegreeOfFreedom*> master;
public:
  SlaveDegreeOfFreedom(MasterDegreeOfFreedom *m, double s, double d, int nf);
  virtual ~SlaveDegreeOfFreedom() {}
  virtual DegreeOfFreedom *controller() { return master[0]->controller(); }
  virtual const DegreeOfFreedom *controller() const {
    return master[0]->controller();
  }
  virtual DegreeOfFreedom *enslave(MasterDegreeOfFreedom*,
			   DegreeOfFreedom *(Grid::*)(DegreeOfFreedom*));
  virtual DegreeOfFreedom *emancipate(MasterDegreeOfFreedom*);
  virtual CharString tag() const;
#ifdef DEBUG
  virtual void dump(ostream &, CharString) const;
#endif
};


ostream &operator<<(ostream &os, const DegreeOfFreedom &dof);

ostream &operator<<(ostream &os, const FreedomFlags &ff);

#endif
