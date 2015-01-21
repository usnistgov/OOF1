// -*- C++ -*-
// $RCSfile: menucolor.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2000-09-06 20:11:05 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


#include "menucolor.h"

void setmenucolors() {
  fl_mapcolor(COL_BACKGROUND, 172, 115, 127);
  //  fl_mapcolor(COL_BACKGROUND, 174, 174, 174);

// light buttons when off
 fl_mapcolor(COL_OFF_LIGHT, 31, 31, 50);
// freeze buttons when on
 fl_mapcolor(COL_FREEZE_LIGHT, 82, 82, 255);

// menuform colors
 fl_mapcolor(COL_MF_TITLE, 136, 41, 33);
 fl_mapcolor(COL_MF_RECORD_LIGHT, 255, 0, 0);
 fl_mapcolor(COL_MF_BROWTEXT, 26, 26, 205);
 fl_mapcolor(COL_MF_BROWBG, 255, 235, 205);
 fl_mapcolor(COL_MF_BROWSELECTION, 255, 255, 55);
 fl_mapcolor(COL_MF_INPUTREGION, 166, 103, 121);

// commandform colors
 fl_mapcolor(COL_CF_MAINTEXT, 245, 0, 18);
 fl_mapcolor(COL_CF_HELPTEXT, 255, 0, 0);
 fl_mapcolor(COL_CF_OTHERTEXT, 0, 0, 0);
 fl_mapcolor(COL_CF_BACK_IDLE, 47, 47, 47);
 fl_mapcolor(COL_CF_BACK_ACTIVE, 255, 0, 101);

// varobjects
 fl_mapcolor(COL_INPUT_SELECTED, 232, 207, 80);
 fl_mapcolor(COL_INPUT_UNSELECTED, 222, 222, 211);
 fl_mapcolor(COL_INPUT_BUTTON_UP, 200, 110, 207);
 fl_mapcolor(COL_INPUT_BUTTON_DOWN, 178, 25, 207);

// interruptform
 fl_mapcolor(COL_IF_STOP_UP, 255, 0, 0);
 fl_mapcolor(COL_IF_STOP_DOWN, 246, 246, 45);

// message form
 fl_mapcolor(COL_MG_TEXT, 0, 0, 0);
 fl_mapcolor(COL_MG_TEXTBG, 255, 235, 205);
 fl_mapcolor(COL_MG_BACKGROUND, 172, 115, 127);

}
