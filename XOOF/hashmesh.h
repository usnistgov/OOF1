// -*- C++ -*-
// $RCSfile: hashmesh.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2005-07-30 20:16:08 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef HASHMESH_H
#define HASHMESH_H

#include "shape.h"
#include "grid.h"
#include "parameters.h"

class MeshCoord;

template <class TYPE>
class HashMesh {
protected:
  int nx, ny;
  double stepx, stepy;
  double xmin, ymin;
  MeshCoord (TYPE::*position)(double) const;
  class HMLink {
  public:
    TYPE *which;
    HMLink *next;
    HMLink(TYPE *newt=0, HMLink *list=0)
      : which(newt), next(list) {}
  };
  HMLink ***mesh;
  void deletemesh();
  void whichbin(const MeshCoord&, int&, int&) const;
public:
  HashMesh(MeshCoord (TYPE::*pos)(double) const)
    : position(pos), nx(0), ny(0), mesh(0) {}
  virtual ~HashMesh() { deletemesh(); }
  void setsize(int, int, const Rectangle&);
  void add(TYPE *);
  TYPE *find(const MeshCoord &) const;
};

// #include "hashmesh.C"

#endif
