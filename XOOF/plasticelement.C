// -*- C++ -*-
// $RCSfile: plasticelement.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-10-31 19:15:13 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// Plastic element, based on GTElement and MutableElement

#include "plasticelement.h"
#include "inputflags.h"
#include "readbinary.h"
#include "createelement.h"
#include "symeig3.h"
#include <math.h>

// static default data
CharString PlasticElement::nameplate("plastic");
double PlasticElement::young_dflt(1.0);
double PlasticElement::poisson_dflt(0.0);
double PlasticElement::t_coeff_dflt(0.0);
double PlasticElement::hardening_coefficient_dflt(0.33);
double PlasticElement::stress_limit_dflt(0.1);
int PlasticElement::Nnodes(3);

ElementTypeRegistration PlasticElement::reg(nameplate,
					   PlasticElement::binaryread);

// Constructor

PlasticElement::PlasticElement(Grid *g)
  : GTElement(g),
    young(young_dflt),
    poisson(poisson_dflt),
    t_coeff(t_coeff_dflt),
    hardening_coefficient(hardening_coefficient_dflt),
    stress_limit(stress_limit_dflt)
{
  onceonly = False;
  orientation = 0;
  last_strain = 0;
  current_strain = 0;
  original_young = young;
  if(mutated)
    // cijkl has been created by MutableElement constructor
    oldgray = oldgray_dflt;
  else
    cijkl = virgin_cijkl();

  if(!Kset) {
    setstiffness();
    setstiffnessT();
    Kset = TF_TRUE;
  }
}

Cijkl PlasticElement::virgin_cijkl() const
{
  Cijkl c;
  double tmp = young/((1.+poisson)*(1.-2*poisson));
  c(0, 0) = c(1, 1) = c(2, 2) = tmp*(1.-poisson);
  c(0, 1) = c(0, 2) = c(1, 2) = tmp*poisson;
  c(3, 3) = c(4, 4) = c(5, 5) = 0.5*young/(1.+poisson);
  return c;
}

void PlasticElement::binarywrite(FILE *file, char formatflag) const {
  if(mutated) formatflag |= MUTATED;
  GTElement::binarywrite(file, formatflag);
  float psn = poisson;
  float yng = young;
  float tc = t_coeff;
  float hn = hardening_coefficient;
  float stlmt = strainlimit;
 
  writebinary(file, psn);         // poisson (float)
  writebinary(file, yng);         // young (float)
  writebinary(file, tc);          // alpha (float)
  writebinary(file, mxs);         // maxstress (float)
  writebinary(file, hn);	  // hardening limit (float)
  writebinary(file, stlmt);	  // strain limit (float)
  MutableElement::binarywrite(file);
}


// static member function that reads the goof file BEFORE creating an
// element.

Element *PlasticElement::binaryread(FILE *file, TrueFalse &ok) {

  // read base class information
  GTElement::binaryread(file, ok);
  if(!ok) return 0;

  // read data specific to this class and set the static variables that will
  // be used by the constructor.

  float psn, yng, tc, kd1, kd2, mxs;
  if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
  poisson_dflt = psn;  // float to double 
  if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
  young_dflt = yng;
  if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; } // alpha (float)
  t_coeff_dflt = tc;
  if(!readbinary(file, mxs)) { ok = TF_FALSE; return 0; } // maxstress (float)
  maxstress_dflt = mxs;
  if(!readbinary(file, hn)) { ok = TF_FALSE; return 0; } // hard.coeff. (float)
  hardening_coefficient_dflt = hn;
  if(!readbinary(file, stlmt)) { ok = TF_FALSE; return 0; } // strainlimit (float)
  strainlimit_dflt = stlmt;

  if(!MutableElement::binaryread(file))
    return 0;
  
  // create the element
  return create_element((PlasticElement*) 0);
}

CharString PlasticElement::parameters() const {
  return GTElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
    + " hardening coefficent=" + to_charstring(hardening_coefficient)
    + " strain limit=" + to_charstring(strainlimit)
    + MutableElement::parameters();
}

Cijkl PlasticElement::setCijkl() const {
  return cijkl;
}

SymmMatrix PlasticElement::setAlpha() const {
  SymmMatrix a(3);
  a(0, 0) = a(1, 1) = a(2, 2) = t_coeff;
  return a;
}

int PlasticElement::adjust_young(const float target_value) {
  if ( fabs(young/target_young - 1.0) <= 1.0e-3) return 0;
  
  young = 0.5*(young + target_value); //simple binary search
  not_current();
  garcon()->msout << ms_info << "mutating " << index
		  << " at stress " << current_stress
		 << endl << ms_normal;
  if (intrinsic_gray > 0.0) oldgray = intrinsic_gray;
  intrinsic_gray = 0.0;
  mutated = 1;
  return 1;
}

int PlasticElement::mutate() {
 

  stresstensor();
  EigenVector eig1, eig2, eig3;
  eigensystem(stress, eig1, eig2, eig3);
  current_stress = eig1 + eig2 + eig3;
  /* are we unloading on this solution attempt? */
  TrueFalse plastic_regime = current_stress >= stress_limit;

  last_stress = current_stress;
  float target_young;
  
  if (plastic_regime)
    {
      //if unloading in plastic, then youngs mod should be like orignal
      if(current_stress <= last_stress) return(adjust_young(original_young));
     
      target_young = original_young*
	pow(current_stress/stress_limit,hardening_coefficient);
      return(adjust_young(target_young));
    }
  else
    {
      return(adjust_young(original_young));
    }
  

}

void PlasticElement::revert() {
  if(mutated) {
    orientation = 0;
    intrinsic_gray = oldgray;
    cijkl = virgin_cijkl();
    mutated = 0;
    setstiffness();
    setstiffnessT();
    not_current();
  }
}
