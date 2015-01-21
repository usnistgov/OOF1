// -*- C++ -*-
// $RCSfile: fiddlemesh.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:31 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef FIDDLEMESH_H
#define FIDDLEMESH_H

#include "menuDef.h"
#include "beginheader.h"

class FiddleCmd : public CommandM {
private:
  double T;			// temperature
  double delta;			// range of motion of each node
  int iterations;
public:
  FiddleCmd();
  virtual CommandFn func();
  virtual CommandM *clone() const;
};

class FiddleDownHillCmd : public CommandM {
private:
  double delta;			// maximum attempted move
  int iterations;
public:
  FiddleDownHillCmd();
  virtual CommandFn func();
  virtual CommandM *clone() const;
};

class FiddleLaplaceCmd : public CommandM {
private:
  int iterations;
public:
  FiddleLaplaceCmd();
  virtual CommandFn func();
  virtual CommandM *clone() const;
};

CommandFn swapedges();
CommandFn swapworst();

EXTERN double Ealpha DFLT(0.5);

#include "endheader.h"

#endif
