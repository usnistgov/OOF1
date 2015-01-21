// -*- C++ -*-
// $RCSfile: pixelgroups.h,v $
// $Revision: 1.11 $
// $Author: langer $
// $Date: 2000-11-01 16:41:51 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef PIXELGROUPS_H
#define PIXELGROUPS_H

#include "groups.h"

class Cell_coordinate;
class MeshCoord;

class PixelGroup : public Group<Cell_coordinate> {
private:
  static unsigned int ngroups;

  // group_id is a unique number assigned to each group. Every time a
  // selection is made, a new unnamed group is formed, so the group_id
  // can go up surprisingly rapidly.  The group_id is used to
  // distinguish classes of pixels when optimizing the adaptive mesh.
  int group_id;

public:
  PixelGroup() :
    group_id(ngroups++)
    {
    }
  PixelGroup(const CharString &n)
    : Group<Cell_coordinate>(n),
      group_id(ngroups++)
    {
    }
  virtual ~PixelGroup();

  static int compare(PixelGroup* const&, PixelGroup* const&);

  virtual void append(const Vec<Cell_coordinate> &);
  virtual void append(const Cell_coordinate&);
  virtual void append(const Group<Cell_coordinate> &grp);
  virtual void clear();
  virtual void remove(const Cell_coordinate&);
  virtual void remove(const Vec<Cell_coordinate>&);
  virtual void remove(const Group<Cell_coordinate>&);
  virtual void trim(const Cell_coordinate&);
  virtual void replace(const Cell_coordinate&, const Cell_coordinate&);

  void calcEigenValues(double&, double&) const;  //the two Eigenvalues are parameters
  double perimeter() const;	// no. bdy pixels/(pi*sqrt(no. pixels))
  double aspectRatio() const;
};


Menu *groupmenu();
Menu *groupselectmenu();
Menu *groupselectmenu_too();
Menu *groupinterselectmenu();
Menu *unselectgroupmenu();

#endif







