// -*- C++ -*-
// $RCSfile: initfile.C,v $
// $Revision: 1.5 $
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


#define GUNZIP "gunzip -c"
#define GZIPSUFFIX ".gz"
#define UNCOMPRESS "uncompress"
#define COMPRESSSUFFIX ".Z"
 
#include "addfilter.h"
#include "binaryread.h"
#include "createelement.h"
#include "elementgroups.h"
#include "grid.h"
#include "griditerator.h"
#include "main.h"
#include "nodegroups.h"
#include "initfile.h"
#include "menuDef.h"
#include "node.h"
#include "twotwo.h"
#include <string.h>
#include <stdio.h>

// these are defined in stdio.h for SGI, but not for Sun!
#ifndef SEEK_END
#define SEEK_END 2
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#endif

/* ELEMENTS */
#include "cubicelement.h"
#include "cubic_crack_element.h"
#include "damageelement.h"
#include "damisoelement.h"
#include "emptyelement.h"
#include "griffith_element.h"
#include "hexelement.h"
#include "isoelement.h"
#include "kessler.h"
#include "orthoexpand.h"
#include "orthorhombic.h"
#include "tetragonal.h"
#include "trigonal.h"
#include "twissel.h"
#include "zimmer_element.h"

int InitInfo::outputversion = 5;
int InitInfo::inputversion = -1;
int InitInfo::elementcount = 0;

static char outputtype = 'a';	// a --> ascii, b --> binary
		// setting this varible triggers the call to binaryread()
static FILE *gridfile = 0;
static CharString gridfilename;

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// Anything that sets stoploading will make the parser stop reading
// the input file.
static bool stoploading = false;

static bool stop_loading() {
  return stoploading;
}

static CharString inputfile_programname;

static void checkprogram(const CharString&) {
  if(inputfile_programname != "oof") {
    cerr << "This program does not work with files thermal versions of OOF!"
	 << endl;
    exit(1);
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// commands for adding nodes

class NodeCmd : public CommandM {
protected:
  int index;
public:
  NodeCmd(const CharString &n)
    : CommandM(n, "add " + n + " node"), index(0)
  {
    AddArgument(this, "index", index);
  }
  virtual void func() = 0;
  virtual CommandM *clone() const {
    NodeCmd *nc = subclone();
    nc->index = index;
    return nc;
  }
  virtual NodeCmd *subclone() const = 0;
};

class XYNodeCmd : public NodeCmd {
private:
  double x, y, dx, dy, fx, fy;
public:
  XYNodeCmd() : NodeCmd("xy") {
    x = y = 0;
    dx = dy = 0;
    AddArgument(this, "x", x);
    AddArgument(this, "y", y);
    AddArgument(this, "dx", dx);
    AddArgument(this, "dy", dy);
    AddArgument(this, "fx", fx, MOPT_SECRET);
    AddArgument(this, "fy", fy, MOPT_SECRET);
  }
  virtual void func() {
    (void) XYNode::create(index, x, y);
  }
  virtual NodeCmd *subclone() const {
    XYNodeCmd *xync = new XYNodeCmd;
    xync->x = x;
    xync->y = y;
    xync->dx = dx;
    xync->dy = dy;
    xync->fx = fx;
    xync->fy = fy;
    return xync;
  }
};

class LinearNodeCmd : public NodeCmd {
private:
  double x, y, dx, dy, fx, fy;
  TwoTwo T;
public:
  LinearNodeCmd() : NodeCmd("linear") {
    x = y = 0;
    AddArgument(this, "x", x);
    AddArgument(this, "y", y);
    AddArgument(this, "dx", dx);
    AddArgument(this, "dy", dy);
    AddArgument(this, "T00", T(0, 0));
    AddArgument(this, "T01", T(0, 1));
    AddArgument(this, "T10", T(1, 0));
    AddArgument(this, "T11", T(1, 1));
    AddArgument(this, "fx", fx, MOPT_SECRET);
    AddArgument(this, "fy", fy, MOPT_SECRET);
  }
  virtual void func() {
    (void) LinearNode::create(index, x, y);
  }
  virtual NodeCmd *subclone() const {
    LinearNodeCmd *xync = new LinearNodeCmd;
    xync->x = x;
    xync->y = y;
    xync->dx = dx;
    xync->dy = dy;
    xync->T = T;
    xync->fx = fx;
    xync->fy = fy;
    return xync;
  }
};

// --------------------------------------------------------------- //

// commands for adding elements

// command class for all element commands. handles the indices

// this needs to be able to handle elements with different numbers of nodes!

template <class ELEMENT>
class ElementCmd : public CommandM, public ELEMENT {
private:
  int index;
  Vec<int> nodenumber;
public:
  ElementCmd() : CommandM(ELEMENT::tag(),
			  "add " + ELEMENT::tag() + "Element"),
				   nodenumber(ELEMENT::Nnodes), index(0)
  {
    /* node numbers and index aren't in Element::AddArgs(),
     * so that they don't appear in the UniGridCmd too.
     */
    AddArgument(this, "i", index);
    for(int i=0; i<nodenumber.capacity(); i++) {
      nodenumber[i] = i-1;
      AddArgument(this, "n" + to_charstring(i+1), nodenumber[i]);
    }
    ELEMENT::AddArgs(this); // puts static default params in arg list
  }
  virtual void func() {
    Element::nodenumber_dflt = nodenumber;
    Element::index_dflt = index;
    // inputformatflag could be leftover from earlier binary read.
    Element::inputformatflag = 0;
    ELEMENT::OptionalArgHandler();
    (void) create_element((ELEMENT*)0);
    Element::reset_defaults();
  }
  virtual CommandM *clone() const {
    ElementCmd<ELEMENT> *ec = new ElementCmd<ELEMENT>;
    ec->nodenumber = nodenumber;
    return ec;
  }
};

// --------------------------------------------------------------- //

class OutputVersionNumber : public CommandM {
private:
  int number;
public:
  OutputVersionNumber()
    : CommandM("version", "confirm version number"), number(0)
  {
    AddArgument(this, "number", number);
  }
  virtual CommandFn func() {
    InitInfo::inputversion = number;
    if(number > InitInfo::outputversion) {
      garcon()->msout << ms_error
		      << "*** Input file has wrong version number!"
		      << endl << ms_normal;
      stoploading = true;
    }
  }
  virtual CommandM *clone() const {
    return new OutputVersionNumber;
  }
};

// --------------------------------------------------------------- //

// put nodes in groups and assign boundary conditions

static CharString currentgrouplabel("you_forgot_to_assign_a_group_label!");
static int currentnumber = 0;	// node or element index
static NodeGroup* currentNgroup = 0;
static ElementGroup* currentEgroup = 0;

/* The next bunch of functions are hooks called when the group or node
 * variables are set in the nodegroup menu. 
 */

static void getnodegroup(const CharString&) {
  currentNgroup = grid->getgroup(currentgrouplabel, 1);
}

static void putnodeingroup(const CharString&) {
  currentNgroup->add(grid->node[currentnumber]);
}

static void makenewEgroup(const CharString&) {
  currentEgroup = grid->getEgroup(currentgrouplabel, 1);
}

static void putelemingroup(const CharString&) {
  currentEgroup->Add(grid->interiorE[currentnumber]);
}

// set properties of node groups

static CommandFn fixgroup_x() {  }
static CommandFn fixgroup_y() {  }
static CommandFn enslavegroup_x() {  }
static CommandFn enslavegroup_y() {  }

static TwoTwo incremental_distortion; // if this code were going to survive
static MeshCoord incremental_shift; // the transition to OOF2, it would be
static MeshCoord incremental_force; // worth putting all these in a class.  
static TwoTwo absolute_distortion;
static MeshCoord absolute_shift;
static MeshCoord absolute_force;


static void set_current_distortion(const CharString &) {  }

// --------------------------------------------------------------- //

/* This function is called when the "type" variable in the loadmenu is set.
 * If the type is 'b',  then the binary reader is called immediately, and
 * control is passed back to the menu parser only when the binary reader
 * is done.
 */
static void setoutputtype(const CharString&) {
  if(outputtype == 'b') {
    if(!binaryread(gridfile)) {
      garcon()->msout << ms_error
		      << "Warning: Defective binary grid file!"
		      << endl << ms_normal;
      fseek(gridfile, 0L, SEEK_END);
    }
  }
}

static int Nnodes_dflt;
static int Nelements_dflt;

static void setNnodes(const CharString&) {
  grid->setNnodes(Nnodes_dflt);
}

static void setNelements(const CharString&) {
  grid->setNelements(Nelements_dflt);
}

// --------------------------------------------------------------- //

/* construct the menu if necessary. It's done this way so that the
 * menu is constructed the first time it's used, but not before. It's
 * used each time initfile() is executed. initfile() is called by
 * LoadGrid::func().
 */

Menu *loadmenu() {
  static Menu *lm = 0;
  if(!lm) {
    lm = new Menu("load");
    lm->AddCommand(new OutputVersionNumber);
    AddVariable(lm, "type", "a(scii) or b(inary)", outputtype, setoutputtype);
    AddVariable(lm, "Nnodes", "number of nodes", Nnodes_dflt, setNnodes);
    AddVariable(lm, "Nelements", "number of elements", Nelements_dflt,
		setNelements);
    
    Menu *nodemenu = lm->AddSubMenu("nodes", "add various types of nodes");
    nodemenu->AddCommand(new XYNodeCmd);
    nodemenu->AddCommand(new LinearNodeCmd);
    
    Menu *elemmenu = lm->AddSubMenu("elements",
				    "add various types of elements");
    /* ELEMENTS */
    elemmenu->AddCommand(new ElementCmd<EmptyElement>);
    elemmenu->AddCommand(new ElementCmd<IsoElement>);
    elemmenu->AddCommand(new ElementCmd<CubicElement>);
    elemmenu->AddCommand(new ElementCmd<HexElement>);
    elemmenu->AddCommand(new ElementCmd<TrigonalAElement>);
    elemmenu->AddCommand(new ElementCmd<TrigonalBElement>);
    elemmenu->AddCommand(new ElementCmd<TrigonalCrack>);
    elemmenu->AddCommand(new ElementCmd<TetragonalElement>);
    elemmenu->AddCommand(new ElementCmd<OrthoElement>);
    elemmenu->AddCommand(new ElementCmd<DamIsoElement>);
    elemmenu->AddCommand(new ElementCmd<DamageElement>);
    elemmenu->AddCommand(new ElementCmd<OrthoExElement>);
    elemmenu->AddCommand(new ElementCmd<GriffithElement>);
    elemmenu->AddCommand(new ElementCmd<GriffithElement2>);
    elemmenu->AddCommand(new ElementCmd<ZimmerElement>);
    elemmenu->AddCommand(new ElementCmd<ZimmerElement2>);
    elemmenu->AddCommand(new ElementCmd<KesslerElement>);
    elemmenu->AddCommand(new ElementCmd<TwisElement>);
    elemmenu->AddCommand(new ElementCmd<CubicCrackElement>);
    
    Menu *groupmenu = lm->AddSubMenu("nodegroup", "label groups of nodes");
    AddVariable(groupmenu, "label", "group label", currentgrouplabel,
		getnodegroup);
    AddVariable(groupmenu, "node", "node number", currentnumber,
		putnodeingroup);
    groupmenu->AddCommand("fix_x", "fix x", fixgroup_x);
    groupmenu->AddCommand("fix_y", "fix y", fixgroup_y);
    groupmenu->AddCommand("enslave_x", "enslave x", enslavegroup_x);
    groupmenu->AddCommand("enslave_y", "enslave y", enslavegroup_y);
    AddVariable(groupmenu, "incremental_distortion", "incremental_distortion",
		incremental_distortion, set_current_distortion);
    AddVariable(groupmenu, "incremental_shift", "incremental_shift",
		incremental_shift, set_current_distortion);
    AddVariable(groupmenu, "incremental_force", "incremental_force",
		incremental_force, set_current_distortion);
    AddVariable(groupmenu, "absolute_distortion", "absolute_distortion",
		absolute_distortion, set_current_distortion);
    AddVariable(groupmenu, "absolute_shift", "absolute_shift",
		absolute_shift, set_current_distortion);
    AddVariable(groupmenu, "absolute_force", "absolute_force",
		absolute_force, set_current_distortion);
    Menu *egroupmenu = lm->AddSubMenu("elementgroup",
				      "label groups of elements");
    AddVariable(egroupmenu, "label", "group label", currentgrouplabel,
		makenewEgroup);
    AddVariable(egroupmenu, "elem", "element number", currentnumber,
		putelemingroup);

    AddVariable(lm, "program", "what program should be running?",
	        inputfile_programname, checkprogram);
    
    // the whole oof menu is added as a submenu, so that
    // initialization commands in the input file can be performed. The
    // commands in the input file must be preceded by "oof", which is
    // the name of the submenu.
    //    lm->AddSubMenu("oof", "all the rest", get_rootmenu());
    
  }
  return lm;
}

void initfile(const CharString &filename) {
  if(filename == "-")
    gridfile = stdin;
  else
    gridfile = fopen(filename, "r");
  gridfilename = filename; // save for reopening
  
  if(!gridfile) {
    garcon()->msout << ms_error << "Can't open grid file " << filename << "!"
		    << endl << ms_normal;
  }
  else {
    if(grid) delete grid;
    grid = new Grid;

    // check for compressed input file.
    int len = strlen(filename);
    if(len > 3 &&
       !strcmp(&filename[len-strlen(GZIPSUFFIX)], GZIPSUFFIX)) {
      gridfile = addfilter(GUNZIP, gridfile, "r");
    }
    else if (len > 2 &&
	     !strcmp(&filename[len-strlen(COMPRESSSUFFIX)], COMPRESSSUFFIX)) {
      gridfile = addfilter(UNCOMPRESS, gridfile, "r");
    }
  
    InitInfo::reset();
    stoploading = 0;
    garcon()->inhibit_logging();
    garcon()->runfile(gridfile, loadmenu(), 0, stop_loading);
    garcon()->disinhibit_logging();
    if(gridfile != stdin) fclose(gridfile);
  }
}

