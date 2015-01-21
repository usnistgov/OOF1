// -*- C++ -*-
// $RCSfile: select.C,v $
// $Revision: 1.5 $
// $Author: langer $
// $Date: 2000-11-15 19:23:38 $


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
#include "menuDef.h"
#include "node.h"
#include "nodegroups.h"
#include "parameters.h"
#include "random.h"
#include "select.h"
#include "shape.h"
#include "mvvd.h"
#include "typeregistry.h"


// --------------------------------------------------------------- //

/* select elements with commands */

CommandFn unselectallelements() {
  if(gridinit()) {
    grid->unselectallelements();
    grid->redraw();
  }
}

static CommandFn selectall() {
  if(gridinit()) {
    grid->selectallelements();
    grid->redraw();
  }
}

static CommandFn toggleselect() {
  if(gridinit()) {
    grid->toggleselect();
    grid->redraw();
  }
}

class SelectRandomCmd : public CommandM {
private:
  double probability;
public:
  SelectRandomCmd() : CommandM("random", "select random elements"), 
    probability(0.5) {
      AddArgument(this, "probability", probability);
    }
  virtual void func() {
    if(gridinit()) {
      grid->selectrandomelements(probability);
      grid->redraw();
    }
  }
  virtual CommandM *clone() const {
    SelectRandomCmd *rsc = new SelectRandomCmd;
    rsc->probability = probability;
    return rsc;
  }
};

class SelectNumberCmd : public CommandM {
private:
  int which;
public:
  SelectNumberCmd()
    : CommandM("number", "select a numbered element"), 
      which(0)
    {
      AddArgument(this, "n", which);
    }
  virtual void func() {
    if(gridinit()) {
      grid->selectelement(which);
      grid->redraw();
    }
  }
  virtual CommandM *clone() const {
    SelectNumberCmd *snc = new SelectNumberCmd;
    snc->which = which;
    return snc;
  }
};

class SelectCircleCmd : public CommandM {
private:
  Circle circ;
public:
  SelectCircleCmd() : CommandM("circle", "select a circle of elements") {
    AddArgument(this, "x", circ.x);
    AddArgument(this, "y", circ.y);
    AddArgument(this, "radius", circ.r);
  }
  virtual void func() {
    if(gridinit()) {
      grid->selectshape(circ);
      grid->redraw();
    }
  }
  CommandM *clone() const {
    SelectCircleCmd *scc = new SelectCircleCmd;
    scc->circ = circ;
    return scc;
  }
};

// ----------------------------------------------------------- //

class SelectRectangleCmd : public CommandM {
private:
  MeshCoord acorner, othercorner;
public:
  SelectRectangleCmd()
    : CommandM("rectangle", "select a rectangle of elements")
  {
    AddArgument(this, "x0", acorner.x);
    AddArgument(this, "y0", acorner.y);
    AddArgument(this, "x1", othercorner.x);
    AddArgument(this, "y1", othercorner.y);
  }
  virtual void func() {
    if(gridinit()) {
      grid->selectshape(Rectangle(acorner, othercorner));
      grid->redraw();
    }
  }
  CommandM *clone() const {
    SelectRectangleCmd *src = new SelectRectangleCmd;
    src->acorner = acorner;
    src->othercorner = othercorner;
    return src;
  }
};

// ---------------------------------------------------------------- //

class SwissCheeseCmd : public CommandM {
private:
  int maxtries;	// max number of tries to place each circle
  int ncircles;	// number of circles
  double ravg;	// mean radius
  double rdev;	// deviation in radius
  double separation;  // min separation between circles;
public:
  SwissCheeseCmd() : CommandM("swisscheese", "select a bunch of circles"), 
    maxtries(100), ncircles(1), ravg(0), rdev(0), separation(0)
  {
    AddArgument(this, "ncircles", ncircles);
    AddArgument(this, "Ravg", ravg);
    AddArgument(this, "Rdev", rdev);
    AddArgument(this, "separation", separation);
    AddArgument(this, "maxtries", maxtries);
  }
  CommandM *clone() const {
    SwissCheeseCmd *scc = new SwissCheeseCmd;
    scc->maxtries = maxtries;
    scc->ncircles = ncircles;
    scc->ravg = ravg;
    scc->rdev = rdev;
    scc->separation = separation;
    return scc;
  }
  virtual void func() {
    if(gridinit()) {
      Rectangle range(grid->query_extreme_pos());
      double width = range.xmax - range.xmin;
      double height = range.ymax - range.ymin;
      /* Need two lists of circles --
       * one for checking for overlaps here,
       * and one for passing to Grid::selectshape().
       */
      Conglomerate holes;
      Vec<Circle> circ(ncircles);
      int nrejected = 0;
      for(int i=0; i<ncircles; i++) {
	double x, y, r;
	int overlap;
	int done = 0;
	for(int attempt=0; attempt<maxtries && !done; attempt++) {
	  x = range.xmin + width*rndm();
	  y = range.ymin + height*rndm();
	  do { // get a positive radius
	    r = ravg + rdev*gasdev();
	  } while (r < 0);
	  
	  // check for overlaps with existing circles
	  overlap = 0;
	  for(int j=0; j<i && !overlap; j++) {
	    double dx = circ[j].x - x;
	    double dy = circ[j].y - y;
	    double rr = circ[j].r + r + separation;
	    if(dx*dx + dy*dy < rr*rr)
	      overlap = 1;
	  }
	  if(!overlap) {
	    circ[i] = Circle(x, y, r);
	    holes.AddShape(&circ[i]);
	    done = 1;
	  }
	  else 
	    nrejected++;
	}
	if(!done)
	  garcon()->msout << ms_error << "Failed to place circle " << i
			  << "!" << endl << ms_normal;
      }
      grid->selectshape(holes);
      grid->redraw();
      garcon()->msout << ms_info << "rejected " << nrejected << " circles"
		      << endl << ms_normal;
    }
  }
};

// ---------------------------------------------------------------- //

CommandFn selecttypecmd() {
  if(gridinit()) {
    CharString whichtype = garcon()->get_command();
    ElementIterator iter(whichtype, grid);
    Element *el;
    while(el = iter())
      el->select();
    grid->redraw();
  }
}

CommandFn selectinterior() {
  if(gridinit()) {
    grid->shufflelists_elastic();
    ElementIterator iter(EI_INTERIOR, grid);
    Element *el;
    while(el = iter())
      el->select();
    grid->redraw();
  }
}

CommandFn selectboundary() {
  if(gridinit()) {
    grid->shufflelists_elastic();
    ElementIterator iter(EI_BOUNDARY, grid);
    Element *el;
    while(el = iter())
      el->select();
    grid->redraw();
  }
}

// just for debugging 
CommandFn selectalltypes() {
  if(gridinit()) {
    ElementIterator iter(EI_ALL, grid);
    Element *el;
    while(el = iter())
      el->select();
    grid->redraw();
  }
}

// just for debugging 
CommandFn selectnonempty() {
  if(gridinit()) {
    ElementIterator iter(EI_NONEMPTY, grid);
    Element *el;
    while(el = iter())
      el->select();
    grid->redraw();
  }
}

// ---------------------------------------------------------------- //

class SelectGrayCmd : public CommandM {
private:
  float mingray, maxgray;
public:
  SelectGrayCmd() : CommandM("gray", "select elements by gray value"), 
    mingray(0), maxgray(1)
  {
    AddArgument(this, "minimum", mingray);
    AddArgument(this, "maximum", maxgray);
  }
  virtual void func() {
    if(gridinit()) {
      ElementIterator iter(EI_ALL, grid);
      Element *el;
      while(el = iter()) {
	float gray = el->intrinsic_gray;
	if(gray >= mingray && gray <= maxgray)
	  el->select();
      }
      grid->redraw();
    }
  }
  CommandM *clone() const {
    SelectGrayCmd *sgc = new SelectGrayCmd;
    sgc->mingray = mingray;
    sgc->maxgray = maxgray;
    return sgc;
  }
};

// ---------------------------------------------------------------- //

// this menu appears in the home menu

Menu *mainselectmenu() {
  Menu *msmenu = new Menu("select");
  msmenu->AddSubMenu("nodes", "select and unselect nodes",
		     selectnodemenu());
  msmenu->AddSubMenu("elements", "select and unselect elements",
		     selectelemmenu());
  return msmenu;
}

// this one appears in the home select menu and the modify menu

Menu *selectelemmenu() {
  static Menu *sm = 0;
  if(!sm) {
    sm = new Menu("select elements");
    sm->AddCommand("all", "select all elements", selectall);
    sm->AddCommand("toggle", "select unselected elements and vice versa",
		   toggleselect);
    sm->AddCommand("none", "unselect all elements", unselectallelements);
    sm->AddCommand(new SelectNumberCmd);
    sm->AddCommand(new SelectRectangleCmd);
    sm->AddCommand(new SelectCircleCmd);
    sm->AddCommand(new SelectRandomCmd);
    sm->AddCommand(new SwissCheeseCmd);
    sm->AddCommand(new SelectGrayCmd);
	
    Menu *tm = sm->AddSubMenu("type", "select elements by type");
    tm->AddCommand("all", "all types", selectalltypes);
    tm->AddCommand("none", "unselect all elements", unselectallelements);
    tm->AddCommand("interior", "select all interior elements", selectinterior);
    tm->AddCommand("boundary", "select all boundary elements", selectboundary);
    tm->AddCommand("nonempty", "nonempty elements", selectnonempty);
    ElementTypeIterator typeiterator;
    CharString const *elemtype;
    while(elemtype = typeiterator())
      tm->AddCommand(*elemtype,
	     "select all " + *elemtype + " elements", selecttypecmd);
	
    sm->AddSubMenu("group", "select all elements in a group", 
	   new GroupMenu<Element>("group", "select all elements in group", 
					  selectelemgroup));
  }
  return sm;
}

