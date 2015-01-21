// -*- C++ -*-
// $RCSfile: statistics.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:11:15 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef STATISTICS_H
#define STATISTICS_H

class StrainStress;

class Statval{
private:
  int size;
  double sum;
  double sum_squares;
  double wsum;
  double max;
  double min;
public:
  Statval():size(0),sum(0),sum_squares(0),max(0),min(0),wsum(0) {}

  double average() const;

  double deviation() const;

  double maximum() const {
    return max;
  }
  
  double minimum() const {
    return min;
  }
  
  void include(double val, double weight=1.0);
  
  int samplesize() const {return size;}

};

class StressStrainStat{
  
public:
  Statval xx;
  Statval xy;
  Statval xz;
  Statval yy;
  Statval yz;
  Statval zz;
  
  Statval maxeigen;
  Statval mideigen;
  Statval mineigen;
  
  Statval inv1;
  Statval inv2;
  Statval inv3;

  Statval shear;

  void include(const StrainStress&, double weight);
  int samplesize(){return xx.samplesize();}
  
};

#endif
