// -*- C++ -*-
// $RCSfile: initfile.C,v $
// $Revision: 1.10 $
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


#define GUNZIP "gunzip -c"
#define GZIPSUFFIX ".gz"
#define UNCOMPRESS "uncompress"
#define COMPRESSSUFFIX ".Z"
 
#include "addfilter.h"
#include "bc.h"
#include "binaryread.h"
//#include "cmdlist.h"
#include "createelement.h"
#include "distort.h"
#include "elementgroups.h"
#include "formdrawer.h"
#include "grid.h"
#include "griditerator.h"
#include "main.h"
#include "nodegroups.h"
#include "initfile.h"
#include "menuDef.h"
#include "node.h"
#include "parameters.h"
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
  if(inputfile_programname != programname) {
    cerr << "You can't read that input file with this program!" << endl;
    stoploading = true;
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
#ifdef THERMAL
  double dphi, fphi;
#endif // THERMAL
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
#ifdef THERMAL
    AddArgument(this, "dT", dphi);
    AddArgument(this, "fT", fphi, MOPT_SECRET);
#endif // THERMAL
  }
  virtual void func() {
    XYNode *newnode = XYNode::create(index, x, y, dx, dy
#ifdef THERMAL
				     , dphi
#endif // THERMAL
				     );
    newnode->applyforce(MeshCoord(fx, fy));
#ifdef THERMAL
    newnode->apply_surfcharge(fphi);
#endif // THERMAL
  }
  virtual NodeCmd *subclone() const {
    XYNodeCmd *xync = new XYNodeCmd;
    xync->x = x;
    xync->y = y;
    xync->dx = dx;
    xync->dy = dy;
    xync->fx = fx;
    xync->fy = fy;
#ifdef THERMAL
    xync->dphi = dphi;
    xync->fphi = fphi;
#endif // THERMAL
    return xync;
  }
};

class LinearNodeCmd : public NodeCmd {
private:
  double x, y, dx, dy, fx, fy;
#ifdef THERMAL
  double fphi, dphi;
#endif // THERMAL
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
#ifdef THERMAL
    AddArgument(this, "dT", dphi);
    AddArgument(this, "fT", fphi, MOPT_SECRET);
#endif
  } // THERMAL
  virtual void func() {
    LinearNode *newnode = LinearNode::create(index, x, y, dx, dy,
#ifdef THERMAL
					     dphi,
#endif // THERMAL
					     T);
    newnode->applyforce(MeshCoord(fx, fy));
#ifdef THERMAL
    newnode->apply_surfcharge(fphi);
#endif // THERMAL
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
#ifdef THERMAL
    xync->dphi = dphi;
    xync->fphi = fphi;
#endif // THERMAL
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
  currentNgroup->Add(grid->node[currentnumber]);
}

static void makenewEgroup(const CharString&) {
  currentEgroup = grid->getEgroup(currentgrouplabel, 1);
}

static void putelemingroup(const CharString&) {
  currentEgroup->Add(grid->interiorE[currentnumber]);
}

// set properties of node groups

static CommandFn fixgroup_x() { currentNgroup->fix_x(); }
static CommandFn fixgroup_y() { currentNgroup->fix_y(); }
static CommandFn enslavegroup_x() { currentNgroup->enslavenodes_x(); }
static CommandFn enslavegroup_y() { currentNgroup->enslavenodes_y(); }
#ifdef THERMAL
static CommandFn fixgroup_phi() { currentNgroup->fix_phi(); }
static CommandFn enslavegroup_phi() { currentNgroup->enslavenodes_phi(); }
#endif // THERMAL

static TwoTwo incremental_distortion; // if this code were going to survive
static MeshCoord incremental_shift; // the transition to OOF2, it would be
static MeshCoord incremental_force; // worth putting all these in a class.  
static TwoTwo absolute_distortion;
static MeshCoord absolute_shift;
static MeshCoord absolute_force;
#ifdef THERMAL
static ThermalDistortion incremental_thermaldistortion;
static ThermalDistortion absolute_thermaldistortion;
#endif // THERMAL

static void set_current_distortion(const CharString &str) {
  if(str == "incremental_distortion")
    currentNgroup->incremental_distortion = incremental_distortion;
  if(str == "incremental_shift")
    currentNgroup->incremental_shift = incremental_shift;
  if(str == "incremental_force")
    currentNgroup->incremental_force = incremental_force;
  if(str == "absolute_distortion")
    currentNgroup->absolute_distortion = absolute_distortion;
  if(str == "absolute_shift")
    currentNgroup->absolute_shift = absolute_shift;
  if(str == "absolute_force")
    currentNgroup->absolute_force = absolute_force;
#ifdef THERMAL
  if(str == "incremental_thermaldistortion")
    currentNgroup->incremental_thermaldistortion =incremental_thermaldistortion;
  if(str == "absolute_thermaldistortion")
    currentNgroup->absolute_thermaldistortion = absolute_thermaldistortion;
#endif // THERMAL
}

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
#ifdef THERMAL
    elemmenu->AddCommand(new ElementCmd<GasElement>);
#endif // THERMAL
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
    elemmenu->AddCommand(new ElementCmd<GradyElement>);
    elemmenu->AddCommand(new ElementCmd<ValeriaElement>);
    
    Menu *groupmenu = lm->AddSubMenu("nodegroup", "label groups of nodes");
    AddVariable(groupmenu, "label", "group label", currentgrouplabel,
		getnodegroup);
    AddVariable(groupmenu, "node", "node number", currentnumber,
		putnodeingroup);
    groupmenu->AddCommand("fix_x", "fix x", fixgroup_x);
    groupmenu->AddCommand("fix_y", "fix y", fixgroup_y);
    groupmenu->AddCommand("enslave_x", "enslave x", enslavegroup_x);
    groupmenu->AddCommand("enslave_y", "enslave y", enslavegroup_y);
#ifdef THERMAL
    groupmenu->AddCommand("fix_T", "fix T", fixgroup_phi);
    groupmenu->AddCommand("enslave_T", "enslave T", enslavegroup_phi);
#endif // THERMAL
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
#ifdef THERMAL
    AddVariable(groupmenu, "incremental_thermaldistortion",
		"incremental_thermaldistortion",
		incremental_thermaldistortion, set_current_distortion);
    AddVariable(groupmenu, "absolute_thermaldistortion",
		"absolute_thermaldistortion",
		absolute_thermaldistortion, set_current_distortion);
#endif // THERMAL
    Menu *egroupmenu = lm->AddSubMenu("elementgroup",
				      "label groups of elements");
    AddVariable(egroupmenu, "label", "group label", currentgrouplabel,
		makenewEgroup);
    AddVariable(egroupmenu, "elem", "element number", currentnumber,
		putelemingroup);

    AddVariable(lm, "program", "what program should be running?",
	        inputfile_programname, checkprogram, MOPT_NOLOG|MOPT_NOPRINT);
    
    // the whole oof menu is added as a submenu, so that
    // initialization commands in the input file can be performed. The
    // commands in the input file must be preceded by "oof", which is
    // the name of the submenu.
    lm->AddSubMenu("oof", "all the rest", get_rootmenu());
    
  }
  return lm;
}

#include "timer.h"

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
    garcon()->graphics_off();
    FormDrawer::hold_all();

    // delete old grid, retaining pointer so that new grid can be put
    // in its place in the grid lists (which aren't really used...)
    Grid *oldgrid = grid;
    if(grid) delete grid;
    grid = new Grid;
    FormDrawer::replace_grid(oldgrid, grid); // would draw, if not for hold_all

    // check for compressed input file. This has to be done after the
    // old grid is deleted. Otherwise, the fork() in addfilter() will
    // use too much memory.
    int len = strlen(filename);
    if(len > 3 &&
       !strcmp(&filename[len-strlen(GZIPSUFFIX)], GZIPSUFFIX)) {
      gridfile = addfilter(GUNZIP, gridfile, "r");
    }
    else if (len > 2 &&
	     !strcmp(&filename[len-strlen(COMPRESSSUFFIX)], COMPRESSSUFFIX)) {
      gridfile = addfilter(UNCOMPRESS, gridfile, "r");
    }
  
    Timer timer;
    InitInfo::reset();
    stoploading = 0;
    //    grid->bccmds->ignore_prefix(1);
    //    grid->distortcmds->ignore_prefix(1);
    garcon()->inhibit_logging();

    garcon()->runfile(gridfile, loadmenu(), 0, stop_loading);

    garcon()->disinhibit_logging();
    //    grid->bccmds->ignore_prefix(0);
    //    grid->distortcmds->ignore_prefix(0);
    garcon()->graphics_on();
    FormDrawer::GridUpdated(grid); // computes bounds
    FormDrawer::center_image(grid);
    FormDrawer::release_hold_all();	// allow grid to be redrawn

    garcon()->msout << ms_info << "  Grid read!" << endl
		    << timer << endl << ms_normal;
    if(gridfile != stdin) fclose(gridfile);
  }
}

// ------------------------------------------------------------ //

void Grid::asciioutput(ostream &os) {
  os << "program = " << programname << endl;
  os << "version number = " << InitInfo::outputversion << endl;
  os << "Nnodes = " << node.capacity() << endl;
  os << "Nelements = "
     << grid->interiorE.capacity() + grid->boundaryE.capacity() << endl;
  
  os << "nodes (" << endl;
  int i;
  for(i=0; i<node.capacity(); i++)
    os << *node[i] << endl;
  os << ")" << endl;
  
  ElementIterator iter(EI_ALL, this);
  Element *el;
  os << "elements (" << endl;
  while(el = iter())
    os << *el << endl;
  os << ")" << endl;
  
  for(i=0; i<nodegroup.capacity(); i++) {
    NodeGroup *ng = nodegroup[i];
    os << "nodegroup (" << endl;
    os << " label=" << ng->name() << endl;
    Node *knowed;
    GroupIterator<Node> iter(*ng);
    while(knowed = iter())
      os << " node=" << knowed->index << endl;

    // This part of the nodegroup data dump has to be repeated in
    // binarywrite() in binaryread.C.  Repeating it is ugly, but the
    // binary files are read by stdio, so the ascii bits at the end
    // have to be read by stdio too, so this routine and that one
    // can't easily share code.
    if(ng->xflag.fixed()) os << " fix_x" << endl;
    if(ng->yflag.fixed()) os << " fix_y" << endl;
    if(ng->xflag.enslaved()) os << " enslave_x" << endl;
    if(ng->yflag.enslaved()) os << " enslave_y" << endl;
#ifdef THERMAL
    if(ng->phiflag.fixed()) os << " fix_T" << endl;
    if(ng->phiflag.enslaved()) os << " enslave_T" << endl;
#endif // THERMAL
    os << " incremental_distortion=" << ng->incremental_distortion << endl;
    os << " incremental_shift=" << ng->incremental_shift << endl;
    os << " incremental_force=" << ng->incremental_force << endl;
    os << " absolute_distortion=" << ng->absolute_distortion << endl;
    os << " absolute_shift=" << ng->absolute_shift << endl;
    os << " absolute_force=" << ng->absolute_force << endl;
#ifdef THERMAL
    os << " incremental_thermaldistortion=" << ng->incremental_thermaldistortion
       << endl;
    os << " absolute_thermaldistortion=" << ng->absolute_thermaldistortion
       << endl;
#endif // THERMAL
    os << ")" << endl;
  }
    
  for(i=0; i<elementgroup.capacity(); i++) {
    ElementGroup *eg = elementgroup[i];
    os << "elementgroup (" << endl;
    os << "label=" << eg->name() << endl;
    Element *elem;
    GroupIterator<Element> iter(*eg);
    while(elem = iter())
      os << "elem=" << elem->index << endl;
    os << ")" << endl;
  }
    
  //  grid->bccmds->show(os, "oof");
  //  grid->distortcmds->show(os, "oof");
  get_rootmenu()->print(os, "oof", TF_TRUE);
}
