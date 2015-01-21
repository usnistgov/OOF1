// -*- C++ -*-
// $RCSfile: attributes.h,v $
// $Revision: 1.3 $
// $Author: langer $
// $Date: 2000-09-06 20:10:46 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "dashboard.h"

class AttributesDashboard : public Dashboard {
protected:
  AttributesDashboard(const CharString&, Drawer*, RubberBandType);
  virtual ~AttributesDashboard() {}
  FL_OBJECT *edges_button;
  FL_OBJECT *edgewidth_input;
  FL_OBJECT *elements_button;
  FL_OBJECT *empties_button;

  static TrueFalse edges_dflt, elements_dflt, empties_dflt;
  TrueFalse edges, elements, empties;
  static int edgewidth_dflt;
  int edgewidth;

  friend void FDCB_edges(FL_OBJECT*, long);
  void set_edges();
  void set_edges_gui();
  static void set_edges_cmd(const CharString&);

  friend void FDCB_edgewidth(FL_OBJECT*, long);
  void set_edgewidth();
  void set_edgewidth_gui();
  static void set_edgewidth_cmd(const CharString&);

  friend void FDCB_elements(FL_OBJECT*, long);
  void set_elements();
  void set_elements_gui();
  static void set_elements_cmd(const CharString&);

  friend void FDCB_empties(FL_OBJECT*, long);
  void set_empties();
  void set_empties_gui();
  static void set_empties_cmd(const CharString&);

  void set_simple_attributes();
  void free_simple_attributes();

  int showingempties() { return empties; }

  friend class MeshDrawer;
};

class SimpleAttributes : public AttributesDashboard {
private:
  FL_OBJECT *simple_attributes;
  virtual void show();
  virtual void hide();

  SimpleAttributes(Drawer*);
  virtual ~SimpleAttributes();

  friend class ColorDrawer;
  friend void create_form_simple_attributes(SimpleAttributes*);
};

class SelectAttributes : public AttributesDashboard {
private:
  SelectAttributes(Drawer*);
  virtual ~SelectAttributes();

  FL_OBJECT *select_attributes;
  FL_OBJECT *selecttext;
  FL_OBJECT *hot_elements_button;
  FL_OBJECT *hot_nodes_button;
  FL_OBJECT *noderadius_input;
  FL_OBJECT *select_elements_button;
  FL_OBJECT *select_nodes_button;
  FL_OBJECT *select_nobody_button;

  TrueFalse select_elements, select_nodes, hot_elements, hot_nodes;
  int noderadius;
  static TrueFalse select_elements_dflt, select_nodes_dflt,
    hot_elements_dflt, hot_nodes_dflt;
  static int noderadius_dflt;

  void set_hot_elements();
  void set_hot_elements_gui();
  friend void FDCB_hot_elements(FL_OBJECT *, long);
  static void set_hot_elements_cmd(const CharString&);

  void set_hot_nodes();
  void set_hot_nodes_gui();
  friend void FDCB_hot_nodes(FL_OBJECT *, long);
  static void set_hot_nodes_cmd(const CharString&);

  void set_noderadius();
  void set_noderadius_gui();
  friend void FDCB_noderadius(FL_OBJECT*, long);
  static void set_noderadius_cmd(const CharString&);

  void set_selection();
  friend void FDCB_set_selection(FL_OBJECT*, long);
  static CommandFn select_elements_cmd();
  void selectelements();
  static CommandFn select_nodes_cmd();
  void selectnodes();
  static CommandFn select_nobody_cmd();
  void select_nobody();

  void enable_selection();
  void disable_selection();

  virtual void clickhandler(const MouseClick&);
  virtual void draghandler(const MouseClick&, const MouseClick&);
  virtual void show();
  virtual void hide();

  friend void create_form_select_attributes(SelectAttributes*);
  friend class SelectDrawer;
};

#endif
