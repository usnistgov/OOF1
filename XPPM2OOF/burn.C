// -*- C++ -*-
// $RCSfile: burn.C,v $
// $Revision: 1.6 $
// $Author: langer $
// $Date: 2000-10-30 19:23:23 $


/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


// These routines set the parameters for the burn selection
// methods. The actual burning is done in burner.C, and is initiated
// by mouseclicks via SelectDashboard::clickhandler() in selectdash.C.

#include <assert.h>
#include "ctype.h"
#include <stdio.h>
#include "stdlib.h"
#include <unistd.h>
#include "menuDef.h"
#include "burn.h"
#include "color.h"
#include "gfx.h"
#include "goof.h"
#include "imageform.h"
#include "ppm2oof.h"
#include "tocharstring.h"


int Burn::lcl_dflt(10);
int Burn::glbl_dflt(15);
TrueFalse Burn::L2_dflt(0);
TrueFalse Burn::nextnear_dflt(1);

BurnCmd::BurnCmd(Burn *b)
  : CommandM("burn", "select pixels by burning"),
    burner(b)
{
  AddArgument(this, "local_flmblty", burner->lcl);
  AddArgument(this, "global_flmblty", burner->glbl);
  AddArgument(this, "use_L2_norm", burner->use_L2norm );
  AddArgument(this, "burn_next_near", burner->next_nearest);
}

CommandFn BurnCmd::func() {
  burner->lcl_dflt = burner->lcl;
  burner->glbl_dflt = burner->glbl;
  burner->L2_dflt = burner->use_L2norm;
  burner->nextnear_dflt = burner->next_nearest;
}

CommandM *BurnCmd::clone() const {
  BurnCmd *bc = new BurnCmd(burner);
  return bc;
}

Burn::Burn()
  : lcl(lcl_dflt),
    glbl(glbl_dflt),
    use_L2norm(L2_dflt)
{
  next_nearest = nextnear_dflt;
}

int Burn::spread(const Color &color1, const Color &color2) const
{
  int local_dist, global_dist;
  if(use_L2norm) {	// using L2 norm
    local_dist = L2_dist2(color1, color2);
    global_dist = L2_dist2(startingcolor, color2);
    return (local_dist < lcl*lcl && global_dist < glbl*glbl);
  }
  // not using L2 norm
  local_dist = L1_dist(color1, color2);
  global_dist = L1_dist(startingcolor, color2);
  return (local_dist < lcl && global_dist < glbl);
}

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

int RGBBurn::redflm_dflt(10);
int RGBBurn::grnflm_dflt(10);
int RGBBurn::bluflm_dflt(10);
TrueFalse RGBBurn::nextnear_dflt(1);

RGBBurnCmd::RGBBurnCmd(RGBBurn *b)
  : CommandM("rgb_burn", "select pixels by burning"),
    burner(b)
{
  AddArgument(this, "red_flmblty", burner->redflm);
  AddArgument(this, "green_flmblty", burner->grnflm);
  AddArgument(this, "blue_flmblty", burner->bluflm);
  AddArgument(this, "burn_next_near", burner->next_nearest);
}

CommandFn RGBBurnCmd::func() {
  burner->redflm_dflt = burner->redflm;
  burner->grnflm_dflt = burner->grnflm;
  burner->bluflm_dflt = burner->bluflm;
  burner->nextnear_dflt = burner->next_nearest;
}

CommandM *RGBBurnCmd::clone() const {
  RGBBurnCmd *bc = new RGBBurnCmd(burner);
  return bc;
}

RGBBurn::RGBBurn()
  : redflm(redflm_dflt),
    grnflm(grnflm_dflt),
    bluflm(bluflm_dflt)
{
  next_nearest = nextnear_dflt;
}

int RGBBurn::spread(const Color &color1, const Color &color2) const {
  if(abs(color1.query_red() - color2.query_red()) > redflm) return 0;
  if(abs(color1.query_blue() - color2.query_blue()) > bluflm) return 0;
  if(abs(color1.query_green() - color2.query_green()) > grnflm) return 0;
  return 1;
}



//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

// void normal_burn(const Cell_coordinate &where, int shiftkey) {
//   do_compare = compare_col;
//   burn_from(where, shiftkey);
// }

// void rgb_burn(const Cell_coordinate &where, int shiftkey) {
//   do_compare = compare_rgb;
//   burn_from(where, shiftkey);
// }

//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//=\\=//

CommandFn set_firewall() {
  if(goofinit())
    current_goof->set_firewall();
}

CommandFn clear_firewall() {
  if(goofinit())
    current_goof->clear_firewall();
}
