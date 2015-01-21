// -*- C++ -*-
// $RCSfile: kessler.C,v $
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

#include "createelement.h"
#include "enum.h"
#include "inputflags.h"
#include "kessler.h"
#include "readbinary.h"

double KesslerElement::barrier_dflt(0.0);
Enum<KesslerApproxMode> KesslerElement::approx_dflt(KA_stress);
Enum<KesslerApproxDirection> KesslerElement::approxdir_dflt(KAD_random);
float KesslerElement::oldgray_dflt(0.0);
TrueFalse KesslerElement::onceonly_dflt(0);
TrueFalse KesslerElement::mutated_dflt(0);
int KesslerElement::mutation_state_dflt(0);

CharString KesslerElement::nameplate("kessler");
ElementTypeRegistration KesslerElement::reg(KesslerElement::nameplate,
					    KesslerElement::binaryread);

void KesslerElement::initialize() {
  // static function called to initialize Enum for approximation mode
  static int once = 0;
  if(once) return;
  once = 1;
  Enum<KesslerApproxMode>::name(KA_stress, "stress");
  Enum<KesslerApproxMode>::name(KA_totalstrain, "total-strain");
  Enum<KesslerApproxMode>::name(KA_elasticstrain, "elastic-strain");
  Enum<KesslerApproxDirection>::name(KAD_random, "random");
  Enum<KesslerApproxDirection>::name(KAD_best, "best");
}

void KesslerElement::AddArgs(CommandM *cmd) {
  initialize();
  TetragonalElement::AddArgs(cmd);
  AddArgument(cmd, "barrier", barrier_dflt);
  AddArgument(cmd, "approximation", approx_dflt);
  AddArgument(cmd, "direction", approxdir_dflt);
  AddArgument(cmd, "only_once", onceonly_dflt);
  AddArgument(cmd, "oldgray", oldgray_dflt, MOPT_SECRET);
  AddArgument(cmd, "mutated", mutated_dflt, MOPT_SECRET);
  AddArgument(cmd, "state", mutation_state_dflt, MOPT_SECRET);
}

KesslerElement::KesslerElement(Grid *g)
  : TetragonalElement(g),
    barrier(barrier_dflt),
    approx(approx_dflt),
    approxdir(approxdir_dflt),
    onceonly(onceonly_dflt),
    oldgray(oldgray_dflt),
    mutated(mutated_dflt),
    mutation_state(mutation_state_dflt)
{
  initialize();
}

Element *KesslerElement::binaryread(FILE *file, TrueFalse &ok) {
  // kludge: We can't just use TetragonalElement::binaryread() because
  // that calls create_element(). The concrete element classes weren't
  // meant to be used as base classes!
  initialize();

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables
  // that will be used by the constructor.

   // This section is copied from TetragonalElement::binaryread().
 
  float c;
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c11 (float)
  c11_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c12 (float)
  c12_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c13 (float)
  c13_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c16 (float)
  c16_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c33 (float)
  c33_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c44 (float)
  c44_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// c66 (float)
  c66_dflt = c;			// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha11 (float)
  alpha11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// alpha33 (float)
  alpha33_dflt = c;		// float to double

  float brr;
  if(!readbinary(file, brr)) { ok = TF_FALSE; return 0; } // barrier (float)
  barrier_dflt = brr;
  if(!readbinary(file, approx_dflt)) { ok = TF_FALSE; return 0; } //approx (int)
  if(!readbinary(file, approxdir_dflt)) { // approxdir (int)
    ok = TF_FALSE; return 0;
  }
  int f;
  if(!readbinary(file, f)) { ok = TF_FALSE; return 0; }	// onceonly (int)
  onceonly_dflt = f;
  mutated_dflt = Element::inputformatflag & MUTATED;
  if(mutated_dflt) {
    if(!readbinary(file, oldgray_dflt)) { // oldgray (float)
      ok = TF_FALSE; return 0;
    }
    if(!readbinary(file, mutation_state_dflt)) { // mutation_state (int)
      ok = TF_FALSE; return 0;
    }
  }
  
  return create_element((KesslerElement*) 0);
}

CharString KesslerElement::parameters() const {
  initialize();
  CharString p = TetragonalElement::parameters()
    + " barrier=" + to_charstring(barrier)
    + " approximation=" + to_charstring(approx)
    + " direction=" + to_charstring(approxdir)
    + " only_once=" + to_charstring(onceonly);
  if(mutated)
    p +=
      " mutated=" + to_charstring(mutated) +
      " state=" + to_charstring(mutation_state) +
      " oldgray=" + to_charstring(oldgray);
  return p;
}

bool KesslerElement::same_type(const Element *el) const {
  const KesslerElement *other = dynamic_cast<const KesslerElement*>(el);
  return other && TetragonalElement::same_type(el)
    && barrier == other->barrier
    && approx == other->approx
    && approxdir == other->approxdir
    && onceonly == other->onceonly
    && mutated == other->mutated
    && mutation_state == other->mutation_state
    && oldgray == other->oldgray;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#include "enumio.C"
