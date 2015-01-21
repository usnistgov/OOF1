// -*- C++ -*-
// $RCSfile: nodegroups.h,v $
// $Revision: 1.9 $
// $Author: langer $
// $Date: 2004-10-15 15:55:54 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#ifndef NODEGROUPS_H
#define NODEGROUPS_H

class NodeGroup;

#include "charstring.h"
#include "distort.h"
#include "freedom.h"
#include "groups.h"
#include "menuDef.h"
#include "node.h"
#include "meshcoord.h"
#include "twotwo.h"
#include <iostream.h>

class NodeGroup : public Group<Node> {
public:
  NodeGroup(const CharString &name)
    : Group<Node>(name),
    master_x(0), master_y(0)
#ifdef THERMAL
    , master_phi(0)
#endif // THERMAL
  {}
  virtual ~NodeGroup();
  FreedomFlags xflag; // which bc's are specified?
  FreedomFlags yflag;
  MasterDegreeOfFreedom *master_x;
  MasterDegreeOfFreedom *master_y;
  void fix_x();
  void free_x();
  void fix_y();
  void free_y();
  void enslavenodes_x();	// set flag for enslavement
  void enslavenodes_y();
  void emancipatenodes_x();
  void emancipatenodes_y();
  void emancipatenodes_all();
  void do_enslavenodes_x();	// actually enslave the group
  void do_enslavenodes_y();
  void do_emancipatenodes_x();
  void do_emancipatenodes_y();

  void do_emancipatenodes_elastic_all(); // calls do_emancipatenodes_x, etc.
  void do_enslavenodes_elastic_all();	// calls do_enslavenodes_x, etc.

  TwoTwo incremental_distortion; // if this code were going to survive 
  MeshCoord incremental_shift;	// the transition to OOF2, it would be  
  MeshCoord incremental_force;	// worth putting all these in a class.  
  TwoTwo absolute_distortion;
  MeshCoord absolute_shift;
  MeshCoord absolute_force;

  void apply_absolute_elastic_distortion() const;
  void clear_elastic_distortion();

  double width() const;
  double height() const;

#ifdef THERMAL
  FreedomFlags phiflag;
  MasterDegreeOfFreedom *master_phi;
  void fix_phi();
  void free_phi();
  void enslavenodes_phi();
  void emancipatenodes_phi();
  void do_enslavenodes_phi();
  void do_emancipatenodes_phi();
  void do_emancipatenodes_thermal_all();
  void do_enslavenodes_thermal_all();
  ThermalDistortion incremental_thermaldistortion;
  ThermalDistortion absolute_thermaldistortion;
  void apply_absolute_thermal_distortion() const;
  void clear_thermal_distortion();
#endif // THERMAL

  void print_bc(ostream&, const CharString&) const; // for oof2oof2

#ifdef DEBUG
  void dump(ostream &) const;
#endif
};

ostream &operator<<(ostream&, const NodeGroup&);
Menu *selectnodemenu();
Menu *nodegroupmenu();
NodeGroup *groupname();	    // returns group w/name of current command

#endif
