// -*- C++ -*-
// $RCSfile: attributes.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-31 19:15:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#include "attributes.h"
#include "colorutils.h"
#include "create_form_drawer.h"
#include "delete_fl_group.h"
#include "formdrawer.h"
#include "graphics.h"
#include "grid.h"
#include "griditerator.h"
#include "mouseclick.h"
#include "node.h"
#include "parameters.h"

TrueFalse AttributesDashboard::edges_dflt = 0;
int AttributesDashboard::edgewidth_dflt = 0;
TrueFalse AttributesDashboard::elements_dflt = 1;
TrueFalse AttributesDashboard::empties_dflt = 0;

AttributesDashboard::AttributesDashboard(const CharString &nm, Drawer *drwr,
					 RubberBandType rb) 
  : Dashboard(nm, drwr, rb),
    edges(edges_dflt),
    edgewidth(edgewidth_dflt),
    elements(elements_dflt),
    empties(empties_dflt)
{
  AddVariable(&menu, "edges", "draw edges?", edges, set_edges_cmd);
  AddVariable(&menu, "edgewidth", "edge width", edgewidth, set_edgewidth_cmd);
  AddVariable(&menu, "elements", "draw elements?", elements, set_elements_cmd);
  AddVariable(&menu, "empties", "draw empties?", empties, set_empties_cmd);
}

void AttributesDashboard::set_simple_attributes() {
  fl_set_button(edges_button, edges);
  fl_set_input(edgewidth_input, to_charstring(edgewidth));
  fl_set_button(empties_button, empties);
  fl_set_button(elements_button, elements);
}

void AttributesDashboard::free_simple_attributes() {
  /*
  fl_free_object(edges_button);
  fl_free_object(edgewidth_input);
  fl_free_object(elements_button);
  fl_free_object(empties_button);
  */
  menu.RemoveOption("edges");
  menu.RemoveOption("edgewidth");
  menu.RemoveOption("elements");
  menu.RemoveOption("empties");
}

// --------------------------

void AttributesDashboard::set_edges_gui() {  // called by FDCB_edges()
  if(active()) {
    edges = fl_get_button(edges_button);
    garcon()->log(menucommand() + "edges=" + to_charstring(edges));
    set_edges();
  }
}

void AttributesDashboard::set_edges() {  // called by FDCB_edges()
  edges_dflt = edges;
  if(edges)
    set_edgewidth(); // draws
  else
    formdrawer()->draw();
  fl_set_button(edges_button, edges);
}

void FDCB_edges(FL_OBJECT*, long ptr) { // called by XForms
  ((AttributesDashboard*) ptr)->set_edges_gui();
}

void AttributesDashboard::set_edges_cmd(const CharString &) { // called by Menu
  ((AttributesDashboard*) get_dashboard())->set_edges();
}

// ---------------------------

void AttributesDashboard::set_edgewidth_gui() {
  if(active()) {
    edgewidth = atoi(fl_get_input(edgewidth_input));
    garcon()->log(menucommand() + "edgewidth=" + to_charstring(edgewidth));
    set_edgewidth();
  }
}

void AttributesDashboard::set_edgewidth() {
  if(edgewidth >= 0)
    edgewidth_dflt = edgewidth;
  else
    edgewidth = edgewidth_dflt;
  if(edges)
    formdrawer()->draw();

  fl_set_input(edgewidth_input, to_charstring(edgewidth));
}

void FDCB_edgewidth(FL_OBJECT*, long ptr) {
  ((AttributesDashboard*) ptr)->set_edgewidth_gui();
}

void AttributesDashboard::set_edgewidth_cmd(const CharString&) {
  ((AttributesDashboard*) get_dashboard())->set_edgewidth();
}

// ---------------------------

void AttributesDashboard::set_elements_gui() { 
  if(active()) {
    elements = fl_get_button(elements_button);
    garcon()->log(menucommand() + "elements=" + to_charstring(elements));
    set_elements();
  }
}

void AttributesDashboard::set_elements() {
  elements_dflt = elements;
  formdrawer()->draw();
  fl_set_button(elements_button, elements);
}

void FDCB_elements(FL_OBJECT*, long ptr) {
  ((AttributesDashboard*) ptr)->set_elements_gui();
}

void AttributesDashboard::set_elements_cmd(const CharString&) {
  ((AttributesDashboard*) get_dashboard())->set_elements();
}

// ---------------------------

void AttributesDashboard::set_empties_gui() {
  if(active()) {
    empties = fl_get_button(empties_button);
    set_empties();
    garcon()->log(menucommand() + "empties=" + to_charstring(empties));
  }
}

void AttributesDashboard::set_empties() {
  empties_dflt = empties;
  formdrawer()->draw();
  fl_set_button(empties_button, empties);
}

void FDCB_empties(FL_OBJECT*, long ptr) {
  ((AttributesDashboard*) ptr)->set_empties_gui();
}

void AttributesDashboard::set_empties_cmd(const CharString&) {
  ((AttributesDashboard*) get_dashboard())->set_empties();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

SimpleAttributes::SimpleAttributes(Drawer *drwr)
  : AttributesDashboard("Attributes", drwr, RB_NONE)
{
  create_form_simple_attributes(this);
  set_simple_attributes();
}

SimpleAttributes::~SimpleAttributes() {
  hide();
  delete_fl_group(simple_attributes);
  /*
  fl_delete_object(simple_attributes);
  fl_free_object(simple_attributes);
  */
  free_simple_attributes();
}

void SimpleAttributes::show() {
  fl_show_object(simple_attributes);
}

void SimpleAttributes::hide() {
  fl_hide_object(simple_attributes);
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

TrueFalse SelectAttributes::select_elements_dflt(0);
TrueFalse SelectAttributes::select_nodes_dflt(0);
TrueFalse SelectAttributes::hot_elements_dflt(0);
TrueFalse SelectAttributes::hot_nodes_dflt(0);
int SelectAttributes::noderadius_dflt(3);

SelectAttributes::SelectAttributes(Drawer *drwr)
  : AttributesDashboard("Attributes", drwr, RB_RECTANGLE),
    hot_elements(hot_elements_dflt),
    hot_nodes(hot_nodes_dflt),
    noderadius(noderadius_dflt),
    select_elements(select_elements_dflt),
    select_nodes(select_nodes_dflt)
{
  create_form_select_attributes(this);
  set_simple_attributes();
  fl_freeze_form(formdrawer()->the_form);
  fl_set_button(hot_elements_button, hot_elements);
  fl_set_button(hot_nodes_button, hot_nodes);
  fl_set_input(noderadius_input, to_charstring(noderadius));
  fl_set_button(select_elements_button, select_elements);
  fl_set_button(select_nodes_button, select_nodes);
  fl_set_button(select_nobody_button, !(select_elements || select_nodes));
  fl_unfreeze_form(formdrawer()->the_form);

  AddVariable(&menu, "hot_elements", "hot elements?", hot_elements,
	      set_hot_elements_cmd);
  AddVariable(&menu, "hot_nodes", "hot nodes?", hot_nodes,
	      set_hot_nodes_cmd);
  AddVariable(&menu, "noderadius", "node radius", noderadius,
	      set_noderadius_cmd);
  menu.AddCommand("select_elements", "select elements", select_elements_cmd);
  menu.AddCommand("select_nodes", "select nodes", select_nodes_cmd);
  menu.AddCommand("select_nobody", "select nobody", select_nobody_cmd);
}

SelectAttributes::~SelectAttributes() {
  hide();
  delete_fl_group(select_attributes);
  free_simple_attributes();
  /*
  fl_delete_object(select_attributes);

  fl_free_object(selecttext);
  fl_free_object(hot_elements_button);
  fl_free_object(hot_nodes_button);
  fl_free_object(noderadius_input);
  fl_free_object(select_elements_button);
  fl_free_object(select_nodes_button);
  fl_free_object(select_nobody_button);

  fl_free_object(select_attributes);
  */
  menu.RemoveOption("select_elements");
  menu.RemoveOption("select_nodes");
  menu.RemoveOption("select_nobody");
  menu.RemoveOption("hot_elements");
  menu.RemoveOption("hot_nodes");
  menu.RemoveOption("noderadius");
}

void SelectAttributes::show() {
  fl_show_object(select_attributes);
  if(select_elements || select_nodes) {
    setcursor(*selectcursor);
    formdrawer()->drag_rubberband();
  }
}

void SelectAttributes::hide() {
  fl_hide_object(select_attributes);
  unsetcursor();
  formdrawer()->drag_nothing();
}

// -----------------------

void SelectAttributes::set_hot_elements_gui() {
  if(active()) {
    hot_elements = fl_get_button(hot_elements_button);
    garcon()->log(menucommand() + "hot_elements=" +
		  to_charstring(hot_elements));
    set_hot_elements();
  }
}

void SelectAttributes::set_hot_elements() {
  hot_elements_dflt = hot_elements;
  if(!hot_elements && select_elements) {
    select_elements = select_elements_dflt = TF_FALSE;
    fl_set_button(select_nobody_button, 1);
    fl_set_button(select_elements_button, 0);
    disable_selection();
  }
  fl_set_button(hot_elements_button, hot_elements);
  formdrawer()->draw();
}

void FDCB_hot_elements(FL_OBJECT*, long ptr) {
  ((SelectAttributes*) ptr)->set_hot_elements_gui();
}

void SelectAttributes::set_hot_elements_cmd(const CharString&) {
  ((SelectAttributes*) get_dashboard())->set_hot_elements();
}

// -----------------------

void SelectAttributes::set_hot_nodes_gui() {
  if(active()) {
    hot_nodes = fl_get_button(hot_nodes_button);
    garcon()->log(menucommand() + "hot_nodes=" + to_charstring(hot_nodes));
    set_hot_nodes();
  }
}

void SelectAttributes::set_hot_nodes() {
  hot_nodes_dflt = hot_nodes;
  if(!hot_nodes && select_nodes) {
    select_nodes = select_nodes_dflt = TF_FALSE;
    fl_set_button(select_nobody_button, 1);
    fl_set_button(select_nodes_button, 0);
    disable_selection();
  }
  fl_set_button(hot_nodes_button, hot_nodes);
  formdrawer()->draw();
}

void FDCB_hot_nodes(FL_OBJECT*, long ptr) {
  ((SelectAttributes*) ptr)->set_hot_nodes_gui();
}

void SelectAttributes::set_hot_nodes_cmd(const CharString&) {
  ((SelectAttributes*) get_dashboard())->set_hot_nodes();
}

// -----------------------

void SelectAttributes::set_noderadius_gui() {
  if(active()) {
    noderadius = atoi(fl_get_input(noderadius_input));
    garcon()->log(menucommand() + "noderadius=" + to_charstring(noderadius));
    set_noderadius();
  }
}

void SelectAttributes::set_noderadius() {
  if(noderadius < 0)
    noderadius = noderadius_dflt;
  else
    noderadius_dflt = noderadius;
  fl_set_input(noderadius_input, to_charstring(noderadius));
  if(hot_nodes)
    formdrawer()->draw();
}

void FDCB_noderadius(FL_OBJECT*, long ptr) {
  ((SelectAttributes*) ptr)->set_noderadius_gui();
}

void SelectAttributes::set_noderadius_cmd(const CharString&) {
  ((SelectAttributes*) get_dashboard())->set_noderadius();
}

// ----------------------

void SelectAttributes::set_selection() {
  if(fl_get_button(select_elements_button))
    selectelements();
  else if(fl_get_button(select_nodes_button))
    selectnodes();
  else
    select_nobody();
}

void FDCB_set_selection(FL_OBJECT*, long ptr) {
  ((SelectAttributes*) ptr)->set_selection();
}

CommandFn SelectAttributes::select_elements_cmd() {
  ((SelectAttributes*) get_dashboard())->selectelements();
}

void SelectAttributes::selectelements() {
  if(active()) {
    select_elements = TF_TRUE;
    select_nodes = TF_FALSE;
    garcon()->log(menucommand() + "select_elements");
    enable_selection();
  }
}

CommandFn SelectAttributes::select_nodes_cmd() {
  ((SelectAttributes*) get_dashboard())->selectnodes();
}

void SelectAttributes::selectnodes() {
  if(active()) {
    select_elements = TF_FALSE;
    select_nodes = TF_TRUE;
    garcon()->log(menucommand() + "select_nodes");
    enable_selection();
  }
}

CommandFn SelectAttributes::select_nobody_cmd() {
  ((SelectAttributes*) get_dashboard())->select_nobody();
}

void SelectAttributes::select_nobody() {
  if(active()) {
    select_elements = select_elements_dflt = TF_FALSE;
    select_nodes = select_nodes_dflt = TF_FALSE;
    garcon()->log(menucommand() + "select_nobody");
    disable_selection();
  }
}

void SelectAttributes::enable_selection() {
  if(select_elements) {
    if(!hot_elements) {
      hot_elements = hot_elements_dflt = TF_TRUE;
      fl_set_button(hot_elements_button, 1);
      formdrawer()->draw();
    }
    fl_set_button(select_elements_button, 1);
    fl_set_button(select_nodes_button, 0);
  }
  else if(select_nodes) {
    if(!hot_nodes) {
      hot_nodes = hot_nodes_dflt = TF_TRUE;
      fl_set_button(hot_nodes_button, 1);
      formdrawer()->draw();
    }
    fl_set_button(select_nodes_button, 1);
    fl_set_button(select_elements_button, 0);
  }
  select_elements_dflt = select_elements;
  select_nodes_dflt = select_nodes;
  fl_set_button(select_nobody_button, 0);
  setcursor(*selectcursor);
  formdrawer()->drag_rubberband();
}

void SelectAttributes::disable_selection() {
  fl_set_button(select_nodes_button, 0);
  fl_set_button(select_elements_button, 0);
  fl_set_button(select_nobody_button, 1);
  unsetcursor();
  formdrawer()->drag_nothing();
}

// --------------------- 

void SelectAttributes::clickhandler(const MouseClick &click) {
  if(!grid) return;
  if(select_elements) {
    Element *e = grid->findElement(click.coord);
    if(e && (!e->isempty() || drawer->showingempties())) {
      if(click.shiftkey) {
	e->toggleselect();
	FormDrawer::ElementUpdated(grid, e);
      }
      else {
	grid->unselectallelements();
	e->select();
	grid->redraw();
      }
    }
  }
  if(select_nodes) {
    Node *n = grid->findNode(click.coord);
    if(click.shiftkey) {
      if(n) {
	n->toggleselect();
	FormDrawer::NodeUpdated(grid, n);
	// if node is now unselected, have to erase the circle drawn at the node
      }
    }
    else {
      grid->unselectallnodes();
      if(n)
	n->select();
      grid->redraw();
    }
  } 
}

void SelectAttributes::draghandler(const MouseClick &mousedn,
				    const MouseClick &mouseup)
{
  if(!grid) return;
  Rectangle rect(mousedn.coord, mouseup.coord);
  int count = 0;
  if(select_elements) {
    ElementIterator iter(drawer->showingempties()?EI_ALL:EI_NONEMPTY, grid);
    Element *e;
    if(mousedn.shiftkey) { // toggle selection of all elements dragged over
      while(e = iter()) {
	if(rect.contains(e->center())) {
	  e->toggleselect();
	  count++;
	}
      }
    }
    else {  // no shift -- select only those elements dragged over
      grid->unselectallelements();
      while(e = iter()) {
 	if(rect.contains(e->center())) {
	  e->select();
	  count++;
	}
      }
    }
  }
  if(select_nodes) {
    if(mousedn.shiftkey) {
      for(int i=0; i<grid->node.capacity(); i++) {
	Node *n = grid->node[i];
	if(rect.contains(n->position(grid->enhancement()))) {
	  n->toggleselect();
	  count++;
	}
      }
    }
    else {
      grid->unselectallnodes();
      for(int i=0; i<grid->node.capacity(); i++) {
	Node *n =grid->node[i];
	if(rect.contains(n->position(grid->enhancement()))) {
	  n->select();
	  count++;
	}
      }
    }
  }
  if(count > 0)
    grid->redraw();
}
