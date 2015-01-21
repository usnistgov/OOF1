// -*- C++ -*-
// $RCSfile: burn.h,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-09-06 20:11:24 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



#ifndef BURN_H
#define BURN_H

#include "menuDef.h"
#include "burner.h"

class Burn : public Burner {
private:
  int lcl, glbl;		// flammabilities
  TrueFalse use_L2norm;
  // default values of parameters 
  static int lcl_dflt, glbl_dflt;
  static TrueFalse L2_dflt, nextnear_dflt;

  virtual int spread(const Color&, const Color&) const;
public:
  Burn();
  friend class BurnCmd;
  friend class ArsonCmd;
  friend class Goof;
};

class BurnCmd : public CommandM {
public:
  BurnCmd(Burn*);
  virtual CommandFn func();
  virtual CommandM *clone() const;
  Burn *burner;
};

class RGBBurn : public Burner {
private:
  int redflm, grnflm, bluflm;
  // default values of parameters
  static int redflm_dflt, grnflm_dflt, bluflm_dflt;
  static TrueFalse nextnear_dflt;

  virtual int spread(const Color&, const Color&) const;
public:
  RGBBurn();
  friend class RGBBurnCmd;
};

class RGBBurnCmd : public CommandM {
public:
  RGBBurnCmd(RGBBurn*);
  virtual CommandFn func();
  virtual CommandM *clone() const;
  RGBBurn *burner;
};

void set_firewall();
void clear_firewall();

#endif










