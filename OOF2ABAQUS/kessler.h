// -*- C++ -*-
// $RCSfile: kessler.h,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-26 14:36:48 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef KESSLER_H
#define KESSLER_H

// Hannes Kessler's model for switching in ferroelastics.

// This is a mutable element, but it's not derived from
// MutableElement, because it doesn't need that much overhead. It's
// inconvenient to store the full mutated Cijkl -- it's only necessary
// to store the rotation state.

#include "mutableelement.h"
#include "tetragonal.h"

enum KesslerApproxMode {
  KA_stress,			// assume stress unchanged during mutation
  KA_totalstrain,		// assume total strain unchanged
  KA_elasticstrain		// assume elastic strain unchanged
};

enum KesslerApproxDirection {
  KAD_random,
  KAD_best
};

class KesslerElement : public TetragonalElement {
private:
  static void initialize();
protected:
  double barrier;		// switching barrier
  static double barrier_dflt;
  Enum<KesslerApproxMode> approx;
  Enum<KesslerApproxDirection> approxdir;
  static Enum<KesslerApproxMode> approx_dflt;
  static Enum<KesslerApproxDirection> approxdir_dflt;
  TrueFalse mutated;
  static TrueFalse mutated_dflt;
  float oldgray;
  static float oldgray_dflt;
  TrueFalse onceonly;
  static TrueFalse onceonly_dflt;
  int mutation_state;
  static int mutation_state_dflt;

  static Element *binaryread(FILE*, TrueFalse&);
  static ElementTypeRegistration reg;
  static CharString nameplate;

public:
  KesslerElement() {}
  KesslerElement(Grid*);
  virtual ~KesslerElement() {}
  virtual const CharString &tag() const { return nameplate; }

  static void AddArgs(CommandM *cmd);

  virtual CharString parameters() const;
  virtual bool same_type(const Element*) const;
};

#endif
