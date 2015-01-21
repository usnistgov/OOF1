// -*- C++ -*-
// $RCSfile: init.C,v $
// $Revision: 1.11 $
// $Author: langer $
// $Date: 2005-02-17 21:31:18 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// initialization commands for uniform grid


#include <math.h>
#include "debug.h"
#include "enum.h"
#include "formdrawer.h"
#include "generate.h"
#include "grid.h"
#include "initfile.h"
#include "menuDef.h"
#include "node.h"
#include "parameters.h"
#include "random.h"
#include "typeregistry.h"

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
// parameters for uniform mesh
enum NodeStyle {XY, LIN};

class UM {
public:
  // putting all this stuff in a class makes the Sun compiler happy. I
  // don't know why.
  static int nx;
  static int ny;
  static double width;
  static double height;
  static Enum<NodeStyle> inodetype; // types of nodes for interior and boundary
  static Enum<NodeStyle> bnodetype;
  static Enum<DiagStyle> style; // how the diagonals are arranged
};

int UM::nx = 20;
int UM::ny = 20;
double UM::width = 1.0;
double UM::height = 1.0;
Enum<NodeStyle> UM::inodetype(XY);
Enum<NodeStyle> UM::bnodetype(XY);
Enum<DiagStyle> UM::style(LEANLEFT);


// these templates aren't instantiated properly and have to be done explicitly
template <>
VarObject* Var<Enum<DiagStyle> >::create_varobject(Variable *var, Form *form,
						   VarObjectType t,
						   int x, int y, int w, int h)
{
  return new EnumVarObject<DiagStyle>(var, form, t, x, y, w, h);
}

template <>
VarObject *Var<Enum<NodeStyle> >:: create_varobject(Variable *var, Form *form,
						   VarObjectType t,
						   int x, int y, int w, int h)
{
  return new EnumVarObject<NodeStyle>(var, form, t, x, y, w, h);
}

// just a declaration of a specialization to keep the Sun compiler
// happy.  [Since we haven't had access to a Sun in a while, I don't
// know what that comment really means.  This declaration used to be
// in an #ifndef sgi/#endif pair, but that seems not to be necessary
// any more.
template <>
VarObject* Var<TrueFalse>::create_varobject(Variable*, Form*,
                                    VarObjectType, int, int, int, int);

// ------------------------------------------------------------ //

void DEBUGprintelems();

template <class ELEMENT>
class UniGridCmd : public CommandM, public ELEMENT {
public:
  UniGridCmd() : CommandM(ELEMENT::tag(), 
			  "set parameters for " + ELEMENT::tag() + " elements")
  {
    ELEMENT::AddArgs(this);
  }
  virtual void func() {
    Grid *oldgrid = grid;
    if(grid) delete grid;
    if(UM::inodetype == XY && UM::bnodetype == XY)
      grid = generate_uniform((ELEMENT*)0, (XYNode*)0, (XYNode*)0,
			      UM::nx, UM::ny, UM::width, UM::height,
			      DiagStyle(UM::style));
    else if(UM::inodetype == XY && UM::bnodetype == LIN)
      grid = generate_uniform((ELEMENT*)0, (XYNode*)0, (LinearNode*)0,
			      UM::nx, UM::ny, UM::width, UM::height,
			      DiagStyle(UM::style));
    else if(UM::inodetype == LIN && UM::bnodetype == LIN)
      grid = generate_uniform((ELEMENT*)0, (LinearNode*)0, (LinearNode*)0,
			      UM::nx, UM::ny, UM::width, UM::height,
			      DiagStyle(UM::style));
    else if(UM::inodetype == LIN && UM::bnodetype == XY)
      grid = generate_uniform((ELEMENT*)0, (LinearNode*)0, (XYNode*)0,
			      UM::nx, UM::ny, UM::width, UM::height,
			      DiagStyle(UM::style));
	    
    FormDrawer::replace_grid(oldgrid, grid); // draws grid
    //    grid->redraw();
  }
  virtual CommandM *clone() const {
    return new UniGridCmd<ELEMENT>;
  }
};

// ------------------------------------------------------------ //

static CommandFn listreaders() {
    garcon()->msout << "Node types:" << endl;
    Vec<NodeTypeRegistration*> &noderegister = node_registry();
    int i;
    for(i=0; i< noderegister.capacity(); i++)
	garcon()->msout << "  " << noderegister[i]->name() << endl;

    garcon()->msout << "Element types:" << endl;
    Vec<ElementTypeRegistration*> &elementregister = element_registry();
    for(i=0; i< elementregister.capacity(); i++)
	garcon()->msout << "  " << elementregister[i]->name() << endl;
}

// --------------------------------------------------------------- //

Menu *initmenu() {
    static Menu *im = 0;
    if(im) return im;
    im = new Menu("initialize");

    Enum<DiagStyle>::name(LEANLEFT, "liberal");
    Enum<DiagStyle>::name(LEANRIGHT, "conservative");
    Enum<DiagStyle>::name(ALTERNATE, "moderate");
    Enum<DiagStyle>::name(RANDOM, "anarchic");
    Enum<NodeStyle>::name(XY, "xy");
    Enum<NodeStyle>::name(LIN, "linear");

    im->AddCommand(new LoadGrid);
    im->AddCommand("show_types", "list known element and node types",
	listreaders);

    Menu *unimenu = im->AddSubMenu("uniform", "create uniform grid");
    AddVariable(unimenu, "nx", "number of nodes in x direction", UM::nx);
    AddVariable(unimenu, "ny", "number of nodes in y direction", UM::ny);
    AddVariable(unimenu, "width", "size in x direction", UM::width);
    AddVariable(unimenu, "height", "size in y direction", UM::height);
    AddVariable(unimenu, "diagonals", "how to put diagonals in rectangles?",
		UM::style);
    AddVariable(unimenu, "interior_nodes", "interior node type", UM::inodetype);
    AddVariable(unimenu, "boundary_nodes", "boundary node type", UM::bnodetype);

    /* ELEMENTS */
    unimenu->AddCommand(new UniGridCmd<EmptyElement>);
#ifdef THERMAL
    unimenu->AddCommand(new UniGridCmd<GasElement>);
#endif // THERMAL
    unimenu->AddCommand(new UniGridCmd<IsoElement>);
    unimenu->AddCommand(new UniGridCmd<CubicElement>);
    unimenu->AddCommand(new UniGridCmd<HexElement>);
    unimenu->AddCommand(new UniGridCmd<TrigonalAElement>);
    unimenu->AddCommand(new UniGridCmd<TrigonalBElement>);
    unimenu->AddCommand(new UniGridCmd<TrigonalCrack>);
    unimenu->AddCommand(new UniGridCmd<TetragonalElement>);
    unimenu->AddCommand(new UniGridCmd<OrthoElement>);
    unimenu->AddCommand(new UniGridCmd<OrthoExElement>);
    unimenu->AddCommand(new UniGridCmd<DamIsoElement>);
    unimenu->AddCommand(new UniGridCmd<DamageElement>);
    unimenu->AddCommand(new UniGridCmd<GriffithElement>);
    unimenu->AddCommand(new UniGridCmd<GriffithElement2>);
    unimenu->AddCommand(new UniGridCmd<ZimmerElement>);
    unimenu->AddCommand(new UniGridCmd<ZimmerElement2>);
    unimenu->AddCommand(new UniGridCmd<KesslerElement>);
    unimenu->AddCommand(new UniGridCmd<TwisElement>);
    unimenu->AddCommand(new UniGridCmd<CubicCrackElement>);
    unimenu->AddCommand(new UniGridCmd<GradyElement>);
    unimenu->AddCommand(new UniGridCmd<ValeriaElement>);
    return im;
}

#include "enumio.C"
