// -*- C++ -*-
// $RCSfile: elector.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-10-26 21:11:42 $


#ifndef ELECTOR_H
#define ELECTOR_H

// Elector classes are used to vote to find the most popular kind of
// pixel in a mesh triangle.

class AMTriangle;

#include "cell_coordinate.h"
#include "truefalse.h"
#include "vec.h"

#include "beginheader.h"

class Elector {
public:
  Elector() : nvotes(0) {}
  virtual ~Elector() {}
  void vote(const Cell_coordinate&, double weight);
  // are two pixels in the same voting bloc?
  virtual int sameparty(const Cell_coordinate&, const Cell_coordinate&) const
    = 0;
  void tally();
  double winning_fraction() const { return vote_fraction; }
  Cell_coordinate winner() const { return winner_; }
  virtual int badrules() { return 0; }
  double turnout() const { return nvotes; }
private:
  Vec<Cell_coordinate> catalog;	// first occurence of each pixel category
  Vec<double> abundance;	// how often it occurs
  // next two are set by tally();
  double vote_fraction;		// fraction of votes obtained by winner
  Cell_coordinate winner_;
  double nvotes;		// sum of weights
};


// GroupElectors are used to find the pixel to be used for deciding
// which groups the mesh triangle is in.

class GroupElector : public Elector {
public:
  TrueFalse mattype;		// should election consider material type?
  TrueFalse gray;		// how about gray value? (not used, not set)
  TrueFalse mattypeparam;	// how about material parameters?
  TrueFalse groupmemb;		// how about which groups the pixel is in?
  GroupElector()
    : mattype(1),
      mattypeparam(1),
      gray(0),
      groupmemb(1)
    {}
  virtual int sameparty(const Cell_coordinate&, const Cell_coordinate&) const;
  virtual int badrules();
};

// MaterialElectors are used to find the pixel to be used to decide
// which material the triangle should be made of.

class MaterialElector : public Elector {
public:
  TrueFalse gray;		// (not used, since it's not set)
  TrueFalse groupmemb;
  MaterialElector()
    : groupmemb(1),
      gray(0)
    {}
  virtual int sameparty(const Cell_coordinate&, const Cell_coordinate&) const;
};

EXTERN MaterialElector material_elector;
EXTERN GroupElector group_elector;

#include "endheader.h"
#endif
