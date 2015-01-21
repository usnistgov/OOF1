// -*- C++ -*-
// $RCSfile: distort.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:47 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef DISTORT_H
#define DISTORT_H

#include <iostream.h>
#include <stdio.h>
#include "grid.h"
#include "menuDef.h"

Menu *distmenu();
//void record_distort_vars(Grid*);

#ifdef THERMAL
class ThermalDistortion {
public:
  double dTdx;
  double dTdy;
  double T0;
  double heatrate;
  ThermalDistortion() : dTdx(0), dTdy(0), T0(0), heatrate(0) {}
  operator bool() const;
  const ThermalDistortion &operator+=(const ThermalDistortion&);
  friend ostream &operator<<(ostream &, const ThermalDistortion&);
  friend istream &operator>>(istream &, ThermalDistortion&);
  void print(FILE*) const;
};
#endif // THERMAL

#endif
