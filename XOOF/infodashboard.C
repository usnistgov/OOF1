// -*- C++ -*-
// $RCSfile: infodashboard.C,v $
// $Revision: 1.7 $
// $Author: langer $
// $Date: 2000-11-20 21:11:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "colorutils.h"
#include "create_form_drawer.h"
#include "delete_fl_group.h"
#include "element.h"
#include "eulerangle.h"
#include "formdrawer.h"
#include "graphics.h"
#include "grid.h"
#include "infodashboard.h"
#include "menuDef.h"
#include "mouseclick.h"
#include "mvmult.h"
#include "node.h"
#include "parameters.h"
#include "symeig3.h"
#include <iomanip.h>
#include <math.h>

TrueFalse InfoDashboard::infoflag_mouseposition_dflt(0);

InfoDashboard::InfoDashboard(const CharString &nm, Drawer *drwr)
  : Dashboard(nm, drwr, RB_NONE),
    infoflag_mouseposition(infoflag_mouseposition_dflt)
{
  // mouse position button is initialized in set_infobuttons().
  AddVariable(&menu, "mouseposition", "mouse position", infoflag_mouseposition,
	      set_infobuttons_cmd);
}

InfoDashboard::~InfoDashboard() {
  menu.RemoveOption("mouseposition");
}

void ElementInfo::show() {
  fl_show_object(elementinfo);
  setcursor(*infocursor);
}

void ElementInfo::hide() {
  fl_hide_object(elementinfo);
  unsetcursor();
}

void InfoDashboard::set_info_mouseposition() {
  infoflag_mouseposition = infoflag_mouseposition_dflt =
    fl_get_button(info_mouseposition);
  garcon()->log(menucommand() + "mouseposition="
		+ to_charstring(infoflag_mouseposition));
}

void FDCB_info_mouseposition(FL_OBJECT*, long ptr) {
  ((InfoDashboard*) ptr)->set_info_mouseposition();
}

void InfoDashboard::set_infobuttons_cmd(const CharString&) {
  ((InfoDashboard*) get_dashboard())->set_infobuttons();
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

TrueFalse ElementInfo::infoflag_elementtype_dflt(1);
TrueFalse ElementInfo::infoflag_nodeindices_dflt(0);
TrueFalse ElementInfo::infoflag_elementindex_dflt(0);
TrueFalse ElementInfo::infoflag_intrinsicgray_dflt(0);
TrueFalse ElementInfo::infoflag_elementparams_dflt(0);
TrueFalse ElementInfo::infoflag_stress_dflt(0);
TrueFalse ElementInfo::infoflag_strain_dflt(0);
TrueFalse ElementInfo::infoflag_components_dflt(1);
TrueFalse ElementInfo::infoflag_invariants_dflt(0);
TrueFalse ElementInfo::infoflag_eigenvalues_dflt(0);
TrueFalse ElementInfo::infoflag_energydensity_dflt(0);
#ifdef THERMAL
TrueFalse ElementInfo::infoflag_heatflux_dflt(0);
TrueFalse ElementInfo::infoflag_tempgradient_dflt(0);
TrueFalse ElementInfo::infoflag_temperature_dflt(0);
#endif // THERMAL

ElementInfo::ElementInfo(Drawer *drwr)
  : InfoDashboard("Element Info", drwr),
    infoflag_elementtype(infoflag_elementtype_dflt),      
    infoflag_nodeindices(infoflag_nodeindices_dflt),      
    infoflag_elementindex(infoflag_elementindex_dflt),      
    infoflag_intrinsicgray(infoflag_intrinsicgray_dflt),    
    infoflag_elementparams(infoflag_elementparams_dflt),    
    infoflag_stress(infoflag_stress_dflt),           
    infoflag_strain(infoflag_strain_dflt),           
    infoflag_components(infoflag_components_dflt),               
    infoflag_invariants(infoflag_invariants_dflt),             
    infoflag_eigenvalues(infoflag_eigenvalues_dflt),
    infoflag_energydensity(infoflag_energydensity_dflt)
#ifdef THERMAL
,
    infoflag_heatflux(infoflag_heatflux_dflt),
    infoflag_tempgradient(infoflag_tempgradient_dflt),
    infoflag_temperature(infoflag_temperature_dflt)
#endif // THERMAL
{
  create_form_elementinfo(this);
  set_infobuttons();


  AddVariable(&menu, "elementtype", "element type", infoflag_elementtype,
	      set_infobuttons_cmd);
  AddVariable(&menu, "nodeindices", "node indices", infoflag_nodeindices,
	      set_infobuttons_cmd); 
  AddVariable(&menu, "elementindex", "element index", infoflag_elementindex,
	      set_infobuttons_cmd); 
  AddVariable(&menu, "gray", "intrinsic gray", infoflag_intrinsicgray,
	      set_infobuttons_cmd);
  AddVariable(&menu, "params", "element parameters", infoflag_elementparams,
	      set_infobuttons_cmd);
  AddVariable(&menu, "stress", "stress", infoflag_stress, set_infobuttons_cmd);
  AddVariable(&menu, "strain", "strain", infoflag_strain, set_infobuttons_cmd);
  AddVariable(&menu, "components", "tensor components", infoflag_components,
	      set_infobuttons_cmd);
  AddVariable(&menu, "invariants", "tensor invariants", infoflag_invariants,
	      set_infobuttons_cmd);
  AddVariable(&menu, "eigenvalues", "eigenvalues", infoflag_eigenvalues,
	      set_infobuttons_cmd);
  AddVariable(&menu, "energydensity", "energy density", infoflag_energydensity,
	      set_infobuttons_cmd);
#ifdef THERMAL
  AddVariable(&menu, "heatflux", "heat flux", infoflag_heatflux,
	      set_infobuttons_cmd);
  AddVariable(&menu, "tempgradient", "temperature gradient",
	      infoflag_tempgradient, set_infobuttons_cmd);
  AddVariable(&menu, "temperature", "temperature", infoflag_temperature,
	      set_infobuttons_cmd);
#endif // THERMAL
}

ElementInfo::~ElementInfo() {
  hide();
  delete_fl_group(elementinfo);
  /*
  fl_delete_object(elementinfo);
  fl_free_object(info_mouseposition);
  fl_free_object(infoframe1);
  fl_free_object(elementinfotext);
  fl_free_object(info_elementtype);
  fl_free_object(info_nodeindices);
  fl_free_object(info_intrinsicgray);
  fl_free_object(info_elementparams);
  fl_free_object(info_stress);
  fl_free_object(info_strain);
  fl_free_object(info_11);
  fl_free_object(info_22);
  fl_free_object(info_12);
  fl_free_object(info_inv1);
  fl_free_object(info_inv2);
  fl_free_object(info_eigenvalues);
  fl_free_object(elementinfo);
  */
  menu.RemoveOption("elementtype");
  menu.RemoveOption("nodeindices");
  menu.RemoveOption("elementindex");
  menu.RemoveOption("gray");
  menu.RemoveOption("params");
  menu.RemoveOption("stress");
  menu.RemoveOption("strain");
  menu.RemoveOption("components");
  menu.RemoveOption("invariants");
  menu.RemoveOption("eigenvalues");
  menu.RemoveOption("energydensity");
#ifdef THERMAL
  menu.RemoveOption("heatflux");
  menu.RemoveOption("tempgradient");
  menu.RemoveOption("temperature");
#endif // THERMAL
}

void ElementInfo::set_infobuttons() const {
  fl_set_button(info_mouseposition, infoflag_mouseposition);
  fl_set_button(info_elementtype, infoflag_elementtype);
  fl_set_button(info_elementindex, infoflag_elementindex);
  fl_set_button(info_nodeindices, infoflag_nodeindices);
  fl_set_button(info_intrinsicgray, infoflag_intrinsicgray);
  fl_set_button(info_elementparams, infoflag_elementparams);
  fl_set_button(info_stress, infoflag_stress);
  fl_set_button(info_strain, infoflag_strain);
  fl_set_button(info_components, infoflag_components);
  fl_set_button(info_invariants, infoflag_invariants);
  fl_set_button(info_eigenvalues, infoflag_eigenvalues);
  fl_set_button(info_energydensity, infoflag_energydensity);
#ifdef THERMAL
  fl_set_button(info_heatflux, infoflag_heatflux);
  fl_set_button(info_tempgradient, infoflag_tempgradient);
  fl_set_button(info_temperature, infoflag_temperature);
#endif // THERMAL
}

void ElementInfo::set_info_elementtype() {
  infoflag_elementtype = infoflag_elementtype_dflt = 
    fl_get_button(info_elementtype);
  garcon()->log(menucommand() + "elementtype=" +
		to_charstring(infoflag_elementtype));
}

void FDCB_info_elementtype(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_elementtype();
}

void ElementInfo::set_info_nodeindices() {
  infoflag_nodeindices = infoflag_nodeindices_dflt =
    fl_get_button(info_nodeindices);
  garcon()->log(menucommand() + "nodeindices=" +
		to_charstring(infoflag_nodeindices));
}

void FDCB_info_nodeindices(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_nodeindices();
}

void ElementInfo::set_info_elementindex() {
  infoflag_elementindex = infoflag_elementindex_dflt =
    fl_get_button(info_elementindex);
  garcon()->log(menucommand() + "elementindex=" +
		to_charstring(infoflag_elementindex));
}

void FDCB_info_elementindex(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_elementindex();
}

void ElementInfo::set_info_intrinsicgray() {
  infoflag_intrinsicgray = infoflag_intrinsicgray_dflt =
    fl_get_button(info_intrinsicgray);
  garcon()->log(menucommand() + "gray="
		+ to_charstring(infoflag_intrinsicgray));
}

void FDCB_info_intrinsicgray(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_intrinsicgray();
}

void ElementInfo::set_info_elementparams() {
  infoflag_elementparams = infoflag_elementparams_dflt =
    fl_get_button(info_elementparams);
  garcon()->log(menucommand() + "params="
		+ to_charstring(infoflag_elementparams));
}

void FDCB_info_elementparams(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_elementparams();
}

void ElementInfo::set_info_stress() {
  infoflag_stress = infoflag_stress_dflt =
    fl_get_button(info_stress);
  garcon()->log(menucommand() + "stress=" + to_charstring(infoflag_stress));
}

void FDCB_info_stress(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_stress();
}

void ElementInfo::set_info_strain() {
  infoflag_strain = infoflag_strain_dflt =
    fl_get_button(info_strain);
  garcon()->log(menucommand() + "strain=" + to_charstring(infoflag_strain));
}

void FDCB_info_strain(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_strain();
}

void ElementInfo::set_info_components() {
  infoflag_components = infoflag_components_dflt
    = fl_get_button(info_components);
  garcon()->log(menucommand() + "components=" +
		to_charstring(infoflag_components));
}

void FDCB_info_components(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_components();
}

void ElementInfo::set_info_invariants() {
  infoflag_invariants = infoflag_invariants_dflt
    = fl_get_button(info_invariants);
  garcon()->log(menucommand() + "invariants=" +
		to_charstring(infoflag_invariants));
}

void FDCB_info_invariants(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_invariants();
}

void ElementInfo::set_info_eigenvalues() {
  infoflag_eigenvalues = infoflag_eigenvalues_dflt =
    fl_get_button(info_eigenvalues);
  garcon()->log(menucommand() + "eigenvalues=" +
		to_charstring(infoflag_eigenvalues));
}

void FDCB_info_eigenvalues(FL_OBJECT *, long ptr) {
  ((ElementInfo*) ptr)->set_info_eigenvalues();
}

void ElementInfo::set_info_energydensity() {
  infoflag_energydensity = infoflag_energydensity_dflt =
    fl_get_button(info_energydensity);
  garcon()->log(menucommand() + "energydensity=" +
		to_charstring(infoflag_energydensity));
}

void FDCB_info_energydensity(FL_OBJECT*, long ptr) {
  ((ElementInfo*) ptr)->set_info_energydensity();
}

#ifdef THERMAL
void ElementInfo::set_info_heatflux() {
  infoflag_heatflux = infoflag_heatflux_dflt = 
    fl_get_button(info_heatflux);
  garcon()->log(menucommand() + "heatflux=" +
		to_charstring(infoflag_heatflux));
}

void FDCB_info_heatflux(FL_OBJECT*, long ptr) {
  ((ElementInfo*) ptr)->set_info_heatflux();
}

void ElementInfo::set_info_tempgradient() {
  infoflag_tempgradient = infoflag_tempgradient_dflt = 
    fl_get_button(info_tempgradient);
  garcon()->log(menucommand() + "tempgradient=" +
		to_charstring(infoflag_tempgradient));
}

void FDCB_info_tempgradient(FL_OBJECT*, long ptr) {
  ((ElementInfo*) ptr)->set_info_tempgradient();
}

void ElementInfo::set_info_temperature() {
  infoflag_temperature = infoflag_temperature_dflt = 
    fl_get_button(info_temperature);
  garcon()->log(menucommand() + "temperature=" +
		to_charstring(infoflag_temperature));
}

void FDCB_info_temperature(FL_OBJECT*, long ptr) {
  ((ElementInfo*) ptr)->set_info_temperature();
}
#endif // THERMAL


void ElementInfo::clickhandler(const MouseClick &click) {
  if(!gridinit()) return;
  garcon()->msout << "-----------" << endl;

  if(infoflag_mouseposition)
    garcon()->msout << "   mouse at x=" << click.coord.x
		    << " y=" << click.coord.y << endl;

  Element *e = grid->findElement(click.coord);
  if(!e || (e->isempty() && !drawer->showingempties())) return;
  if(infoflag_elementtype)
    garcon()->msout << "   type: " << e->tag() << endl;
  if(infoflag_elementindex)
    garcon()->msout << "  index: " << e->index << endl;
  if(infoflag_nodeindices) {
    garcon()->msout << "  nodes: ";
    for(int i=0; i<e->corner.capacity(); i++) {
      garcon()->msout << " " << e->corner[i]->index;
    }
    garcon()->msout << endl;
  }
  if(infoflag_intrinsicgray)
    garcon()->msout << "   gray: " << e->intrinsic_gray << endl;
  if(infoflag_elementparams) 
    garcon()->msout << " params:" << e->parameters() << endl;
  if(infoflag_stress) {
    garcon()->msout << " stress:" << endl;
    infostressstrain(e->stresstensor());
  }
  if(infoflag_strain) {
    garcon()->msout << " strain:" << endl;
    infostressstrain(e->total_straintensor());
  }
  if(infoflag_energydensity)
    garcon()->msout << " energy density: " << e->elastic_energy() << endl;
#ifdef THERMAL
  if(infoflag_temperature)
    garcon()->msout << "     temperature: " << e->average_T() << endl;
  if(infoflag_tempgradient)
    garcon()->msout << "grad temperature: " << e->gradient_temperature_field()
		    << endl;
  if(infoflag_heatflux)
    garcon()->msout << "       heat flux: " << e->heat_flux_field() << endl;
#endif // THERMAL
}

void ElementInfo::infostressstrain(const StrainStress &str) const {
  if(infoflag_components)
    str.fancy_output(garcon()->msout,"   ") << endl;
  if(infoflag_invariants) {
    garcon()->msout << "    invariant 1: " << str.invariant1() << endl;
    garcon()->msout << "    invariant 2: " << str.invariant2() << endl;
    garcon()->msout << "    invariant 3: " << str.invariant3() << endl;
  }
  if(infoflag_eigenvalues) {
    EigenVector eig1, eig2, eig3;
    eigensystem(str, eig1, eig2, eig3);
    garcon()->msout << "   eigenvalue  orientation" << endl
 		    << setw(13) << eig1.eigenvalue << "  "
		    << eig1.orientation() << endl
 		    << setw(13) << eig2.eigenvalue << "  "
		    << eig2.orientation() << endl
 		    << setw(13) << eig3.eigenvalue << "  "
		    << eig3.orientation() << endl;
    //    garcon()->msout << "  eigenvectors & eigenvalues: " << endl
    //		    << eig1 << endl << eig2 << endl << eig3 << endl;
//     MV_Vector_double test(3);
//     test[0] = eig1.eigenvector[0];
//     test[1] = eig1.eigenvector[1];
//     test[2] = eig1.eigenvector[2];
//     garcon()->msout << MV_ColMat_double(eig1.orientation())*test << endl;
//     garcon()->msout << trans_mult(MV_ColMat_double(eig1.orientation()),test) << endl;
//     garcon()->msout << str.transform(eig1.orientation()) << endl;
//     garcon()->msout << str.transform(eig2.orientation()) << endl;
//     garcon()->msout << str.transform(eig3.orientation()) << endl;
  }
}

//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//-\\-//

TrueFalse NodeInfo::infoflag_nodetype_dflt(1);
TrueFalse NodeInfo::infoflag_nodeindex_dflt(0);
TrueFalse NodeInfo::infoflag_nodeposition_dflt(1);
TrueFalse NodeInfo::infoflag_nodedisplacement_dflt(0);
TrueFalse NodeInfo::infoflag_nodeoriginalpos_dflt(0);
TrueFalse NodeInfo::infoflag_nodeforce_dflt(0);
#ifdef THERMAL
TrueFalse NodeInfo::infoflag_nodetemperature_dflt(0);
#endif // THERMAL

NodeInfo::NodeInfo(Drawer *drwr)
  : InfoDashboard("Node Info", drwr),
    infoflag_nodetype(infoflag_nodetype_dflt),
    infoflag_nodeindex(infoflag_nodeindex_dflt),
    infoflag_nodeposition(infoflag_nodeposition_dflt),
    infoflag_nodedisplacement(infoflag_nodedisplacement_dflt),
    infoflag_nodeoriginalpos(infoflag_nodeoriginalpos_dflt),
    infoflag_nodeforce(infoflag_nodeforce_dflt)
#ifdef THERMAL
,
    infoflag_nodetemperature(infoflag_nodetemperature_dflt)
#endif // THERMAL
{
  create_form_nodeinfo(this);
  set_infobuttons();
  AddVariable(&menu, "nodetype", "node type", infoflag_nodetype,
	      set_infobuttons_cmd);
  AddVariable(&menu, "nodeindex", "node index", infoflag_nodeindex,
	      set_infobuttons_cmd);
  AddVariable(&menu, "position", "node position", infoflag_nodeposition,
	      set_infobuttons_cmd);
  AddVariable(&menu, "displacement", "node displacement",
	      infoflag_nodedisplacement, set_infobuttons_cmd);
  AddVariable(&menu, "original", "node's original position",
	      infoflag_nodeoriginalpos, set_infobuttons_cmd);
  AddVariable(&menu, "force", "applied force on node",
	      infoflag_nodeforce, set_infobuttons_cmd);
#ifdef THERMAL
  AddVariable(&menu, "temperature", "temperature of a node",
	      infoflag_nodetemperature, set_infobuttons_cmd);
#endif // THERMAL
}

NodeInfo::~NodeInfo() {
  hide();
  delete_fl_group(nodeinfo);
  /*
  fl_delete_object(nodeinfo);
  fl_free_object(info_mouseposition);
  fl_free_object(nodeinfotext);
  fl_free_object(info_nodetype);
  fl_free_object(info_nodeindex);
  fl_free_object(info_nodeposition);
  fl_free_object(info_nodedisplacement);
  fl_free_object(info_nodeoriginalpos);
  fl_free_object(nodeinfo);
  */
  menu.RemoveOption("nodetype");
  menu.RemoveOption("nodeindex");
  menu.RemoveOption("position");
  menu.RemoveOption("displacement");
  menu.RemoveOption("original");
  menu.RemoveOption("force");
#ifdef THERMAL
  menu.RemoveOption("temperature");
#endif // THERMAL
}

void NodeInfo::show() {
  fl_show_object(nodeinfo);
  setcursor(*infocursor);
}

void NodeInfo::hide() {
  fl_hide_object(nodeinfo);
  unsetcursor();
}

void NodeInfo::set_infobuttons() const {
  fl_set_button(info_mouseposition, infoflag_mouseposition);
  fl_set_button(info_nodetype, infoflag_nodetype);
  fl_set_button(info_nodeindex, infoflag_nodeindex);
  fl_set_button(info_nodeposition, infoflag_nodeposition);
  fl_set_button(info_nodedisplacement, infoflag_nodedisplacement);
  fl_set_button(info_nodeforce, infoflag_nodeforce);
#ifdef THERMAL
  fl_set_button(info_nodetemperature, infoflag_nodetemperature);
#endif // THERMAL
}

void NodeInfo::set_info_nodetype() {
  infoflag_nodetype = infoflag_nodetype_dflt =
    fl_get_button(info_nodetype);
  garcon()->log(menucommand() + "nodetype=" + to_charstring(infoflag_nodetype));
}

void FDCB_info_nodetype(FL_OBJECT *, long ptr) {
  ((NodeInfo*) ptr)->set_info_nodetype();
}

void NodeInfo::set_info_nodeindex() {
  infoflag_nodeindex = infoflag_nodeindex_dflt =
    fl_get_button(info_nodeindex);
  garcon()->log(menucommand() + "nodeindex=" +
		to_charstring(infoflag_nodeindex));
}

void FDCB_info_nodeindex(FL_OBJECT *, long ptr) {
  ((NodeInfo*) ptr)->set_info_nodeindex();
}

void NodeInfo::set_info_nodeposition() {
  infoflag_nodeposition = infoflag_nodeposition_dflt =
    fl_get_button(info_nodeposition);
  garcon()->log(menucommand() + "position=" +
		to_charstring(infoflag_nodeposition));
}

void FDCB_info_nodeposition(FL_OBJECT *, long ptr) {
  ((NodeInfo*) ptr)->set_info_nodeposition();
}

void NodeInfo::set_info_nodedisplacement() {
  infoflag_nodedisplacement = infoflag_nodedisplacement_dflt =
    fl_get_button(info_nodedisplacement);
  garcon()->log(menucommand() + "displacement=" +
		to_charstring(infoflag_nodedisplacement));
}

void FDCB_info_nodedisplacement(FL_OBJECT *, long ptr) {
  ((NodeInfo*) ptr)->set_info_nodedisplacement();
}

void NodeInfo::set_info_nodeoriginalpos() {
  infoflag_nodeoriginalpos = infoflag_nodeoriginalpos_dflt =
    fl_get_button(info_nodeoriginalpos);
  garcon()->log(menucommand() + "original=" +
		to_charstring(infoflag_nodeoriginalpos));
}

void FDCB_info_nodeoriginalpos(FL_OBJECT *, long ptr) {
  ((NodeInfo*) ptr)->set_info_nodeoriginalpos();
}

void NodeInfo::set_info_nodeforce() {
  infoflag_nodeforce = infoflag_nodeforce_dflt =
    fl_get_button(info_nodeforce);
  garcon()->log(menucommand() + "force=" + to_charstring(infoflag_nodeforce));
}

void FDCB_info_nodeforce(FL_OBJECT*, long ptr) {
  ((NodeInfo*) ptr)->set_info_nodeforce();
}

#ifdef THERMAL
void NodeInfo::set_info_nodetemperature() {
  infoflag_nodetemperature = infoflag_nodetemperature_dflt =
    fl_get_button(info_nodetemperature);
  garcon()->log(menucommand() + "temperature=" + to_charstring(infoflag_nodetemperature));
}

void FDCB_info_nodetemperature(FL_OBJECT*, long ptr) {
  ((NodeInfo*) ptr)->set_info_nodetemperature();
}
#endif // THERMAL

void NodeInfo::clickhandler(const MouseClick &click) {
  if(!gridinit()) return;
  grid->fix_masters();		// make sure flags are correct in nodes
  garcon()->msout << "-----------" << endl;

  if(infoflag_mouseposition)
    garcon()->msout << "   mouse at x=" << click.coord.x
	  << " y=" << click.coord.y << endl;
  Node *n = grid->findNode(click.coord);
  if(!n) return;
  if(infoflag_nodetype)
    garcon()->msout << "    type: " << n->tag() << n->parameters() << " " <<
      n->flags() << endl;
  if(infoflag_nodeindex)
    garcon()->msout << "   index: " << n->index << endl;
  if(infoflag_nodeposition)
    garcon()->msout << "    x, y: "
	  << n->position().x << ", " << n->position().y << endl;
  if(infoflag_nodedisplacement)
    garcon()->msout << "  dx, dy: "
	  << n->displacement().x << ", " << n->displacement().y << endl;
  if(infoflag_nodeoriginalpos)
    garcon()->msout << "  x0, y0: "
	  << n->original().x << ", " << n->original().y << endl;
  if(infoflag_nodeforce) {
    MeshCoord f = n->force();
    garcon()->msout << "  fx, fy: " << f.x << ", " <<  f.y << endl;
  }
#ifdef THERMAL 
 if(infoflag_nodetemperature)
    garcon()->msout << "       T: " << n->voltage() << endl;
#endif // THERMAL
}
