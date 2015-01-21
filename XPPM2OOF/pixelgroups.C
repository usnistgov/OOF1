// -*- C++ -*-
// $RCSfile: pixelgroups.C,v $
// $Revision: 1.12 $
// $Author: langer $
// $Date: 2000-11-01 16:41:51 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "adaptmesh.h"
#include "cell_coordinate.h"
#include "goof.h"
#include "meshcoord.h"
#include "pixelgroups.h"
#include "ppm2oof.h"
#include <math.h>

//template <class TYPE>			// do I need this?
//Vec<GroupMenu<Cell_coordinate>*> GroupMenu<Cell_coordinate>::menulist;

unsigned int PixelGroup::ngroups = 0;

PixelGroup::~PixelGroup() {
  // remove this group from lists stored in pixels
  for(int i=0; i<members.capacity(); i++)
    current_goof->pixelgrouplist[members[i]].Remove(this);
}

void PixelGroup::append(const Vec<Cell_coordinate> &list) {
  append_base(list);

  // add this group to the list of groups in each pixel, making sure
  // that the list remains sorted.
  if(name != "") {
    for(int i=0; i<list.capacity(); i++)
      current_goof->pixelgrouplist[list[i]].Insert(this, compare);
    if(current_goof->mesh())
      current_goof->mesh()->groups_need_recomputing();
    ++current_goof->groups_changed;
  }
}

void PixelGroup::append(const Cell_coordinate &pxl) {
  append_base(pxl);
  
  // add this group to the list of groups in the given pixel, making
  // sure that the list remains sorted.
  if(name != "") {
    current_goof->pixelgrouplist[pxl].Insert(this, compare);
    if(current_goof->mesh())
      current_goof->mesh()->groups_need_recomputing();
    ++current_goof->groups_changed;
  }
}

void PixelGroup::append(const Group<Cell_coordinate> &grp) {
  append(grp.membership());
}

void PixelGroup::clear() {
  // remove this group from lists stored in pixels
  for(int i=0; i<members.capacity(); i++)
    current_goof->pixelgrouplist[members[i]].Remove(this);

  clear_base();
  if(name != "") {
    if(current_goof->mesh())
      current_goof->mesh()->groups_need_recomputing();
    ++current_goof->groups_changed;
  }
}

void PixelGroup::remove(const Cell_coordinate &pxl) {
  remove_base(pxl);
  if(name != "") {
    current_goof->pixelgrouplist[pxl].Remove(this);
    if(current_goof->mesh())
      current_goof->mesh()->groups_need_recomputing();
    ++current_goof->groups_changed;
  }
}

void PixelGroup::remove(const Vec<Cell_coordinate> &pxls) {
  remove_base(pxls);
  if(name != "") {
    for(int i=0; i<pxls.capacity(); i++)
      current_goof->pixelgrouplist[pxls[i]].Remove(this);
    if(current_goof->mesh())
      current_goof->mesh()->groups_need_recomputing();
    ++current_goof->groups_changed;
  }
}

void PixelGroup::remove(const Group<Cell_coordinate> &grp) {
  remove(grp.membership());
}

void PixelGroup::trim(const Cell_coordinate &pxl) {
  Group<Cell_coordinate>::trim(pxl);
  if(name != "") {
    current_goof->pixelgrouplist[pxl].Remove(this);
    if(current_goof->mesh())
      current_goof->mesh()->groups_need_recomputing();
    ++current_goof->groups_changed;
  }
}

void PixelGroup::replace(const Cell_coordinate &oldpxl,
			 const Cell_coordinate &newpxl)
{
  Group<Cell_coordinate>::replace(oldpxl, newpxl);
  if(name != "") {
    current_goof->pixelgrouplist[oldpxl].Remove(this);
    current_goof->pixelgrouplist[newpxl].Insert(this, compare);
    if(current_goof->mesh())
      current_goof->mesh()->groups_need_recomputing();
    ++current_goof->groups_changed;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

static const Cell_coordinate xhat(1, 0);
static const Cell_coordinate yhat(0, 1);

double PixelGroup::perimeter() const {
  int nperim = 0;
  int width = current_goof->query_width();
  int height = current_goof->query_height();
  Array<bool> sel(height, width);
  sel.clear(0);
  for(int i=0; i<size(); i++)
    sel[(*this)[i]] = 1;
  for(int i=0; i<size(); i++) {
    Cell_coordinate here = (*this)[i];
    if((here.x == 0 || !sel[here-xhat]) ||
       (here.x == width-1  || !sel[here+xhat]) ||
       (here.y == 0 || !sel[here-yhat]) ||
       (here.y == height-1 || !sel[here+yhat]))
      nperim++;
  }
  return nperim/(sqrt(double(size()))*M_PI);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// function used to keep groups sorted in Goof::pixelgrouplists

int PixelGroup::compare(PixelGroup* const &a, PixelGroup* const &b)
{
  if(a->group_id < b->group_id) return -1;
  if(a->group_id > b->group_id) return 1;
  return 0;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

class NewPixelGroupCmd : public CommandM {
private:
  CharString name;
public:
  NewPixelGroupCmd() : CommandM("new", "create a new group"), 
    name("groupname")
  {
    AddArgument(this, "name", name);
  }
  virtual CommandFn func() {
    if(goofinit()) {
      char *c;
      int modified = 0;
      while(c = (char*) strchr(name, ' ')) { // cast required by Sun
	*c = '_';
	modified = 1;
      }
      if(modified)
	garcon()->msout << ms_info
			<< "Group name changed to \"" << name << "\""
			<< endl << ms_normal;
      if(name == "")
	garcon()->msout << ms_error << "Can't create a group with no name!"
			<< endl << ms_normal;
      else
	current_goof->add_group(name);
    }
  }
  virtual CommandM *clone() const {
    NewPixelGroupCmd *ngc = new NewPixelGroupCmd;
    ngc->name = name;
    return ngc;
  }
};

static PixelGroup *getgroup() {
    if(!current_goof) return 0;
    CharString whichgroup = garcon()->get_command();
    return current_goof->get_group(whichgroup);
}

// functions operating on groups

static CommandFn addpixels() {
  PixelGroup *g = getgroup();
  if(g) {
    current_goof->selected().weed();
    g->append(current_goof->selected());
    g->weed();
    garcon()->msout << ms_info << "Group " << g->query_name()
		    << " now contains " << g->size() << " pixels"
		    << endl << ms_normal;
  }
}

static CommandFn removepixels() {
  PixelGroup *g = getgroup();
  if(g) {
    current_goof->selected().weed();
    g->remove(current_goof->selected());
    garcon()->msout << ms_info << "Group " << g->query_name()
		    << " now contains " << g->size() << " pixels"
		    << endl << ms_normal;
  }
}

static CommandFn replacepixels() {
  PixelGroup *g = getgroup();
  if(g) {
    g->clear();
    current_goof->selected().weed();
    g->append(current_goof->selected());
    garcon()->msout << ms_info << "Group " << g->query_name()
		    << " now contains " << g->size() << " pixels"
		    << endl << ms_normal;
  }
}

static CommandFn removegroup() {
    PixelGroup *g = getgroup();
    if(g)
	current_goof->remove_group(g->query_name());
}

static CommandFn selectgroup() {
  PixelGroup *g = getgroup();
  if(g) {
    current_goof->init_selection(0);
    current_goof->select(*g);
    current_goof->redraw();
  }
}

static CommandFn selectgroup_too() {
  PixelGroup *g = getgroup();
  if(g) {
    current_goof->init_selection(1);
    current_goof->select(*g);
    current_goof->redraw();
  }
}

static CommandFn interselectgroup() {
  // select the intersection of the current selection and a group
  PixelGroup *g = getgroup();
  if(g) {
    // don't call init_selection, because it's done in Goof::select_intersection
    current_goof->select_intersection(*g);
    current_goof->redraw();
  }
}

static CommandFn unselectgroup() {
  // select the intersection of the complement of the current
  // selection and a group
  PixelGroup *g = getgroup();
  if(g) {
    // don't call init_selection, because it's done in Goof::select_intersection
    current_goof->select_intersection(*g, 1);
    current_goof->redraw();
  }
}

static CommandFn select_all_groups() {
  if(current_goof) {
    current_goof->init_selection(0);
    current_goof->select_all_groups();
    current_goof->redraw();
  }
}

static CommandFn select_all_groups_too() {
  if(current_goof) {
    current_goof->init_selection(1);
    current_goof->select_all_groups();
    current_goof->redraw();
  }
}

static CommandFn infogroup() {
  PixelGroup *g = getgroup();
  if(g) {
    g->weed();
    double total = current_goof->query_width()*current_goof->query_height();
    garcon()->msout << "Group " << g->query_name() << " contains "
		    << g->size() << " pixels ("
		    << g->size()/total*100 << "% of the image)" << endl;
  }
}

// ---------------------------------------------------- //

Menu *groupselectmenu() {
    static Menu *menu = 0;
    if(menu) return menu;
    menu = new GroupMenu<Cell_coordinate>("select group",
					  "select all pixels in group",
					  selectgroup);
    menu->AddCommand("all_groups", "select all groups", select_all_groups);
    return menu;
}

Menu *groupselectmenu_too() {
    static Menu *menu = 0;
    if(menu) return menu;
    menu = new GroupMenu<Cell_coordinate>("also select group",
					  "also select all pixels in group",
					  selectgroup_too);
    menu->AddCommand("all_groups", "select all groups", select_all_groups_too);
    return menu;
}

Menu *groupinterselectmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new GroupMenu<Cell_coordinate>("intersect",
			"select intersection of selected pixels with group",
					interselectgroup);
  return menu;
}

Menu *unselectgroupmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new GroupMenu<Cell_coordinate>("unselect",
					"unselect the pixels in group",
					unselectgroup);
  return menu;
}

static Menu *groupinfomenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new GroupMenu<Cell_coordinate>("group information",
					"print information about group",
					infogroup);
  return menu;
}

Menu *groupmenu() {
  static Menu *menu = 0;
  if(menu) return menu;
  menu = new Menu("group");
  menu->AddCommand(new NewPixelGroupCmd);
  menu->AddSubMenu("delete", "delete a group", 
		   new GroupMenu<Cell_coordinate>("delete groups", "delete group", removegroup));
  menu->AddSubMenu("add", "add selected pixels to a group", 
		   new GroupMenu<Cell_coordinate>("add pixels",
				 "add selected pixels to group", addpixels));
  menu->AddSubMenu("remove", "remove selected pixels from a group", 
		   new GroupMenu<Cell_coordinate>("remove pixels",
				  "remove selected pixels from group",
				  removepixels));
  menu->AddSubMenu("replace", "replace a group with the selected pixels", 
		   new GroupMenu<Cell_coordinate>("add pixels",
				 "use selected pixels to replace group",
				 replacepixels));

  // This duplicates stuff in the "select" menu, and it doesn't have
  // to be here as well, does it?

//   menu->AddSubMenu("select",
// 		   "select pixels in a group, unselecting all other pixels",
// 		   groupselectmenu()); 
//   menu->AddSubMenu("also_select",
// 		   "select pixels in a group, keeping already selected pixels",
// 		   groupselectmenu_too()); 
//   menu->AddSubMenu("intersect_group",
//    "select the intersection of a group with the previously selected pixels",
// 		   groupinterselectmenu());
//   menu->AddSubMenu("unselect_group", "unselect the pixels in a group",
// 		   unselectgroupmenu());

  menu->AddSubMenu("info", "print information about a group", groupinfomenu());
  return menu;
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

void PixelGroup::calcEigenValues(double &eig1, double &eig2) const{
  //calculates the  Eigenvalues for a selection
  
  double sumX=0, sumY=0 , sumXsquared=0, sumYsquared=0, sumXtimesY=0;
  int i, count=0;;
  //for every selected pixel that is in the burn
  for(i=0; i<size(); i++){
    Cell_coordinate here=(*this)[i];
    if(current_goof->active(here)){
      sumX+=here.x;
      sumY+=here.y;
      sumXsquared+=here.x*here.x;
      sumYsquared+=here.y*here.y;
      sumXtimesY+=here.x*here.y;
      count++;
    }
  }

  //get average coordinates
  double meanX=sumX/(double)count;
  double meanY=sumY/(double)count;
  
  //calculate matrix values
  double deltaXsquared=sumXsquared-meanX*meanX*count;
  double deltaYsquared=sumYsquared-meanY*meanY*count;
  double deltaXdeltaY=sumXtimesY-meanX*meanY*count;
  
  //solve the matrix equation using the quadratic formula 
  //to get the two Eigen values
  double a=1;
  double b=-(deltaXsquared+deltaYsquared);  //matrix -(a+d)
  double c=deltaXsquared*deltaYsquared-deltaXdeltaY*deltaXdeltaY;  //matrix ad-bc
  
  double q=0;
  if(b<0)
    q=-.5*(b-sqrt(b*b-4*a*c));
  else
    q=-.5*(b+sqrt(b*b-4*a*c));
  
  if(count!=0){
    eig1=q/a;
    eig2=c/q;
  }
  else{
    garcon()->msout << ms_error << "No selected pixels" << endl << ms_normal;
  }
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

double PixelGroup::aspectRatio() const{
  //returns the aspect ratio of a selection
  
  double eig1=0, eig2=1;
  calcEigenValues(eig1, eig2);
  
  return sqrt(eig1/eig2);  //0 is returned if no pixels are selected
}
