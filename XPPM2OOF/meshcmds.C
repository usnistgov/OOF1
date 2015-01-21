// -*- C++ -*-
// $RCSfile: meshcmds.C,v $
// $Revision: 1.23 $
// $Author: langer $
// $Date: 2004-10-26 02:17:17 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "adaptmesh.h"
#include "amtriangle.h"
#include "elector.h"
#include "fiddlemesh.h"
#include "filename.h"
#include "gfx.h"
#include "goof.h"
#include "imageform.h"
#include "material.h"
#include "menuDef.h"
#include "meshcmds.h"
#include "meshgroups.h"
#include "mouseclick.h"
#include "memusage.h"

static Menu *meshgroupmenu();
static CommandFn selectnonecmd();
static MeshGroup *getgroup();



int meshexists() {
  if(!current_goof) {
    garcon()->msout << ms_error << "No image!" << endl << ms_normal;
    return 0;
  }
  if(!current_goof->mesh()) {
    garcon()->msout << ms_error << "No mesh!" << endl << ms_normal;
    return 0;
  }
  return 1;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// these functions are called to indicate that the rules for
// transferring pixel properties to triangles have changed, so that if
// properties are being transferred automatically, they should be
// transferred the next time they're needed.

// should be static, but Sun doesn't like it...
void group_rulechange() {
  if(current_goof) {
    AdaptiveMesh *mesh = current_goof->mesh();
    if(mesh) {
      mesh->groups_need_recomputing();
      mesh->inherit_pixel_groups();
      current_goof->redraw();
    }
  }
}

static void groups_timechange(const CharString&) {
  if(current_goof) {
    AdaptiveMesh *mesh = current_goof->mesh();
    if(mesh)
      mesh->groups_timechange();
  }
}

void AdaptiveMesh::groups_timechange() {
  if(grouptransfertime == CONTINUALLY) {
    if(groups_rules_changed > groups_recompute_performed)
      ++groups_recompute_requested;
  }
}

void AdaptiveMesh::groups_need_recomputing() {
  // Called by other routines to indicate that groups are out of date.
  ++groups_rules_changed;
  if(grouptransfertime == CONTINUALLY)
    ++groups_recompute_requested;
}

// should be static, but Sun doesn't like it...
void material_rulechange() {
  if(current_goof) {
    AdaptiveMesh *mesh = current_goof->mesh();
    if(mesh) {
      mesh->materials_need_recomputing();
      mesh->inherit_pixel_materials();
      current_goof->redraw();
    }
  }
}

static void material_timechange(const CharString&) {
  if(current_goof) {
    AdaptiveMesh *mesh = current_goof->mesh();
    if(mesh)
      mesh->material_timechange();
  }
}

void AdaptiveMesh::material_timechange() {
  // If the transfertime has just been changed to CONTINUALLY, then if
  // the rules have changed since the last recomputation, a new
  // computation is required.
  if(materialtransfertime == CONTINUALLY) {
    if(material_rules_changed > material_recompute_performed)
      ++material_recompute_requested;
  }
}

void AdaptiveMesh::materials_need_recomputing() {
  // Called by other routines to indicate that materials are out of
  // date.
  ++material_rules_changed;
  if(materialtransfertime == CONTINUALLY) {
    ++material_recompute_requested;
  }
}

// when called from menus, these need a dummy argument
static void group_rulechange(const CharString&) {
  group_rulechange();
}

static void material_rulechange(const CharString&) {
  material_rulechange();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class CreateMeshCmd : public CommandM {
private:
  int nx, ny;
public:
  CreateMeshCmd()
    : CommandM("create", "create initial mesh"), nx(1), ny(1)
  {
    AddArgument(this, "nx", nx);
    AddArgument(this, "ny", ny);
  }
  virtual CommandFn func() {
    if(current_goof) {
      if(nx <= 0 || ny <= 0) {
	garcon()->msout << ms_error << "Mesh dimensions must be positive!"
			<< endl << ms_normal;
      }
      else {
	material_rulechange();
	group_rulechange();
	current_goof->new_mesh(nx, ny);
	current_goof->redraw();
      }
    }
  }
  virtual CommandM *clone() const {
    CreateMeshCmd *cmc = new CreateMeshCmd;
    cmc->nx = nx;
    cmc->ny = ny;
    return cmc;
  }
};

static CommandFn destroymesh() {
  if(meshexists()) {
    current_goof->destroy_mesh();
    current_goof->redraw();
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// mesh stack manipulations

static CommandFn undomesh() {
  if(meshexists()) {
    if(!current_goof->prev_mesh())
      garcon()->msout << ms_error << "No previous mesh!" << endl
		      << ms_normal;
    else
      current_goof->redraw();
  }
}

static CommandFn redomesh() {
  if(meshexists()) {
    if(!current_goof->next_mesh())
      garcon()->msout << ms_error << "No next mesh!" << endl << ms_normal;
    else
      current_goof->redraw();
  }
}

static CommandFn oldestmesh() {
  if(meshexists()) {
    current_goof->oldest_mesh();
    current_goof->redraw();
  }
}

static CommandFn newestmesh() {
  if(meshexists()) {
    current_goof->newest_mesh();
    current_goof->redraw();
  }
}

Menu *meshstackmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("mesh stack");
  menu->AddCommand("undo", "undo mesh modifications", undomesh);
  menu->AddCommand("redo", "redo previously undone mesh modification",
		   redomesh);
  menu->AddCommand("newest", "display most recent mesh", newestmesh);
  menu->AddCommand("oldest", "display oldest mesh", oldestmesh);
  AddVariable(menu, "stacksize", "number of meshes to retain",
	      Goof::meshstacksize, Goof::set_meshstacksize);
  return menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class MeshOutputCmd : public CommandM {
private:
  FileName filename;
public:
  MeshOutputCmd()
    : CommandM("save", "write an OOF file"),
      filename("out.goof")
    {
      AddArgument(this, "filename", filename);
    }
  virtual CommandFn func() {
    if(meshexists())
      current_goof->mesh()->writegoof(filename);
  }
  virtual CommandM *clone() const {
    MeshOutputCmd *moc = new MeshOutputCmd;
    moc->filename = filename;
    return moc;
  }
};

class OOF2OutputCmd : public CommandM {
private:
  FileName filename;
public:
  OOF2OutputCmd()
    : CommandM("save_oof2", "write an OOF2 file"),
      filename("oofout.py")
  {
    AddArgument(this, "filename", filename);
  }
  virtual CommandFn func() {
    if(meshexists()) {
      ofstream os(filename);
      if(!os) {
	garcon()->msout << ms_error << "Could not open file: "
			<< filename << "!" << endl << ms_normal;
	return;
      }
      current_goof->mesh()->write_oof2(os, filename);
    }
  }
  virtual CommandM *clone() const {
    OOF2OutputCmd *o2oc = new OOF2OutputCmd;
    o2oc->filename = filename;
    return o2oc;
  }
};

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static CommandFn findErange() {
  if(meshexists()) {
    double min, max;
    current_goof->mesh()->Erange(min, max);
    garcon()->msout << "min=" << min << "  max=" << max << endl;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Mesh Refinement Commands

class RefineCmd : public CommandM {
private:
  RefinementCondition * const condition;
  int iterations;
public:
  RefineCmd(RefinementCondition *rc)
    : condition(rc),
      iterations(1),
      CommandM(rc->name(), "refine by " + rc->name())
    {
      AddArgument(this, "iterations", iterations);
      condition->AddArgs(this);
    }
  virtual ~RefineCmd() { delete condition; }
  virtual CommandFn func() {
    if(meshexists()) {
      if(condition->setup()) {
	current_goof->dup_mesh();
#ifdef MEMUSAGE
	print_memusage("after duplicating mesh");
#endif
	current_goof->mesh()->refine(iterations, *condition);
#ifdef MEMUSAGE
	print_memusage("after refining mesh");
#endif
	current_goof->redraw();
      }
      else
	garcon()->msout << ms_error << "Cannot refine mesh!"
			<< endl << ms_normal;
    }
  }
  virtual CommandM *clone() const {
    RefineCmd *rc = new RefineCmd(condition->clone());
    rc->iterations = iterations;
    return rc;
  }
};


static CommandFn refinegroup() {
  if(getgroup()) {
    current_goof->dup_mesh();
    // Have to get the group again, since it lives in the mesh, and
    // we've duplicated the mesh!
    MeshGroup *g = getgroup();
    g->weed();
    current_goof->mesh()->refine_group(1, g);
    current_goof->redraw();
  }
}

template <>
VarObject *Var<Enum<TriangleDivisionMethod> >::create_varobject(Variable *var,
		Form *form, VarObjectType t, int x, int y, int w, int h)
{
  return new EnumVarObject<TriangleDivisionMethod>(var, form, t, x, y, w, h);
}

static Menu *refinemenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("refine");
  menu->AddCommand(new RefineCmd(new RefineAll));
  menu->AddCommand(new RefineCmd(new RefineE));
  menu->AddCommand(new RefineCmd(new RefineSelected));
  menu->AddCommand(new RefineCmd(new RefineDepth));
  menu->AddCommand(new RefineCmd(new RefineInterface));
  menu->AddCommand(new RefineCmd(new RefineDoubleInterface));
  menu->AddSubMenu("group", "refine a group of triangles",
		   new GroupMenu<AMTriangle*>("refine group", "refine group",
					      refinegroup));
  AddVariable(menu, "division", "how to divide triangles",
	      AMTriangle::divisionmethod);
  AddVariable(menu, "min_area", "don't divide triangles smaller than this",
	      AdaptiveMesh::min_area);
  AddVariable(menu, "max_divisions",\
 "limit on number of subdivisions per triangle per refinement iteration",
	      AdaptiveMesh::max_divisions);
  return menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Group management commands

static MeshGroup *getgroup() {
  if(!meshexists()) return 0;
  CharString whichgroup = garcon()->get_command();
  return current_goof->mesh()->get_group(whichgroup);
}

// add selected triangles to a group

static CommandFn addtriangles() {
  MeshGroup *g = getgroup();
  if(g) {
    AdaptiveMesh *mesh = current_goof->mesh();
    for(AMIterator i(mesh, AMI_SELECTED); !i.end(); ++i) {
      AMTriangle *tri = (*mesh)[i];
      g->append(tri);
      tri->inhibit_group_inheritance();
    }
    current_goof->redraw();	// in case groups are displayed in dashboard, eg
  }
}

// remove selected triangles from a group

static CommandFn subtracttriangles() {
  MeshGroup *g = getgroup();
  if(g) {
    AdaptiveMesh *mesh = current_goof->mesh();
    for(AMIterator i(mesh, AMI_SELECTED); !i.end(); ++i) {
      AMTriangle *tri = (*mesh)[i];
      g->remove(tri);
      tri->inhibit_group_inheritance();
    }
    current_goof->redraw();	// in case groups are displayed in dashboard, eg
  }
}

// select triangles in a group

static CommandFn selectgroup() {
  MeshGroup *g = getgroup();
  if(g) {
    current_goof->mesh()->select(*g);
    current_goof->redraw();
  }
}

static CommandFn selectgroup2() {
  MeshGroup *g = getgroup();
  if(g) {
    current_goof->mesh()->select_too(*g);
    current_goof->redraw();
  }
}

static Menu *selectmshgrpmnu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new GroupMenu<AMTriangle*>("select group",
				   "select only triangles in group",
				   selectgroup);
  menu->AddCommand("none", "unselect all triangles", selectnonecmd);
  return menu;
}

static Menu *selectmshgrpmnu2() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new GroupMenu<AMTriangle*>("select group too",
				   "also select all triangles in group",
				   selectgroup2);
  menu->AddCommand("none", "unselect all triangles", selectnonecmd);
  return menu;
}

// create a new group

class NewMeshGroupCmd : public CommandM {
private:
  CharString name;
public:
  NewMeshGroupCmd() : CommandM("new", "create a new group"), 
    name("groupname")
  {
    AddArgument(this, "name", name);
  }
  virtual CommandFn func() {
    if(meshexists()) {
      char *c;
      int modified = 0;
      while(c = (char*) strchr(name, ' ')) { // cast required by Sun
	*c = '_';
	modified = 1;
      }
      if(modified) garcon()->msout << ms_info << "Group name changed to \""
				   << name << "\"" << endl << ms_normal;
      current_goof->mesh()->add_group(name);
    }
  }
  virtual CommandM *clone() const {
    NewMeshGroupCmd *ngc = new NewMeshGroupCmd;
    ngc->name = name;
    return ngc;
  }
};

// get information on a group

static CommandFn infogroup() {
  MeshGroup *g = getgroup();
  if(g) {
    g->weed();
    double grouparea = 0.0;
    for(int i=0; i<g->size(); i++)
      grouparea += (*g)[i]->area();
    garcon()->msout << "Group " << g->query_name() << " contains " << g->size()
		    << " triangles, and "
		    << grouparea << " pixels  ("
		    << 100.*grouparea/
                      (current_goof->query_height()*current_goof->query_width())
		    << "% of total area)" << endl;
  }
}

// transfer groups from pixels to mesh

static CommandFn transfergroups() {
  if(meshexists()) {
    current_goof->mesh()->inherit_pixel_groups(1);
    current_goof->redraw();
  }
}

CommandFn removegrpcmd() {
  MeshGroup *g = getgroup();
  if(g) {
    current_goof->mesh()->remove_group(g, 1);
    material_rulechange();
    group_rulechange();
  }
}

CommandFn removeallgrps() {
  if(meshexists()) {
    current_goof->mesh()->remove_all_groups(1);
    material_rulechange();
    group_rulechange();
  }
}

static Menu *deletegrpmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new GroupMenu<AMTriangle*>("delete group", "delete group",
				    removegrpcmd);
  menu->AddCommand("all", "delete all groups", removeallgrps);
  return menu;
}

static Menu *meshgroupmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("groups");
  menu->AddCommand(new NewMeshGroupCmd);
  menu->AddSubMenu("select", "select a group of triangles", selectmshgrpmnu());
  menu->AddSubMenu("add", "add selected triangles to a group",
		   new GroupMenu<AMTriangle*>("add to group",
				   "add selected triangles to group",
					      addtriangles));
  menu->AddSubMenu("subtract", "remove selected triangles from a group",
		   new GroupMenu<AMTriangle*>("remove from group",
				   "remove selected triangles from group",
					      subtracttriangles));
  menu->AddSubMenu("delete", "delete a group", deletegrpmenu());
  menu->AddSubMenu("info", "print information about a group",
		   new GroupMenu<AMTriangle*>("group info",
					      "print information about group",
					      infogroup));

  Menu *electmenu = menu->AddSubMenu("election_rules",
			       "criteria for electing groups");
  AddVariable(electmenu, "matl_type", "elect by material type",
	      group_elector.mattype, group_rulechange);
  AddVariable(electmenu, "matl_type/params",
	      "elect by material type and parameters",
	      group_elector.mattypeparam, group_rulechange);
  AddVariable(electmenu, "group_membership", "elect by pixel group membership",
	      group_elector.groupmemb, group_rulechange);

  menu->AddCommand("transfer", "transfer groups from pixels to triangles",
		   transfergroups);
  AddVariable(menu, "transfer_method",
	      "how to transfer groups from pixels to triangles",
	      grouptransfermethod, group_rulechange);
  AddVariable(menu, "transfer_time",
	      "when to transfer groups from pixels to triangles",
	      grouptransfertime, groups_timechange);
  AddVariable(menu, "transfer_replace",
	      "replace existing groups or add to them?",
	      grouptransferreplace);
  return menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// transfer materials from pixels to mesh

static CommandFn transfermaterials() {
  if(meshexists()) {
    current_goof->dup_mesh();
    current_goof->mesh()->inherit_pixel_materials(1);
    current_goof->redraw();
  }
}

Menu *meshmaterialmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("materials");

  Menu *setmatmenu = menu->AddSubMenu("set",
		      "explicitly set material type for selected triangles");
  Vec<MaterialTypeRegistration*> &registry = material_registry();
  MaterialTypeRegistration::sort();
  for(int i=0; i<registry.capacity(); i++)
    setmatmenu->AddCommand(registry[i]->tricommand);

  AddVariable(menu, "transfer_method",
	      "how to transfer groups from pixels to triangles",
	      materialtransfermethod, material_rulechange);
  AddVariable(menu, "transfer_time",
	      "when to transfer groups from pixels to triangles",
	      materialtransfertime, material_timechange);
  menu->AddCommand("transfer", "transfer materials from pixels to triangles",
		   transfermaterials);

  Menu *electmenu = menu->AddSubMenu("election_rules",
			       "criteria for electing materials");
  AddVariable(electmenu, "group_membership", "elect by pixel group membership",
	      material_elector.groupmemb, material_rulechange);
  return menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// Triangle selection commands

static CommandFn selectallcmd() {
  if(meshexists()) {
    current_goof->mesh()->select_all();
    current_goof->redraw();
  }
}


static CommandFn selectnonecmd() {
  if(meshexists()) {
    current_goof->mesh()->unselect_all();
    current_goof->redraw();
  }
}

static CommandFn selectdimples() {
  if(meshexists()) {
    int n = current_goof->mesh()->select_dimples();
    garcon()->msout << ms_info << "Selected " << n << " additional "
		    << (n==1? "triangle." : "triangles.")
		    << endl << ms_normal;
    current_goof->redraw();
  }
}

static CommandFn selectthumbs() {
  if(meshexists()) {
    int n = current_goof->mesh()->unselect_sorethumbs();
    garcon()->msout << ms_info << "Unselected " << n
		    << (n==1? " triangle." :" triangles.")
		    << endl << ms_normal;
    current_goof->redraw();
  }
}

static CommandFn select_interface() {
  if(meshexists()) {
    int n = current_goof->mesh()->select_interface();
    garcon()->msout << ms_info << "Selected " << n
		    << (n==1?" triangle." : " triangles.")
		    << endl << ms_normal;
    current_goof->redraw();
  }
}

static CommandFn select_neighbors() {
  // select neighbors of selected triangles. Mostly for debugging.
  if(meshexists()) {
    int n = current_goof->mesh()->select_neighbors();
    garcon()->msout << ms_info << "Selected " << n
		    << (n==1? " triangle." : " triangles.")
		    << endl << ms_normal;
    current_goof->redraw();
  }
}

class SelectECmd : public CommandM {
private:
  double min, max;
public:
  SelectECmd()
    : min(0), max(1),
      CommandM("E", "select triangles with a range of E values")
    {
      AddArgument(this, "min", min);
      AddArgument(this, "max", max);
    }
  virtual CommandFn func() {
    if(meshexists()) {
      int n = current_goof->mesh()->selectE(min, max);
      garcon()->msout << ms_info << "Selected " << n
		      << (n==1? " triangle." : " triangles.")
		      << endl << ms_normal;
      current_goof->redraw();
    }
  }
  virtual CommandM *clone() const {
    SelectECmd *sec = new SelectECmd;
    sec->min = min;
    sec->max = max;
    return sec;
  }
};

static Menu *selectmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("select");
  menu->AddCommand("all", "select all triangles", selectallcmd);
  menu->AddCommand("none", "select no triangles", selectnonecmd);
  menu->AddCommand(new SelectECmd);
  menu->AddCommand("dimples", "select triangles with 2 or 3 selected neighbors",
		   selectdimples);
  menu->AddCommand("thumbs",
		   "unselect triangles with fewer than 2 selected neighbors",
		   selectthumbs);
  menu->AddCommand("interface",
	       "select all triangles with neighbors of a different material",
		   select_interface);
  menu->AddCommand("neighbors",
		   "select all neighbors of currently selected triangles",
		   select_neighbors);
  menu->AddSubMenu("group", "select mesh groups", selectmshgrpmnu());
  menu->AddSubMenu("group_too", "select additional mesh groups",
		   selectmshgrpmnu2());
  return menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static CommandFn sanity() {
  if(meshexists())
    current_goof->mesh()->sanity();
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

Menu *meshmenu() {
  static Menu *menu = 0;
  if(menu) return menu;

  Enum<GroupTransferMethod>::name(GTM_ELECTION, "election");
  Enum<GroupTransferMethod>::name(GTM_ALLPIXELS, "all");
  Enum<GroupTransferMethod>::name(GTM_CENTERPIXEL, "center");
  Enum<MaterialTransferMethod>::name(MTM_ELECTION, "election");
  Enum<MaterialTransferMethod>::name(MTM_CENTERPIXEL, "center");
  Enum<TransferTime>::name(CONTINUALLY, "automatic");
  Enum<TransferTime>::name(EXPLICITLY, "on_demand");

  menu = new Menu("adaptive mesh");
  menu->AddCommand(new CreateMeshCmd);
  menu->AddCommand("destroy", "destroy the mesh", destroymesh);
  menu->AddCommand(new FiddleCmd);
  menu->AddCommand(new FiddleDownHillCmd);
  menu->AddCommand(new FiddleLaplaceCmd);
  menu->AddCommand("swap_two", "swap two triangles", swapedges);
  menu->AddCommand("swap_worst", "make swaps that lower energy", swapworst);
  menu->AddCommand("E_range", "find range of E values", findErange);
  menu->AddCommand(new MeshOutputCmd);
  menu->AddCommand(new OOF2OutputCmd);
  menu->AddSubMenu("groups", "manage triangle groups", meshgroupmenu());
  menu->AddSubMenu("materials", "transfer materials from pixels to mesh",
		   meshmaterialmenu());
  menu->AddSubMenu("refine", "refine the mesh", refinemenu());
  menu->AddSubMenu("select", "select triangles", selectmenu());
  menu->AddSubMenu("stack", "manipulate stack of old meshes", meshstackmenu());
  AddVariable(menu, "alpha_E", "balance between homogeneity and shape",
	      AdaptiveMesh::alpha);
  AddVariable(menu, "redraw", "redraw mesh continuously?",
	      AdaptiveMesh::continuous_redraw);
  menu->AddCommand("sanity_check", "check mesh", sanity);
  return menu;
}

template <>
VarObject *Var<Enum<GroupTransferMethod> >::create_varobject(Variable *var,
	Form *form, VarObjectType t, int x, int y, int w, int h)
{
  return new EnumVarObject<GroupTransferMethod>(var, form, t, x, y, w, h);
}

template <>
VarObject *Var<Enum<MaterialTransferMethod> >::create_varobject(Variable *var,
	Form *form, VarObjectType t, int x, int y, int w, int h)
{
  return new EnumVarObject<MaterialTransferMethod>(var, form, t, x, y, w, h);
}

template <>
VarObject *Var<Enum<TransferTime> >::create_varobject(Variable *var,
	Form *form, VarObjectType t, int x, int y, int w, int h)
{
  return new EnumVarObject<TransferTime>(var, form, t, x, y, w, h);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void select_mesh_triangle(const MeshCoord &where, int shiftkey) {
  if(meshexists()) {
    if(!shiftkey)
      current_goof->mesh()->unselect_all();
    AMTriangle *clicked = current_goof->mesh()
      ->smallest_triangle_containing(where);
    if(clicked && clicked->active())
      clicked->toggle_select();
  }
}


void select_mesh_triangles(const MeshCoord &c1, const MeshCoord &c2,
			   int shiftkey)
{
  if(meshexists()) {
    double xmin, xmax, ymin, ymax;
    if(c1.x < c2.x) {
      xmin = c1.x;
      xmax = c2.x;
    }
    else {
      xmin = c2.x;
      xmax = c1.x;
    }
    if(c1.y < c2.y) {
      ymin = c1.y;
      ymax = c2.y;
    }
    else {
      ymin = c2.y;
      ymax = c1.y;
    }

    AdaptiveMesh *mesh = current_goof->mesh();
    if(!shiftkey)
      mesh->unselect_all();

    for(AMIterator i(mesh, AMI_ALL); !i.end(); ++i) {
      AMTriangle *tri = (*mesh)[i];
      if(tri->active()) {
	MeshCoord ctr(tri->center());
	if(ctr.x >= xmin && ctr.x <= xmax && ctr.y >= ymin && ctr.y <= ymax) {
	  if(shiftkey)
	    tri->toggle_select();
	  else
	    tri->select();
	}
      }
    }
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// include template definitions

#include "enumio.C"
