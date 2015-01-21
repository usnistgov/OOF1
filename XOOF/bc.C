// -*- C++ -*-
// $RCSfile: bc.C,v $
// $Revision: 1.8 $
// $Author: langer $
// $Date: 2000-11-20 21:11:07 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/* commands to set the dof flags for nodes in groups
 * and to manage the groups
 */

#include "bc.h"
#include "distort.h"
#include "freedom.h"
#include "grid.h"
#include "menuDef.h"
#include "nodegroups.h"
#include "output.h"
#include "parameters.h"
#include "plotout.h"

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

/* These functions set the bc flags in the dofs for groups of nodes.
 * The name of the nodegroup is the name of the command.
 */ 

static CommandFn fixxgroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->fix_x();
  }
}

static CommandFn fixygroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->fix_y();
  }
}

#ifdef THERMAL
static CommandFn fixTgroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->fix_phi();
  }
}
#endif // THERMAL


static CommandFn fixgroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->fix_x();
    n->fix_y();
  }
}

#ifdef THERMAL
static CommandFn fixallgroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->fix_x();
    n->fix_y();
    n->fix_phi();
  }
}
#endif // THERMAL


static CommandFn freexgroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->free_x();
  }
}

static CommandFn freeygroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->free_y();
  }
}

#ifdef THERMAL
static CommandFn freeTgroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->free_phi();
  }
}

static CommandFn freexyTgroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->free_x();
    n->free_y();
    n->free_phi();
  }
}
#endif // THERMAL

static CommandFn freexygroup() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->free_x();
    n->free_y();
  }
}

static CommandFn freeall() {
  if(gridinit()) {
    grid->freeall_nodes_x();
    grid->freeall_nodes_y();
#ifdef THERMAL
    grid->freeall_nodes_phi();
#endif // THERMAL
  }
} 

static CommandFn freexall() {
  if(gridinit()) {
    grid->freeall_nodes_x();
  }
}

static CommandFn freeyall() {
  if(gridinit()) {
    grid->freeall_nodes_y();
  }
}

#ifdef THERMAL
static CommandFn freeTall() {
  if(gridinit()) {
    grid->freeall_nodes_phi();
  }
}
#endif

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static CommandFn enslavex() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->enslavenodes_x();
  }
}

static CommandFn emancipatex() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->emancipatenodes_x();
  }
}

static CommandFn enslavey() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->enslavenodes_y();
  }
}

static CommandFn emancipatey() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->emancipatenodes_y();
  }
}

#ifdef THERMAL
static CommandFn enslaveT() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->enslavenodes_phi();
  }
}

static CommandFn emancipateT() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->emancipatenodes_phi();
  }
}
#endif // THERMAL

static CommandFn enslavexy() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->enslavenodes_x();
    n->enslavenodes_y();
  }
}

static CommandFn emancipatexy() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->emancipatenodes_x();
    n->emancipatenodes_y();
  }
}

#ifdef THERMAL
static CommandFn enslavexyT() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->enslavenodes_x();
    n->enslavenodes_y();
    n->enslavenodes_phi();
  }
}

static CommandFn emancipatexyT() {
  NodeGroup *n;
  if(gridinit() && (n = groupname())) {
    n->emancipatenodes_x();
    n->emancipatenodes_y();
    n->emancipatenodes_phi();
  }
}
#endif // THERMAL

static CommandFn emancipateall() {
  if(gridinit())
    grid->emancipate_all_groups();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

static CommandFn showgroups() {    // list all groups
  if(gridinit()) {
    garcon()->msout << "Node Groups:" << endl;
    grid->showgroups();
  }
}

static CommandFn apply_explicitly() {
  if(gridinit())
    grid->fix_masters();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

Menu *bcmenu() {
  static Menu *menu = 0;
  if(!menu) {
    menu = new Menu("bc");
    
    Menu *fixmenu = menu->AddSubMenu("fix", "turn on fixed displacements");
    fixmenu->AddSubMenu("x", "turn on fixed x displacements",
	new GroupMenu<Node>("x", "fix x displacement for group", fixxgroup));
    fixmenu->AddSubMenu("y", "turn on fixed y displacements", 
	new GroupMenu<Node>("y", "fix y displacement for group", fixygroup));
#ifndef THERMAL
    fixmenu->AddSubMenu("both", "turn on fixed x and y displacements",
	new GroupMenu<Node>("both", "fix x and y displacements for group",
			    fixgroup));
#else // THERMAL
    fixmenu->AddSubMenu("T", "fix temperature to a temperature reservoir", 
	new GroupMenu<Node>("T", "fix temperature for group", fixTgroup));
    fixmenu->AddSubMenu("xy", "turn on fixed x and y displacements",
	new GroupMenu<Node>("xy", "fix x and y displacements for group",
			    fixgroup));
    fixmenu->AddSubMenu("xyT",
	"turn on fixed x and y displacements and temperature T ",
	 new GroupMenu<Node>("xyT",
		     "fix x and y displacements and temperature T for group",
			     fixallgroup));
#endif // THERMAL
    Menu *freemenu = menu->AddSubMenu("free", "don't set displacement");
    freemenu->AddCommand("all", "remove all boundary conditions", freeall);
    Menu *bcfreexmenu = freemenu->AddSubMenu("x",
					     "turn off fixed x displacements", 
	     new GroupMenu<Node>("x", "remove x boundary conditions for group",
				 freexgroup));
    bcfreexmenu->AddCommand("all",
			    "remove all x boundary conditions", freexall);
    Menu *bcfreeymenu = freemenu->AddSubMenu("y",
					     "turn off fixed y displacements",
	     new GroupMenu<Node>("y", "remove y boundary conditions for group",
				 freeygroup));
    bcfreeymenu->AddCommand("all", "remove all y boundary conditions",
			    freeyall);

#ifdef THERMAL
    Menu *bcfreeTmenu = freemenu->AddSubMenu("T",
					     "turn off fixed T reservoir",
	     new GroupMenu<Node>("T", "remove T boundary conditions for group",
				 freeTgroup));
    bcfreeTmenu->AddCommand("all", "remove all T boundary conditions",
			    freeTall);

    freemenu->AddSubMenu("xy", "turn off fixed x and y displacements",
			 new GroupMenu<Node>("xy",
		  "remove x and y boundary conditions for group",
					     freexygroup));
    freemenu->AddSubMenu("xyT", "turn off all fixed boundary conditions",
			 new GroupMenu<Node>("xyT",
			     "remove x, y, and T boundary conditions for group",
					     freexyTgroup));
#else // !THERMAL
    freemenu->AddSubMenu("both", "turn off fixed x and y displacements",
			 new GroupMenu<Node>("both",
		  "remove x and y boundary conditions for group",
					     freexygroup));
#endif // THERMAL
    freemenu->AddCommand("all", "remove all boundary conditions", freeall);

    Menu *slavemenu = menu->AddSubMenu("enslave",
			      "enslave degrees of freedom to one another");
    slavemenu->AddSubMenu("x", "enslave x displacements",
       new GroupMenu<Node>("x", "enslave x displacements for group", enslavex));
    slavemenu->AddSubMenu("y", "enslave y displacements",
       new GroupMenu<Node>("y", "enslave y displacements for group", enslavey));
#ifdef THERMAL
    slavemenu->AddSubMenu("T", "enslave temperature T",
       new GroupMenu<Node>("T", "enslave temperature T for group", enslaveT));
    slavemenu->AddSubMenu("xy", "enslave x and y displacements",
       new GroupMenu<Node>("xy", "enslave x and y displacements for group",
			   enslavexy));
    slavemenu->AddSubMenu("xyT", "enslave x, y, and T",
       new GroupMenu<Node>("xyT", "enslave x, y, and T for group", enslavexyT));
#else // !THERMAL
    slavemenu->AddSubMenu("both", "enslave x and y displacements",
       new GroupMenu<Node>("both", "enslave x and y displacements for group",
			   enslavexy));
#endif
    Menu *emancmenu = menu->AddSubMenu("emancipate",
			      "emancipate degrees of freedom from one another");
    emancmenu->AddCommand("all", "emancipate all degrees of freedom",
			  emancipateall);
    emancmenu->AddSubMenu("x", "emancipate x displacements",
       new GroupMenu<Node>("x", "emancipate x displacements for group",
			   emancipatex));
    emancmenu->AddSubMenu("y", "emancipate y displacements",
       new GroupMenu<Node>("y", "emancipate y displacements for group",
			   emancipatey));
#ifdef THERMAL
    emancmenu->AddSubMenu("T", "emancipate y displacements",
       new GroupMenu<Node>("T", "emancipate y displacements for group",
			   emancipateT));
    emancmenu->AddSubMenu("xy", "emancipate x and y displacements",
       new GroupMenu<Node>("xy", "emancipate x and y displacements for group",
			   emancipatexy));
    emancmenu->AddSubMenu("xyT", "emancipate x, y, and T ",
       new GroupMenu<Node>("xyT", "emancipate x, y, and T for group",
			   emancipatexyT));       
#else // !THERMAL
    emancmenu->AddSubMenu("both", "emancipate x and y displacements",
       new GroupMenu<Node>("both", "emancipate x and y displacements for group",
			   emancipatexy));
#endif // THERMAL
    menu->AddCommand("show", "list node groups and boundary conditions",
		     showgroups);
    
    menu->AddSubMenu("groups", "manage node groups", nodegroupmenu());
    menu->AddCommand("apply", "explicitly apply boundary conditions to nodes",
		     apply_explicitly);
  }
  return menu;
}

