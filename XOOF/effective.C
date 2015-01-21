// -*- C++ -*-
// $RCSfile: effective.C,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2001-11-30 15:26:07 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "effective.h"
#include "main.h"
#include "menuDef.h"
#include "nodegroups.h"
#include "parameters.h"

class YoungXCmd : public CommandM {
private:
  double strain;
public:
  YoungXCmd()
    : CommandM("Ex", "compute Young's modulus in x direction"),
      strain(0.01)
  {	
    AddArgument(this, "strain", strain);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      CharString cmd =
	"bc (free all; fix x (left; right));" 
	"bc emancipate all;"
	"distort clear all;"
	"distort "
#ifdef THERMAL
	"elastic "
#endif // THERMAL
	"(set (reset_params; xstrain=" + to_charstring(strain) + "; right));"
	"distort increment;"
#ifdef THERMAL
	"equilibrate elastic";
#else // !THERMAL
	"equilibrate";
#endif // THERMAL

      ScriptWaiter sw(cmd, garcon()->msout);
      sw.order(get_rootmenu());

      NodeGroup *rgt = grid->getgroup("right", TF_FALSE);
      MeshCoord f = grid->force(rgt);
      garcon()->msout << "E = " << f.x/(rgt->height()*strain) << endl;
    }	
  }
  virtual CommandM *clone() const {
    YoungXCmd *yxc = new YoungXCmd;
    yxc->strain = strain;
    return yxc;
  }
};

class YoungYCmd : public CommandM {
private:
  double strain;
public:
  YoungYCmd()
    : CommandM("Ey", "compute Young's modulus in y direction"),
      strain(0.01)
  {	
    AddArgument(this, "strain", strain);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      CharString cmd =
	"bc (free all; fix y (top; bottom));" 
	"bc emancipate all;"
	"distort clear all;"
	"distort "
#ifdef THERMAL
	"elastic "
#endif // THERMAL
	"(set (reset_params; ystrain=" + to_charstring(strain) + "; top));"
	"distort increment;"
#ifdef THERMAL
	"equilibrate elastic";
#else // !THERMAL
	"equilibrate";
#endif // THERMAL

      ScriptWaiter sw(cmd, garcon()->msout);
      sw.order(get_rootmenu());

      NodeGroup *top = grid->getgroup("top", TF_FALSE);
      MeshCoord f = grid->force(top);
      garcon()->msout << "E = " << f.y/(top->width()*strain) << endl;
    }	
  }
  virtual CommandM *clone() const {
    YoungYCmd *yyc = new YoungYCmd;
    yyc->strain = strain;
    return yyc;
  }
};

Menu *effectivemenu() {
  static Menu *menu = 0;
  if(menu)
    return menu;

  menu = new Menu("effective properties");

  menu->AddCommand(new YoungXCmd);
  menu->AddCommand(new YoungYCmd);
  return menu;
}
