// -*- C++ -*-
// $RCSfile: mutableelement.h,v $
// $Revision: 1.2 $
// $Author: langer $
// $Date: 2000-12-22 21:24:32 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Class from which elements can be derived if they're mutable.
// They should probably also be derived from GTElement.

#ifndef MUTABLEELEMENT_H
#define MUTABLEELEMENT_H

#include "truefalse.h"
#include "charstring.h"
#include "cijkl.h"
#include "eulerangle.h"
#include "enum.h"
class CommandM;

class MutableElement {
protected:
  TrueFalse mutated;
  TrueFalse onceonly;

  // storage used when reading elements that aren't created yet
  static Cijkl cijkl_dflt;
  static TrueFalse mutated_dflt;
  static float oldgray_dflt;
  static TrueFalse onceonly_dflt;
  static EulerAngle oldorientation_dflt;
  static int binaryread(FILE *file);

  static void AddArgs(CommandM*);
  CharString parameters() const;

public:
  MutableElement()
    : mutated(mutated_dflt),
      onceonly(onceonly_dflt)
  {}
  virtual ~MutableElement() {}

  bool same_type(const Element*) const;
  static void reset_defaults();
};

#endif
