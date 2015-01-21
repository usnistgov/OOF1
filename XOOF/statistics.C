// -*- C++ -*-
// $RCSfile: statistics.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-31 19:15:14 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



#include "eigenvalues.h"
#include "statistics.h"
#include "strainstress.h"
#include "symeig3.h"
#include <math.h>

void Statval::include(double val, double weight)
{
  if(size==0)
    {
      max = val;
      min = val;
    }
  else
    {
      if (val > max) max = val;
      if (val < min) min = val;
    }
  
  size++;
  sum += val*weight;
  wsum += weight;
  sum_squares += val*val*weight;
}

double Statval::average() const {
  return sum/wsum;
}

double Statval::deviation() const
{
  double a = average();
  double variance = sum_squares/wsum - a*a;
  if(variance <= 0.0)		// possible roundoff error
    return 0.0;
  return(sqrt(variance));
}

void StressStrainStat::include(const StrainStress &s, double weight)
{
  xx.include(s(0,0), weight);
  xy.include(s(0,1), weight);
  xz.include(s(0,2), weight);
  yy.include(s(1,1), weight);
  yz.include(s(1,2), weight);
  zz.include(s(2,2), weight);
  
  EigenValues eig;
  eigenvalues(s, eig);
  maxeigen.include(eig.max(), weight);
  mideigen.include(eig.mid(), weight);
  mineigen.include(eig.min(), weight);
  inv1.include(s.invariant1(), weight);
  inv2.include(s.invariant2(), weight);
  inv3.include(s.invariant3(), weight);
  shear.include(0.5*(eig.max()-eig.min()), weight);
}
