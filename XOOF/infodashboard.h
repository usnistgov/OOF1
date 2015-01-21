// -*- C++ -*-
// $RCSfile: infodashboard.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-10-13 20:02:50 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef INFODASHBOARD_H
#define INFODASHBOARD_H

#include "dashboard.h"
class MouseClick;
class StrainStress;

class InfoDashboard : public Dashboard {
protected:
  FL_OBJECT *info_mouseposition;
  static TrueFalse infoflag_mouseposition_dflt;
  TrueFalse infoflag_mouseposition;

  void set_info_mouseposition();
  friend void FDCB_info_mouseposition(FL_OBJECT *, long);

 // set buttons in form according to current flags.
  virtual void set_infobuttons() const = 0;
  static void set_infobuttons_cmd(const CharString&);

  InfoDashboard(const CharString&, Drawer*);
  virtual ~InfoDashboard();
};

class ElementInfo : public InfoDashboard {
private:
  FL_OBJECT *elementinfo;
  FL_OBJECT *infoframe1;
  FL_OBJECT *elementinfotext;
  FL_OBJECT *info_elementtype;
  FL_OBJECT *info_nodeindices;
  FL_OBJECT *info_elementindex;
  FL_OBJECT *info_intrinsicgray;
  FL_OBJECT *info_elementparams;
  FL_OBJECT *info_stress;
  FL_OBJECT *info_strain;
  FL_OBJECT *info_components;
  FL_OBJECT *info_invariants;
  FL_OBJECT *info_eigenvalues;
  FL_OBJECT *info_energydensity;
  FL_OBJECT *info_heatflux;
  FL_OBJECT *info_tempgradient;
  FL_OBJECT *info_temperature;

  TrueFalse infoflag_elementtype;
  TrueFalse infoflag_nodeindices;
  TrueFalse infoflag_elementindex;
  TrueFalse infoflag_intrinsicgray;
  TrueFalse infoflag_elementparams;
  TrueFalse infoflag_stress;
  TrueFalse infoflag_strain;
  TrueFalse infoflag_components;
  TrueFalse infoflag_invariants;
  TrueFalse infoflag_eigenvalues;      
  TrueFalse infoflag_energydensity;
  TrueFalse infoflag_heatflux;
  TrueFalse infoflag_tempgradient;
  TrueFalse infoflag_temperature;

  static TrueFalse infoflag_elementtype_dflt;
  static TrueFalse infoflag_nodeindices_dflt;
  static TrueFalse infoflag_elementindex_dflt;
  static TrueFalse infoflag_intrinsicgray_dflt;
  static TrueFalse infoflag_elementparams_dflt;
  static TrueFalse infoflag_stress_dflt;
  static TrueFalse infoflag_strain_dflt;
  static TrueFalse infoflag_components_dflt;
  static TrueFalse infoflag_invariants_dflt;
  static TrueFalse infoflag_eigenvalues_dflt;    
  static TrueFalse infoflag_energydensity_dflt;
  static TrueFalse infoflag_heatflux_dflt;
  static TrueFalse infoflag_tempgradient_dflt;
  static TrueFalse infoflag_temperature_dflt;
 

  friend void FDCB_info_elementtype(FL_OBJECT *, long);
  friend void FDCB_info_nodeindices(FL_OBJECT *, long);
  friend void FDCB_info_elementindex(FL_OBJECT *, long);
  friend void FDCB_info_intrinsicgray(FL_OBJECT *, long);
  friend void FDCB_info_elementparams(FL_OBJECT *, long);
  friend void FDCB_info_stress(FL_OBJECT *, long);
  friend void FDCB_info_strain(FL_OBJECT *, long);
  friend void FDCB_info_components(FL_OBJECT *, long);
  friend void FDCB_info_invariants(FL_OBJECT *, long);
  friend void FDCB_info_eigenvalues(FL_OBJECT *, long);
  friend void FDCB_info_energydensity(FL_OBJECT*, long);
  friend void FDCB_info_heatflux(FL_OBJECT *, long);
  friend void FDCB_info_tempgradient(FL_OBJECT *, long);
  friend void FDCB_info_temperature(FL_OBJECT *, long);


  void set_info_elementtype();
  void set_info_nodeindices();
  void set_info_elementindex();
  void set_info_intrinsicgray();
  void set_info_elementparams();
  void set_info_stress();
  void set_info_strain();
  void set_info_components();
  void set_info_invariants();
  void set_info_eigenvalues();
  void set_info_energydensity();
  void set_info_heatflux();
  void set_info_tempgradient();
  void set_info_temperature();

  virtual void set_infobuttons() const;

  virtual void show();
  virtual void hide();

  ElementInfo(Drawer*);
  virtual ~ElementInfo();

  virtual void clickhandler(const MouseClick&);

  void infostressstrain(const StrainStress&) const;
  
  friend class MeshDrawer;
  friend void create_form_elementinfo(ElementInfo*);
};

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

class NodeInfo : public InfoDashboard {
private:
  FL_OBJECT *nodeinfo;
  FL_OBJECT *nodeinfotext;
  FL_OBJECT *info_nodetype;
  FL_OBJECT *info_nodeindex;
  FL_OBJECT *info_nodeposition;
  FL_OBJECT *info_nodedisplacement;
  FL_OBJECT *info_nodeoriginalpos;
  FL_OBJECT *info_nodeforce;
  FL_OBJECT *info_nodetemperature;

  TrueFalse infoflag_nodetype;
  TrueFalse infoflag_nodeindex;
  TrueFalse infoflag_nodeposition;
  TrueFalse infoflag_nodedisplacement;
  TrueFalse infoflag_nodeoriginalpos;
  TrueFalse infoflag_nodeforce;
  TrueFalse infoflag_nodetemperature;

  static TrueFalse infoflag_nodetype_dflt;
  static TrueFalse infoflag_nodeindex_dflt;
  static TrueFalse infoflag_nodeposition_dflt;
  static TrueFalse infoflag_nodedisplacement_dflt;
  static TrueFalse infoflag_nodeoriginalpos_dflt;
  static TrueFalse infoflag_nodeforce_dflt;
  static TrueFalse infoflag_nodetemperature_dflt;

  friend void FDCB_info_mouseposition2(FL_OBJECT *, long);
  friend void FDCB_info_nodetype(FL_OBJECT *, long);
  friend void FDCB_info_nodeindex(FL_OBJECT *, long);
  friend void FDCB_info_nodeposition(FL_OBJECT *, long);
  friend void FDCB_info_nodedisplacement(FL_OBJECT *, long);
  friend void FDCB_info_nodeoriginalpos(FL_OBJECT *, long);
  friend void FDCB_info_nodeforce(FL_OBJECT *, long);
  friend void FDCB_info_nodetemperature(FL_OBJECT*, long);

  void set_info_nodetype();
  void set_info_nodeindex();
  void set_info_nodeposition();
  void set_info_nodedisplacement();
  void set_info_nodeoriginalpos();
  void set_info_nodeforce();
  void set_info_nodetemperature();

  virtual void set_infobuttons() const;

  virtual void show();
  virtual void hide();

  NodeInfo(Drawer*);
  virtual ~NodeInfo();

  virtual void clickhandler(const MouseClick&);
  
  friend class MeshDrawer;
  friend void create_form_nodeinfo(NodeInfo*);
};


#endif
