// -*- C++ -*-
// $RCSfile: despeckle.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:28 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef DESPECKLE_H
#define DESPECKLE_H

#include "menuDef.h"
#include "truefalse.h"

class DespeckleCmd : public CommandM {
private:
  int nnbrs;
public:
  DespeckleCmd() :
    CommandM("despeckle", "select pixels with n selected neighbors"), 
    nnbrs(8)
  {
    AddArgument(this, "neighbors", nnbrs);
  }
  virtual CommandFn func();
  virtual CommandM *clone() const {
    DespeckleCmd *dc = new DespeckleCmd;
    dc->nnbrs = nnbrs;
    return dc;
  }
};

class elkcepseDCmd : public CommandM {
private:
  int nnbrs;
public:
  elkcepseDCmd()
    : CommandM("elkcepsed",
	       "unselect pixels with fewer than n selected neighbors"), 
      nnbrs(3)
  {
    AddArgument(this, "neighbors", nnbrs);
  }
  virtual CommandFn func();
  virtual CommandM *clone() const {
    elkcepseDCmd *dc = new elkcepseDCmd;
    dc->nnbrs = nnbrs;
    return dc;
  }
};

class LifeCmd : public CommandM {
private:
  int maxiter;
  TrueFalse pbc;
public:
  LifeCmd()
    : CommandM("life", "play the game of life with the selected pixels"),
      maxiter(10000)
    {
      AddArgument(this, "generations", maxiter);
      AddArgument(this, "periodic_bc", pbc);
    }
  virtual CommandFn func();
  virtual CommandM *clone() const {
    LifeCmd *lc = new LifeCmd;
    lc->maxiter = maxiter;
    lc->pbc = pbc;
    return lc;
  }
};

#endif

