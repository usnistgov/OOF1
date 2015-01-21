// -*- C++ -*-
// $RCSfile: nodegroups.C,v $
// $Revision: 1.20 $
// $Author: langer $
// $Date: 2005-02-17 22:22:47 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */



#include "grid.h"
#include "menuDef.h"
#include "nodegroups.h"
#include "parameters.h"

template <class Node> Vec<GroupMenu<Node>*> GroupMenu<Node>::menulist;
template <class Node> Vec<GAMenu<Node>*> GAMenu<Node>::menulist;

NodeGroup::~NodeGroup() {
  // nodes can't be fixed unless they're in a group, so if a group is
  // deleted it has to free its nodes.
  free_x();
  free_y();
#ifdef THERMAL
  free_phi();
#endif // THERMAL
  grid->nodegroup.remove(this);
}

ostream &operator<<(ostream &os, const NodeGroup &ng) {
    int i = ng.size();
    os << ng.name() << " (" << i << (i == 1? " node" : " nodes");
    os << ", x = " << ng.xflag;
    if(ng.master_x) os << "/enslaved";
    os << ", y = " << ng.yflag;
    if(ng.master_y) os << "/enslaved";
#ifdef THERMAL
    os << ", T = " << ng.phiflag;
    if(ng.master_phi) os << "/enslaved";
#endif // THERMAL
    os << ")";
    return os;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void NodeGroup::apply_absolute_elastic_distortion() const {
  GroupIterator<Node> iter(*this);
  Node *n;
  int do_x = xflag.fixed();
  int do_y = yflag.fixed();
  while(n = iter()) {
    // reset displacements just before setting them, because
    // displacements in master dofs shouldn't be multiplied by the
    // number of slaves.  Also, nodes that are in more than one group
    // should only get one displacement.  Nodes that are in more than
    // one group get the force from each group, and master dofs get
    // the force from each slave, so the forces are not reset.
    n->reset_displacement(do_x && n->x->fixed(), do_y && n->y->fixed());
//     n->distort(absolute_distortion, absolute_shift, 1, 1);
    n->distort(absolute_distortion, absolute_shift,
  	       do_x && n->x->fixed(), do_y && n->y->fixed());
    n->applyforce(absolute_force);
  }
}

static const TwoTwo zero_matrix;
static const MeshCoord zero_vector;

void NodeGroup::clear_elastic_distortion() {
  incremental_distortion = zero_matrix;
  incremental_shift = zero_vector;
  incremental_force = zero_vector;
  absolute_distortion = zero_matrix;
  absolute_shift = zero_vector;
  absolute_force = zero_vector;
}

#ifdef THERMAL

void NodeGroup::apply_absolute_thermal_distortion() const {
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter()) {
    if(phiflag.fixed()) {
      n->reset_voltage();
      n->v_distort(absolute_thermaldistortion);
    }
    n->apply_surfcharge(absolute_thermaldistortion.heatrate);
  }
}

static const ThermalDistortion zerothermaldistortion;

void NodeGroup::clear_thermal_distortion() {
  incremental_thermaldistortion = zerothermaldistortion;
  absolute_thermaldistortion = zerothermaldistortion;
}

#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void NodeGroup::fix_x() {
  if(xflag.fixed()) return;
  xflag.set(FIX_DISP);
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter())
    n->fix_x();
  grid->fix_masters_elastic_needed();
}

void NodeGroup::fix_y() {
  if(yflag.fixed()) return;
  yflag.set(FIX_DISP);
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter())
    n->fix_y();
  grid->fix_masters_elastic_needed();
}

void NodeGroup::free_x() {
  if(!xflag.fixed()) return;
  xflag.clear(FIX_DISP);
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter())
    n->free_x();
  grid->fix_masters_elastic_needed();
}

void NodeGroup::free_y() {
  if(!yflag.fixed()) return;
  yflag.clear(FIX_DISP);
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter())
    n->free_y();
  grid->fix_masters_elastic_needed();
}

void NodeGroup::enslavenodes_x() {
  xflag.set(ENSLAVED);
  grid->fix_masters_elastic_needed();
}

void NodeGroup::enslavenodes_y() {
  yflag.set(ENSLAVED);
  grid->fix_masters_elastic_needed();
}

void NodeGroup::emancipatenodes_x() {
  xflag.clear(ENSLAVED);
  grid->fix_masters_elastic_needed();
}

void NodeGroup::emancipatenodes_y() {
  yflag.clear(ENSLAVED);
  grid->fix_masters_elastic_needed();
}

void NodeGroup::emancipatenodes_all() {
  xflag.clear(ENSLAVED);
  yflag.clear(ENSLAVED);
#ifdef THERMAL
  phiflag.clear(ENSLAVED);
#endif // THERMAL
  grid->fix_masters_needed();
}

#ifdef THERMAL

void NodeGroup::fix_phi() {
  if(phiflag.fixed()) return;
  phiflag.set(FIX_DISP);
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter())
    n->fix_phi();
  grid->fix_masters_thermal_needed();
}

void NodeGroup::free_phi() {
  if(!phiflag.fixed()) return;
  phiflag.clear(FIX_DISP);
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter())
    n->free_phi();
  grid->fix_masters_thermal_needed();
}

void NodeGroup::enslavenodes_phi() {
  phiflag.set(ENSLAVED);
  grid->fix_masters_thermal_needed();
}

void NodeGroup::emancipatenodes_phi() {
  phiflag.clear(ENSLAVED);
  grid->fix_masters_thermal_needed();
}

#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// See comments in freedom.C

void NodeGroup::do_enslavenodes_elastic_all() { // called by Grid::fix_masters()
  if(xflag.enslaved())
    do_enslavenodes_x();
  if(yflag.enslaved())
    do_enslavenodes_y();
}

void NodeGroup::do_emancipatenodes_elastic_all() {
  // called by Grid::fix_masters()
  do_emancipatenodes_x();
  do_emancipatenodes_y();
}

void NodeGroup::do_enslavenodes_x() {
  if(master_x) return;		// don't enslave a group twice
  GroupIterator<Node> iter(*this);
  Node *n;
  MasterDegreeOfFreedom *master = new MasterDegreeOfFreedom;
  grid->AddDoFelastic(master);
  master_x = master;
  double avgdisp = 0;		// set enslaved displacement to avg disp of grp
  while(n = iter()) {
    avgdisp += n->x->disp();
    n->x = n->x->enslave(master, &Grid::AddDoFelastic);
  }
  master->disp() = avgdisp/size();
  grid->require_rehash();
  grid->redraw();
}

void NodeGroup::do_enslavenodes_y() {
  if(master_y) return;		// don't enslave a group twice
  GroupIterator<Node> iter(*this);
  Node *n;
  MasterDegreeOfFreedom *master = new MasterDegreeOfFreedom;
  grid->AddDoFelastic(master);
  master_y = master;
  double avgdisp = 0;		// set enslaved displacement to avg disp of grp
  while(n = iter()) {
    avgdisp += n->y->disp();
    n->y = n->y->enslave(master, &Grid::AddDoFelastic);
  }
  master->disp() = avgdisp/size();
  grid->require_rehash();
  grid->redraw();
}

#ifdef THERMAL

void NodeGroup::do_enslavenodes_thermal_all() { // called by Grid::fix_masters()
  if(phiflag.enslaved())
    do_enslavenodes_phi();
}

void NodeGroup::do_emancipatenodes_thermal_all() {
  // called by Grid::fix_masters().  This function is trivial, but is
  // kept to maintain the analogy between thermal and elastic
  // functions.
  do_emancipatenodes_phi();
}

void NodeGroup::do_enslavenodes_phi() {
  if(master_phi) return;	// don't enslave a group twice
  GroupIterator<Node> iter(*this);
  Node *n;
  MasterDegreeOfFreedom *master = new MasterDegreeOfFreedom;
  grid->AddDoFthermal(master);
  master_phi = master;
  double avgvolt = 0;		// set enslaved displacement to avg disp of grp
  while(n = iter()) {
    avgvolt += n->phi->disp();
    n->phi = n->phi->enslave(master, &Grid::AddDoFthermal);
  }
  master->disp() = avgvolt/size();
}
#endif // THERMAL

// See comments in freedom.C

void NodeGroup::do_emancipatenodes_x() {
  if(!master_x) return;
  //  free_x();
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter()) {
    DegreeOfFreedom *newdof = n->x->emancipate(master_x);
    if(newdof) {
      n->x = newdof;
      n->x->force() = absolute_force.x;
      grid->AddDoFelastic(n->x);
    }
  }
  master_x = 0;
}

void NodeGroup::do_emancipatenodes_y() {
  if(!master_y) return;
  //  free_y();
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter()) {
    DegreeOfFreedom *newdof = n->y->emancipate(master_y);
    if(newdof) {
      n->y = newdof;
      n->y->force() = absolute_force.y;
      grid->AddDoFelastic(n->y);
    }
  }
  master_y = 0;
}

#ifdef THERMAL
void NodeGroup::do_emancipatenodes_phi() {
  if(!master_phi) return;
  //  free_phi();
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter()) {
    DegreeOfFreedom *newdof = n->phi->emancipate(master_phi);
    if(newdof) {
      n->phi = newdof;
      n->phi->force() = absolute_thermaldistortion.heatrate;
      grid->AddDoFthermal(n->phi);
    }
  }
  master_phi = 0;
}
#endif // THERMAL

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

double NodeGroup::width() const {
  GroupIterator<Node> iter(*this);
  Node *n;
  double xmin=   1.e10;
  double xmax = -1.e10;
  while(n = iter()) {
    double x = n->original().x;
    if(x < xmin) xmin = x;
    if(x > xmax) xmax = x;
  }
  return xmax - xmin;
}

double NodeGroup::height() const {
  GroupIterator<Node> iter(*this);
  Node *n;
  double ymin=   1.e10;
  double ymax = -1.e10;
  while(n = iter()) {
    double y = n->original().y;
    if(y < ymin) ymin = y;
    if(y > ymax) ymax = y;
  }
  return ymax - ymin;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// get the group from the command name

NodeGroup *groupname() {
  CharString whichgroup = garcon()->get_command();
  NodeGroup *n = grid->getgroup(whichgroup, TF_FALSE);
  if(!n)
    garcon()->msout << ms_error << "There is no group " << whichgroup << "!"
		    << endl << ms_normal;
  return n;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

void NodeGroup::print_bc(ostream &os, const CharString &meshName) const {
    if(xflag.fixed()) {
    double strain = absolute_distortion(0,0);
    double shear = absolute_distortion(0,1);
    double shift = absolute_shift.x;
    os << "OOF.Mesh.Boundary_Conditions.New(name='" << name()+"_x"
       << "', mesh='" << meshName
       << "', condition=DirichletBC("
       << "field=Displacement, field_component='" << 'x'
       << "', equation=Force_Balance, eqn_component='" << 'x'
       << "', profile=ContinuumProfile(function='"
       << strain << "*x + " << shear << "*y + " << shift 
       << "'), boundary='" << name() << "'))" << endl;
  }
  if(yflag.fixed()) {
    double strain = absolute_distortion(1,1);
    double shear = absolute_distortion(1,0);
    double shift = absolute_shift.y;
    os << "OOF.Mesh.Boundary_Conditions.New(name='" << name()+"_y"
       << "', mesh='" << meshName
       << "', condition=DirichletBC("
       << "field=Displacement, field_component='" << 'y'
       << "', equation=Force_Balance, eqn_component='" << 'y'
       << "', profile=ContinuumProfile(function='"
       << strain << "*y + " << shear << "*x + " << shift 
       << "'), boundary='" << name() << "'))" << endl;
  }
  if(xflag.enslaved() && xflag.isfree()) {
    os << "OOF.Mesh.Boundary_Conditions.New(name='" << name()+"_X"
       << "', mesh='" << meshName
       << "', condition=FloatBC("
       << "field=Displacement, field_component='" << 'x'
       << "', equation=Force_Balance, eqn_component='" << 'x'
       << "', profile=ConstantProfile(value=0.0), boundary='" << name() << "'))"
       << endl;
  }
  if(yflag.enslaved() && yflag.isfree()) {
    os << "OOF.Mesh.Boundary_Conditions.New(name='" << name()+"_Y"
       << "', mesh='" << meshName
       << "', condition=FloatBC("
       << "field=Displacement, field_component='" << 'y'
       << "', equation=Force_Balance, eqn_component='" << 'y'
       << "', profile=ConstantProfile(value=0.0), boundary='" << name() << "'))"
       << endl;
  }
  if(absolute_force.x != 0.0) {
    os << "OOF.Mesh.Boundary_Conditions.New(name='"
       << name() + "_fx"
       << "', mesh='" << meshName
       << "', condition=ForceBC("
       << "equation=Force_Balance, eqn_component='x'"
       << ", profile=ConstantProfile(value=" << absolute_force.x 
       << "), boundary='" << name() << "'))" << endl;
  }
  if(absolute_force.y != 0.0) {
    os << "OOF.Mesh.Boundary_Conditions.New(name='"
       << name() + "_fy"
       << "', mesh='" << meshName
       << "', condition=ForceBC("
       << "equation=Force_Balance, eqn_component='y'"
       << ", profile=ConstantProfile(value=" << absolute_force.y 
       << "), boundary='" << name() << "'))" << endl;
  }
#ifdef THERMAL
  if(phiflag.fixed()) {
    os << "OOF.Mesh.Boundary_Conditions.New(name='" << name()+"_t"
       << "', mesh='" << meshName
       << "', condition=DirichletBC("
       << "field=Temperature, field_component=''"
       << ", equation=Heat_Eqn, eqn_component=''"
       << ", profile=ContinuumProfile(function='"
       << absolute_thermaldistortion.T0 << " + "
       << absolute_thermaldistortion.dTdx << "*x + "
       << absolute_thermaldistortion.dTdy << "*y'), boundary='"
       << name() << "'))" << endl;
  }
  if(phiflag.enslaved() && phiflag.isfree()) {
    os << "OOF.Mesh.Boundary_Conditions.New(name='" << name()+"_T"
       << "', mesh='" << meshName
       << "', condition=FloatBC("
       << "field=Temperature, field_component=''"
       << ", equation=Heat_Eqn, eqn_component=''"
       << ", profile=ConstantProfile(value=0.0), boundary='" << name() << "'))"
       << endl;
  }
  if(absolute_thermaldistortion.heatrate != 0.0) {
    os << "OOF.Mesh.Boundary_Conditions.New(name='"
       << name() + "_fT"
       << "', mesh='" << meshName
       << "', condition=ForceBC("
       << "equation=Heat_Eqn, eqn_component=''"
       << ", profile=ConstantProfile(value="
       << absolute_thermaldistortion.heatrate
       << "), boundary='" << name() << "'))" << endl;
  }
#endif // THERMAL
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

// group management commands

class NewNodeGroupCmd : public CommandM {   // create a new group
private:
  CharString name;
public:
  NewNodeGroupCmd() : CommandM("new", "create a new empty node group"), 
  name("groupname") {
    AddArgument(this, "name", name);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      char *c;
      int modified = 0;
      while(c = (char*) strchr(name, ' ')) { // cast required by Sun
	*c = '_';
	modified = 1;
      }
      if(modified) garcon()->msout << ms_info << "Group name changed to \""
				   << name << "\"" << endl << ms_normal;
      (void) grid->getgroup(name, TF_TRUE);
    }
  }
  virtual CommandM *clone() const {
    NewNodeGroupCmd *nngc = new NewNodeGroupCmd;
    nngc->name = name;
    return nngc;
  }
};

class RenameNGroupCmd : public CommandM {
private:
  CharString newname;
public:
  RenameNGroupCmd(const CharString group)
    : CommandM(group, "rename an element group"),
      newname("newname")
  {
    AddArgument(this, "newname", newname);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      NodeGroup *eg = groupname();
      if(!eg->rename(newname))
	garcon()->msout << ms_error << "There already is a group called "
			<< newname << "!" << endl << ms_normal;
    }
  }
  virtual CommandM *clone() const {
    RenameNGroupCmd *rngc = new RenameNGroupCmd(name);
    rngc->newname = newname;
    return rngc;
  }
};

static CommandFn deletegroup() {    // remove a group 
    if(gridinit()) {
	NodeGroup *n = groupname();
	if(n) delete n;
    }
}

static CommandFn addselectednodes() { // add the selected nodes to the group
  NodeGroup *ng = groupname();
  if(gridinit() && ng) {
    for(int i=0; i<grid->node.capacity(); i++) {
      Node *n = grid->node[i];
      if(n->isselected() && !ng->contains(n)) {
	ng->Add(n);
	n->x->setflag(ng->xflag);
	n->y->setflag(ng->yflag);
	n->reset_force();
	int do_x = ng->xflag.fixed() && n->x->fixed();
	int do_y =  ng->yflag.fixed() && n->y->fixed();
	n->reset_displacement(do_x, do_y);
	n->distort(ng->absolute_distortion, ng->absolute_shift, do_x, do_y);
	n->applyforce(ng->absolute_force);
#ifdef THERMAL
	n->phi->setflag(ng->phiflag);
	if(ng->phiflag.fixed()) {
	  n->reset_voltage();
	  n->v_distort(ng->absolute_thermaldistortion);
	}
	n->reset_surfcharge();
	n->apply_surfcharge(ng->absolute_thermaldistortion.heatrate);
#endif
//  	grid->distort_node(ng->absolute_distortion, ng->absolute_shift,
//  		      ng->absolute_force,
//  		      ng, n);
      }
    }
    grid->redraw();
    grid->fix_masters_needed();
  }
}

static CommandFn removeselectednodes() {
  NodeGroup *ng = groupname();
  if(gridinit() && ng) {
    for(int i=0; i<grid->node.capacity(); i++) {
      Node *n = grid->node[i];
      if(n->isselected() && ng->contains(n)) {
	ng->Remove(n);
      }
    }
    grid->redraw();
    grid->fix_masters_needed();
  }
}

static CommandFn selectnodegroup() {	// select all nodes in the group
  NodeGroup *ng = groupname();
  if(gridinit() && ng) {
    GroupIterator<Node> iter(*ng);
    Node *n;
    while(n = iter())
      n->select();
  }
  grid->redraw();
}

static CommandFn unselectnodegroup() {	// unselect all nodes in the group
  NodeGroup *ng = groupname();
  if(gridinit() && ng) {
    GroupIterator<Node> iter(*ng);
    Node *n;
    while(n = iter())
      n->unselect();
  }
  grid->redraw();
}

static CommandFn selectall() {
  if(gridinit()) {
    for(int i=0; i<grid->node.capacity(); i++)
      grid->node[i]->select();
    grid->redraw();
  }
}

static CommandFn unselectall() {
  if(gridinit()) {
    for(int i=0; i<grid->node.capacity(); i++)
      grid->node[i]->unselect();
    grid->redraw();
  }
}

static CommandFn toggleselect() {
  if(gridinit()) {
    for(int i=0; i<grid->node.capacity(); i++)
      grid->node[i]->toggleselect();
    grid->redraw();
  }
}

class SelectNodeNumberCmd : public CommandM {
private:
  int number;
  TrueFalse select;
public:
  SelectNodeNumberCmd()
    : CommandM("number", "select a numbered node"),
      number(0)
    {
      AddArgument(this, "n", number);
    }
  virtual CommandFn func() {
    if(gridinit()) {
      if(number < grid->node.capacity() && number >= 0) {
	grid->node[number]->select();
	grid->redraw();
      }
      else {
	garcon()->msout << ms_error << "There is no node number " << number
			<< "!" << endl << ms_normal;
      }
    }
  }
  virtual CommandM *clone () const {
    SelectNodeNumberCmd *snnc = new SelectNodeNumberCmd;
    snnc->number = number;
    return snnc;
  }
};

class SelectNodeNearestCmd : public CommandM {
private:
  double x;
  double y;
public:
  SelectNodeNearestCmd()
    : CommandM("nearest", "select a node by writing its coordinates"),
      x(0), y(0)
  {
    AddArgument(this, "x", x);
    AddArgument(this, "y", y);
  }
  virtual CommandFn func() {
    MeshCoord localcoord(x,y);
    Node *selected_node;
    if(gridinit()) {
      selected_node = grid->findNode(localcoord);
      selected_node->select();
      grid->redraw();
    }
  }
  virtual CommandM *clone () const {
    SelectNodeNearestCmd *snnc = new SelectNodeNearestCmd;
    snnc->x = x;
    snnc->y = y;
    return snnc;
  }
};

class SelectNodeRectangleCmd : public CommandM {
private:
  MeshCoord acorner, othercorner;
public:
  SelectNodeRectangleCmd()
    : CommandM("rectangle", "select a rectangle of nodes"),
      acorner(0,0), othercorner(1,1)
  {
    AddArgument(this, "x0", acorner.x);
    AddArgument(this, "y0", acorner.y);
    AddArgument(this, "x1", othercorner.x);
    AddArgument(this, "y1", othercorner.y);
  }
  virtual CommandFn func() {
    if(gridinit()) {
      grid->selectshape_nodes(Rectangle(acorner, othercorner));
      grid->redraw();
    }
  }
  virtual CommandM *clone () const {
    SelectNodeRectangleCmd *snnc = new SelectNodeRectangleCmd;
    snnc->acorner = acorner;
    snnc->othercorner = othercorner;
    return snnc;
  }
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

Menu *nodegroupmenu() {
    static Menu *groupmenu = 0;
    if(groupmenu) return groupmenu;
    groupmenu = new Menu("node groups");
    groupmenu->AddCommand(new NewNodeGroupCmd);
    groupmenu->AddSubMenu("delete",
	"delete a node group",
	new GroupMenu<Node>("delete", "delete group", deletegroup));
    groupmenu->AddSubMenu("rename", "rename a node group",
		  new GroupArgMenu<Node, RenameNGroupCmd>("rename group",
							  "rename group"));
    groupmenu->AddSubMenu("add_nodes",
	"add selected nodes to a node group",
	new GroupMenu<Node>("add_nodes", "add selected nodes to group",
	    addselectednodes));
    groupmenu->AddSubMenu("remove_nodes",
	"remove selected nodes from a node group",
	new GroupMenu<Node>("remove_nodes", "remove selected nodes from group",
	    removeselectednodes));
    groupmenu->AddSubMenu("select",
	"select all nodes in a group", selectnodemenu());
    Menu *unselectmenu = groupmenu->AddSubMenu("unselect",
	"unselect all nodes in a group",
	new GroupMenu<Node>("unselect", "unselect all nodes in group",
	    unselectnodegroup));
    unselectmenu->AddCommand("all", "unselect all nodes", unselectall);
    return groupmenu;
}

Menu *selectnodemenu() {
    static Menu *snm = 0;
    if(!snm) {
	snm = new GroupMenu<Node>("select nodes", "select all nodes in group",
	    selectnodegroup);
	snm->AddCommand("all", "select all nodes",  selectall);
	snm->AddCommand(new SelectNodeNumberCmd);
        snm->AddCommand(new SelectNodeNearestCmd);
        snm->AddCommand(new SelectNodeRectangleCmd);
	snm->AddCommand("toggle", "select unselected nodes and vice versa",
			toggleselect);
	snm->AddCommand("none", "unselect all nodes", unselectall);
    }
    return snm;
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

#ifdef DEBUG
void NodeGroup::dump(ostream &os) const {
  os << "**** NodeGroup " << name() << "   size=" << size() << endl;
  GroupIterator<Node> iter(*this);
  Node *n;
  while(n = iter()) {
    os << "-- " << *n << endl;
#ifdef THERMAL
    n->phi->dump(os,"---- ");
#endif // THERMAL
  }
  if(master_x) {
    os << "-- master_x: ";
    master_x->dump(os,"---- ");
  }
  if(master_y) {
    os << "-- master_y: ";
    master_y->dump(os,"---- ");
  }
#ifdef THERMAL
  if(master_phi) {
    os << "-- master_T: ";
    master_phi->dump(os,"---- ");
  }
#endif // THERMAL
}
#endif // DEBUG
