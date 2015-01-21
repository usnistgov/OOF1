// -*- C++ -*-
// $RCSfile: gaselement.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2005-02-15 22:19:50 $

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
#include "element.h"
#include "gaselement.h"
#include "grid.h"
#include "inputflags.h"
#include "mvmult.h"
#include "mvvd.h"
#include "node.h"
#include "parameters.h"
#include "readbinary.h"
#include "strainstress.h"
#include <iostream.h>

#ifdef THERMAL

// static default data
CharString GasElement::nameplate("gas");
double GasElement::kappa_dflt(0.0);
StrainStress GasElement::nostress;
int GasElement::Nnodes(3);


ElementTypeRegistration GasElement::reg(nameplate, GasElement::binaryread);

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

GasElement::GasElement(Grid *g)
  : TriElement(g),
    kappa(kappa_dflt)
{
  K.resize(0);			// save some space
  if(inputformatflag & STIFFNESS) {
    K_t = K_t_dflt;
  }
  else if(inputformatflag & DUPLICATE) {
    K_t = ((GasElement*)g->interiorE[cloneindex])->K_t;
  }
  else {
    setstiffnessT();
  }
}

Element *GasElement::binaryread(FILE *file, TrueFalse &ok) {
  TriElement::binaryread(file, ok);
  if(!ok) return 0;
  float kpa;
  if(!readbinary(file, kpa)) { ok = TF_FALSE; return 0; } // kappa (float)
  kappa_dflt = kpa;
  if(!readbinary(file, inputformatflag)) { ok = TF_FALSE; return 0; }
  // flag (char)
  
  if(inputformatflag & STIFFNESS) {
    if(!K_t_dflt.binaryread(file)) {
      ok = TF_FALSE;
      return 0;
    }
  }
  else if(inputformatflag & DUPLICATE) {
    if(!readbinary(file, cloneindex)) { ok = TF_FALSE; return 0; }
  }
  return create_element((GasElement*)0);
}

void GasElement::binarywrite(FILE *file, char formatflag) const {
  TriElement::binarywrite(file, formatflag);
  float kpa = kappa;
  writebinary(file, kpa);	// kappa (float)
  writebinary(file, formatflag); // flag (char)
  if(formatflag & STIFFNESS) {
    K_t.binarywrite(file);
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void GasElement::setstiffness() {}

void GasElement::setstiffnessT() {
  const SideList side(this);
  const double area = side.area;
  if(area == 0.0)
    garcon()->msout << ms_error << "Element " << index
		    << " has zero area!" << endl << ms_normal;
  if(area < 0.0)
    garcon()->msout << ms_error << "Element " << index
		    << " has negative area! " << area << endl << ms_normal;
  const Gvector G(side);	// 3 3-vectors whose third component is zero
  for(int nu=0; nu<3; nu++)
    for(int mu=0; mu<=nu; mu++)
      K_t(nu, mu) = -area * kappa * (G[nu](0)*G[mu](0) + G[nu](1)*G[mu](1));
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

MeshCoord GasElement::force(const Node*) const {
  return MeshCoord(0.0, 0.0);
}

const MV_Vector_double &GasElement::gradient_temperature_field() {
  // this shouldn't have to override
  // TriElement::gradient_temperature_field(), but if it doesn't, then
  // that function would have to set current_grad_thermal, and then it
  // probably ought to cache its value. That would screw up the
  // caching of the GTElement thermal gradient.
  if(current_grad_thermal) return Grad_T_field;
  current_grad_thermal = TF_TRUE;
  return TriElement::gradient_temperature_field();
}

const MV_Vector_double &GasElement::heat_flux_field() {
  if(current_J) return J_field;
  current_J = TF_TRUE;
  J_field = -kappa*gradient_temperature_field();
  return J_field;
}

double GasElement::average_T() const {
  double tsum = 0;
  for(int i=0; i<3; i++)
    tsum += corner[i]->phi->disp();
  return tsum/3.0;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

CharString GasElement::parameters() const {
  return TriElement::parameters()
    + " kappa=" + to_charstring(kappa);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

bool GasElement::same_type(const Element *el) const {
  const GasElement *other = dynamic_cast<const GasElement*>(el);
  return other && TriElement::same_type(el)
    && kappa == other->kappa;
}

std::vector<CharString> *GasElement::print_properties(ostream &os) const {
  std::vector<CharString> *names = TriElement::print_properties(os);
  CharString pname = newPropertyName();
  os << "OOF.LoadData.Property.Thermal.Conductivity.Isotropic(name='" << pname
     << "', kappa=" << kappa << ")" << endl;
  names->push_back("Thermal:Conductivity:Isotropic:" + pname);
  return names;
}

#endif // THERMAL
