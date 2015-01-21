// -*- C++ -*-
// $RCSfile: meshcmds.h,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2001-04-03 14:28:07 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */

#ifndef MESHCMDS_H
#define MESHCMDS_H

#include "menuDef.h"
#include "truefalse.h"
#include "enum.h"

#include "beginheader.h"

class MeshCoord;

Menu *meshmenu();
Menu *meshstackmenu();
void select_mesh_triangle(const MeshCoord&, int);
void select_mesh_triangles(const MeshCoord&, const MeshCoord&, int);
int meshexists();

// specifies how pixel groups get transferred to triangles
enum GroupTransferMethod {GTM_ELECTION, GTM_ALLPIXELS, GTM_CENTERPIXEL};
EXTERN Enum<GroupTransferMethod> grouptransfermethod DFLT(GTM_ELECTION);

// specifies how materials get transferred to triangles
enum MaterialTransferMethod {MTM_ELECTION, MTM_CENTERPIXEL};
EXTERN Enum<MaterialTransferMethod> materialtransfermethod DFLT(MTM_ELECTION);

// specifies when materials and groups get transferred to triangles
enum TransferTime {CONTINUALLY, EXPLICITLY};
EXTERN Enum<TransferTime> grouptransfertime DFLT(CONTINUALLY);
EXTERN Enum<TransferTime> materialtransfertime DFLT(CONTINUALLY);

// specifies whether transfers replace or add to existing groups
EXTERN TrueFalse grouptransferreplace DFLT(1);


#include "endheader.h"

#endif
