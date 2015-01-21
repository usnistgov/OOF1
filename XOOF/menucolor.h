// -*- C++ -*-
// $RCSfile: menucolor.h,v $
// $Revision: 1.3 $
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


#ifndef MENUCOLOR_H
#define MENUCOLOR_H

#include "forms.h"

void setmenucolors();

#define COL_BACKGROUND		FL_FREE_COL1

// light buttons when off
#define COL_OFF_LIGHT		FL_FREE_COL1+1
// freeze buttons when on
#define COL_FREEZE_LIGHT	FL_FREE_COL1+2

// menuform colors
#define COL_MF_TITLE		FL_FREE_COL1+3
#define COL_MF_RECORD_LIGHT	FL_FREE_COL1+4
#define COL_MF_BROWTEXT		FL_FREE_COL1+5
#define COL_MF_BROWBG		FL_FREE_COL1+6
#define COL_MF_BROWSELECTION	FL_FREE_COL1+7
#define COL_MF_INPUTREGION	FL_FREE_COL1+8

// commandform colors
#define COL_CF_MAINTEXT		FL_FREE_COL1+9
#define COL_CF_HELPTEXT		FL_FREE_COL1+10
#define COL_CF_OTHERTEXT	FL_FREE_COL1+11
#define COL_CF_BACK_IDLE	FL_FREE_COL1+12
#define COL_CF_BACK_ACTIVE	FL_FREE_COL1+13

// varobjects
#define COL_INPUT_SELECTED	FL_FREE_COL1+14
#define COL_INPUT_UNSELECTED	FL_FREE_COL1+15
#define COL_INPUT_BUTTON_UP     FL_FREE_COL1+16
#define COL_INPUT_BUTTON_DOWN   FL_FREE_COL1+17

// interruptform
#define COL_IF_STOP_UP		FL_FREE_COL1+18
#define COL_IF_STOP_DOWN	FL_FREE_COL1+19

// message form
#define COL_MG_TEXT		FL_FREE_COL1+20
#define COL_MG_TEXTBG		FL_FREE_COL1+21
#define COL_MG_BACKGROUND	FL_FREE_COL1+22	

#define XMENU_FREE_COL		FL_FREE_COL1+23

#endif
