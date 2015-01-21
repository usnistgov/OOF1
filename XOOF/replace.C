// -*- C++ -*-
// $RCSfile: replace.C,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2003-06-27 18:46:02 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "elementgroups.h"
#include "grid.h"
#include "griditerator.h"
#include "inputflags.h"
#include "menuDef.h"
#include "parameters.h"
#include "select.h"

/* ELEMENTS */
#include "cubicelement.h"
#include "cubic_crack_element.h"
#include "damageelement.h"
#include "damisoelement.h"
#include "emptyelement.h"
#include "gaselement.h"
#include "grady_element.h"
#include "griffith_element.h"
#include "hexelement.h"
#include "isoelement.h"
#include "kessler.h"
#include "orthoexpand.h"
#include "orthorhombic.h"
#include "tetragonal.h"
#include "trigonal.h"
#include "twissel.h"
#include "valeria.h"
#include "zimmer_element.h"

// replace elements with other elements

// these two templates should be grid methods, but it's not allowed 

/* replaceelement replaces an element with a new'ed element of type
 * ELEMENT. The old element is deleted. A pointer to the new element
 * is stored at the same place as the former pointer to the old element, 
 * so the grid lists don't have to be modified.
 */
template <class ELEMENT>
ELEMENT* replaceelement(Element **oldelement, const ELEMENT*) {
#ifdef DEBUG
  breakpoint();
#endif
  Element *obsolete = *oldelement;
  Element::inputformatflag &= ~DUPLICATE;
  Vec<ElementGroup*> groups = obsolete->groups;
  int i;
    
  for(i=0; i<obsolete->corner.capacity(); i++)
    Element::nodenumber_dflt[i] = obsolete->corner[i]->index;
  Element::index_dflt = obsolete->index;
    
  delete obsolete;
  ELEMENT *newelem = new ELEMENT(grid);
  *oldelement = newelem;

  // put the new element in the old element's groups
  // newelem->groups = groups;
  for(i=0; i<groups.capacity(); i++)
    groups[i]->Add(newelem);

  grid->require_rehash();
  return newelem;
}

// ---------------------------------------------------------------- //

template <class ELEMENT>
class ReplaceSelectedCmd : public CommandM, public ELEMENT {
public:
  ReplaceSelectedCmd() 
    : CommandM(ELEMENT::tag(),
	       "replace selected elements with " + ELEMENT::tag()) {
      ELEMENT::AddArgs(this);
    }
  virtual void func() {
    if(gridinit()) {
      // This doesn't work, since replaceelement can't know what
      // list to put the new element in...
      // 	      ElementIterator iter(EI_SELECTED, grid);
      // 	      Element *el;
      // 	      while(el = iter())
      // 		replaceelement(&el, (ELEMENT*)0)->select();
      
      int i;
      for(i=0; i<grid->interiorE.capacity(); i++)
	if(grid->interiorE[i]->isselected())
	  replaceelement(&grid->interiorE[i], (ELEMENT*)0)->select();
      for(i=0; i<grid->boundaryE.capacity(); i++)
	if(grid->boundaryE[i]->isselected())
	  replaceelement(&grid->boundaryE[i], (ELEMENT*)0)->select();

      grid->reset_connectivity();
      grid->redraw();
    }
  }
  virtual CommandM *clone() const {
    return new ReplaceSelectedCmd<ELEMENT>;
  }
};

// ---------------------------------------------------------------- //

Menu *modifymenu() {
  Menu *mm = new Menu("modify");
    
  Menu *replaceselmenu =
    mm->AddSubMenu("replace", "replace selected elements");

  /* ELEMENTS */
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<EmptyElement>);
#ifdef THERMAL
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<GasElement>);
#endif
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<IsoElement>); 
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<CubicElement>); 
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<HexElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<TrigonalAElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<TrigonalBElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<TrigonalCrack>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<TetragonalElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<OrthoElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<DamIsoElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<DamageElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<OrthoExElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<GriffithElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<GriffithElement2>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<ZimmerElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<ZimmerElement2>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<KesslerElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<TwisElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<CubicCrackElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<GradyElement>);
  replaceselmenu->AddCommand(new ReplaceSelectedCmd<ValeriaElement>);

  mm->AddSubMenu("select", "select and unselect elements", selectelemmenu());
  return mm;
}
