// -*- C++ -*-
// $RCSfile: kessler.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2005-02-17 21:34:53 $

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
#include "parameters.h"
#include "random.h"
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

// rotation matrices are cyclic permutations of {x,y,z}
static double T1data[] = {0, 0, 1, 1, 0, 0, 0, 1, 0};
static double T2data[] = {0, 1, 0, 0, 0, 1, 1, 0, 0};
const MV_ColMat_double KesslerElement::T1(&T1data[0], 3, 3);
const MV_ColMat_double KesslerElement::T2(&T2data[0], 3, 3);

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

  // This is a kludge. The local stiffness matrix is calculated in the
  // TetragonalElement constructor, but that's done without knowledge
  // of the mutation state.  To do this correctly, setstiffness should
  // be called for ALL elements only after the derived class
  // constructor is called, but that would require too great a rewrite
  // to be feasible now. We'll do it right in version 2.0.

  Kset = TF_FALSE;
  setstiffness();
}

void KesslerElement::binarywrite(FILE *file, char formatflag) const {
  initialize();
  if(mutated) formatflag |= MUTATED;
  TetragonalElement::binarywrite(file, formatflag);
  float brr = barrier;
  writebinary(file, brr);	// barrier (float)
  writebinary(file, approx);	// approximation mode (int)
  writebinary(file, approxdir);	// approximation direction (int)
  writebinary(file, onceonly?1:0);
  if(mutated) {
    writebinary(file, oldgray);
    writebinary(file, mutation_state);
  }
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
#ifdef THERMAL
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// kappa11 (float)
  kappa11_dflt = c;		// float to double
  if(!readbinary(file, c)) { ok = TF_FALSE; return 0; }	// kappa33 (float)
  kappa33_dflt = c;		// float to double
#endif // THERMAL

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

Cijkl KesslerElement::setCijkl() const {
  if(!mutated || mutation_state == 0)
    return TetragonalElement::setCijkl();

  // since T1 and T2 are just axis permutations, using
  // Cijkl::transform here is inefficient.  It should probably be
  // fixed.
  if(mutation_state == 1)
    return TetragonalElement::setCijkl().transform(T1);
  return TetragonalElement::setCijkl().transform(T2);
}

SymmMatrix KesslerElement::setAlpha() const {
  if(!mutated || mutation_state == 0)
    return TetragonalElement::setAlpha();
  if(mutation_state == 1)
    return TetragonalElement::setAlpha().transform(T1);
  return TetragonalElement::setAlpha().transform(T2);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

double KesslerElement::find_deltag(const MV_ColMat_double &mutrot) {
  // stress in lab coordinates
  stresstensor();		// sets StrainStress stress (lab coords)
  StrainStress stress2;		// stress after mutation

  // elastic strain in lab coordinates
  elastic_straintensor();	// sets StrainStress elastic_strain 
  StrainStress elastic_strain2;	// elastic strain after mutation

  // elastic moduli in lab coordinates
  Cijkl cijkl2 = setCijkl().transform(mutrot).transform(orientation);
  // (inefficient, but correct)

  SymmMatrix alpha1(setAlpha().transform(orientation));
  SymmMatrix alpha2(setAlpha().transform(mutrot).transform(orientation));
  StrainStress gamma1(alpha1*deltaT());
  StrainStress gamma2(alpha2*deltaT());
  StrainStress deltagamma(gamma2 - gamma1);
  switch(approx) {
  case KA_stress:
    // approximation is that stress(after) = stress(before)
    stress2 = stress;
    elastic_strain2 = cijkl2.solve(stress);
    break;
  case KA_totalstrain:
    elastic_strain2 = total_straintensor() - gamma2;
    stress2 = cijkl2*elastic_strain2;
    break;
  case KA_elasticstrain:
    elastic_strain2 = elastic_strain;
    stress2 = cijkl2*elastic_strain2;
    break;
  }

  double deltag = 0.5*((stress + stress2)*deltagamma)
    - stress2*elastic_strain + stress*elastic_strain2;

  return deltag;
}

int KesslerElement::mutate() {
  initialize();
  if(onceonly && mutated)
    return 0;

  double deltag;
  const MV_ColMat_double *mutrot;

  // choose which way to try to mutate, and find deltag
  switch(approxdir) {
  case KAD_random:
    if(rndm() < 0.5)
      mutrot = &T1;
    else
      mutrot = &T2;		// T2 = T1*T1,  T2*T1 = 1
    deltag = find_deltag(*mutrot);
    break;
  case KAD_best:
    double dg1 = find_deltag(T1);
    double dg2 = find_deltag(T2);
    if(dg1 > dg2) {
      mutrot = &T1;
      deltag = dg1;
    }
    else {
      mutrot = &T2;
      deltag = dg2;
    }
    break;
  }

  if(deltag >= barrier) {	// element mutates
    if(mutrot == &T1)
      mutation_state = (mutation_state + 1)%3;
    else
      mutation_state = (mutation_state + 2)%3;

    if(mutation_state == 0) {	// element has mutated back to original state
      mutated = 0;
      intrinsic_gray = oldgray;
    }
    else {			// element has mutated to an unoriginal state
      if(intrinsic_gray > 0.0) oldgray = intrinsic_gray;
      intrinsic_gray = 0.0; 
      mutated = 1;
    }
    not_current();
    return 1;
  }
  return 0;
}

void KesslerElement::revert() {
  if(mutated) {
    if(mutation_state != 0) {
      mutation_state = 0;
      not_current();
      setstiffness();
    }
    mutated = 0;
    intrinsic_gray = oldgray;
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

template <>
VarObject*
Var<Enum<KesslerApproxMode> >::create_varobject(Variable *var, Form *form,
						VarObjectType t,
						int x, int y, int w, int h)
{
  return new EnumVarObject<KesslerApproxMode>(var, form, t, x, y, w, h);
}

template <>
VarObject*
Var<Enum<KesslerApproxDirection> >::create_varobject(Variable *var, Form *form,
						     VarObjectType t,
						     int x, int y, int w, int h)
{
  return new EnumVarObject<KesslerApproxDirection>(var, form, t, x, y, w, h);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#include "enumio.C"
