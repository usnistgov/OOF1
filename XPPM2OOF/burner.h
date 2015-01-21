// -*- C++ -*-
// $RCSfile: burner.h,v $
// $Revision: 1.6 $
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

#ifndef BURNER_H
#define BURNER_H

#include "cell_coordinate.h"
#include "color.h"
#include "array.h"
#include "truefalse.h"
#include "vec.h"

class Image;

class Burner {
private:
  Array<bool> burned;
  const Image *forest;
  const Array<bool> *fireproof;

  // should the fire spread?
  virtual int spread(const Color &from, const Color &to) const = 0;

  void burn(const Cell_coordinate&); // actually spread the fire
  void burn_nbrs(Vec<Cell_coordinate>&, const Cell_coordinate&);
  int nburned_;

  // list of directions to neighbors. There is one static instance of
  // this class.
  class Nbr {
  private:
    Cell_coordinate nbr[8];
    Nbr();			// loads the directions into the array.
    const Cell_coordinate &operator[](int x) const { return nbr[x]; }
    friend class Burner;
  };
  static Nbr neighbor;

protected:
  Cell_coordinate startingpoint;
  Color startingcolor;

public:
  Burner() : next_nearest(0) {}
  virtual ~Burner() {}
  void ignite(const Image&, const Cell_coordinate&, int shiftkey);
  void igniteNoUndo(const Image&, const Cell_coordinate&, int shiftkey);
  int nburned() const { return nburned_; }
  void store_burned(Vec<Cell_coordinate>&) const;
  TrueFalse next_nearest;
};

#endif // BURNER_H
