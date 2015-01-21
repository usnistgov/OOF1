// -*- C++ -*-
// $RCSfile: damisoelement.C,v $
// $Revision: 1.1 $
// $Author: langer $
// $Date: 2000-12-22 21:24:31 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "charstring.h"
#include "createelement.h"
#include "damisoelement.h"
#include "element.h"
#include "grid.h"
#include "inputflags.h"
#include "node.h"
#include "readbinary.h"
#include <iostream.h>

// static default data
CharString DamIsoElement::nameplate("damisotropic");
double DamIsoElement::young_dflt(1.0);
double DamIsoElement::poisson_dflt(0.33);
double DamIsoElement::t_coeff_dflt(0.0);
TrueFalse DamIsoElement::planestrain_dflt(0);
double DamIsoElement::maxstress_dflt(1.0);
double DamIsoElement::minstress_dflt(-1.0);
double DamIsoElement::maxknockdown_dflt(0.5);
double DamIsoElement::minknockdown_dflt(0.5);
float DamIsoElement::oldgray_dflt(0.0);
double DamIsoElement::total_knockdown_dflt(1.0);

int DamIsoElement::Nnodes(3);

ElementTypeRegistration DamIsoElement::reg(nameplate,
					   DamIsoElement::binaryread);


DamIsoElement::DamIsoElement(Grid *g)
  : TriElement(g),
    young(young_dflt),
    poisson(poisson_dflt), t_coeff(t_coeff_dflt), 
    maxstress(maxstress_dflt), minstress(minstress_dflt),
    maxknockdown(maxknockdown_dflt), minknockdown(minknockdown_dflt),
    total_knockdown(total_knockdown_dflt),
    oldgray(oldgray_dflt)
{
  planestrain = planestrain_dflt;
  // get ready to create next element.

  // total_knockdown_dflt is an optional argument when reading ascii
  // goof files, so it has to be reset to the default value. This
  // plays the role of Element::reset_defaults(), but DamIsoElement
  // doesn't fit in with the general scheme.
  total_knockdown_dflt = 1.0;
} 

Element *DamIsoElement::binaryread(FILE *file, TrueFalse &ok) {
    TriElement::binaryread(file, ok);
    if(!ok) return 0;

    float psn, yng, tc;
    if(!readbinary(file, psn)) { ok = TF_FALSE; return 0; } // poisson (float)
    poisson_dflt = psn;	// float to double
    if(!readbinary(file, yng)) { ok = TF_FALSE; return 0; } // young (float)
    young_dflt = yng;	// float to double
    if(!readbinary(file, tc)) { ok = TF_FALSE; return 0; }  // alpha (float)
    t_coeff_dflt = tc;
#ifdef THERMAL
    float kpa;
    if(!readbinary(file, kpa)) { ok = TF_FALSE; return 0; } // kappa (float)
    kappa_dflt = kpa;
#endif // THERMAL

    float mxs, mns, mxk, mnk;
    if(!readbinary(file, mxs)) { ok = TF_FALSE; return 0; } // maxstress(float)
    maxstress_dflt = mxs;
    if(!readbinary(file, mns)) { ok = TF_FALSE; return 0; } // minstress(float)
    minstress_dflt = mns;
    if(!readbinary(file, mxk)) { ok = TF_FALSE; return 0; } // maxknock (float)
    maxknockdown_dflt = mxk;
    if(!readbinary(file, mnk)) { ok = TF_FALSE; return 0; } // minknock (float)
    minknockdown_dflt = mnk;
    
    if(!readbinary(file, inputformatflag)) { ok = TF_FALSE; return 0; } // flag (char)
    planestrain_dflt = inputformatflag & PLANESTRAIN;
    
    if(inputformatflag & STIFFNESS) {
	if(!K_dflt.binaryread(file)) {		// stiffness matrix
	    ok = TF_FALSE;
	    return 0;
	}
#ifdef THERMAL
	if(!K_t_dflt.binaryread(file)) {
	  ok = TF_FALSE;
	  return 0;
	}
#endif // THERMAL
	for(int i=0; i<6; i++)
	    if(!readbinary(file, thermal_dflt(i))) {	// thermal forces
		ok = TF_FALSE;
		return 0;
	    }
    }
    else if(inputformatflag & DUPLICATE) {
      if(!readbinary(file, cloneindex)) { ok = TF_FALSE; return 0; }
    }
    if(inputformatflag & MUTATED) {
      float tkd;
      if(!readbinary(file, tkd)) { ok = TF_FALSE; return 0; } // total_knockdown
      total_knockdown_dflt = tkd;
      if(!readbinary(file, oldgray_dflt)) { ok = TF_FALSE; return 0; }// oldgray
    }
    return create_element((DamIsoElement*)0);
}



// ---------------------------------------------------------- //

CharString DamIsoElement::parameters() const {
  CharString p = TriElement::parameters()
    + " poisson=" + to_charstring(poisson)
    + " young=" + to_charstring(young)
    + " alpha=" + to_charstring(t_coeff)
    + " planestrain=" + to_charstring(planestrain)
    + " max_stress=" +  to_charstring(maxstress)
    + " min_stress=" +to_charstring( minstress)
    + " max_knockdown=" + to_charstring(maxknockdown)
    + " min_knockdown=" + to_charstring(minknockdown);
//    if(total_knockdown != 1.0)
//      p += " oldgray=" + to_charstring(oldgray)
//        + " total_knockdown=" + to_charstring(total_knockdown);
  return p;
}

void DamIsoElement::AddArgs(CommandM *cmd) {
  TriElement::AddArgs(cmd);
  AddArgument(cmd, "planestrain", planestrain_dflt);
  AddArgument(cmd, "young", young_dflt);
  AddArgument(cmd, "poisson", poisson_dflt);
  AddArgument(cmd, "alpha", t_coeff_dflt);
  AddArgument(cmd, "max_stress", maxstress_dflt);
  AddArgument(cmd, "min_stress", minstress_dflt);
  AddArgument(cmd, "max_knockdown", maxknockdown_dflt);
  AddArgument(cmd, "min_knockdown", minknockdown_dflt);
  AddArgument(cmd, "oldgray", oldgray_dflt, MOPT_SECRET);
  AddArgument(cmd, "total_knockdown", total_knockdown_dflt, MOPT_SECRET);
}

bool DamIsoElement::same_type(const Element *el) const {
  const DamIsoElement *other = dynamic_cast<const DamIsoElement*>(el);
  return el && TriElement::same_type(el)
    && planestrain == other->planestrain
    && young == other->young
    && poisson == other->poisson
    && t_coeff == other->t_coeff
    && maxstress == other->maxstress
    && minstress == other->minstress
    && maxknockdown == other->maxknockdown
    && minknockdown == other->minknockdown;
}
